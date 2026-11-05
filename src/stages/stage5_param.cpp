#include "stages/stage5_param.h"
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <iostream>

namespace qremesh {

void Stage5ParamAndExtract::balance_and_quantize_patch_sizes(
    RemeshContext& ctx,
    PatchGraph& graph
) {
    if (ctx.params->target_quad_count <= 0) return;

    int64_t target = ctx.params->target_quad_count;
    int64_t current_total = 0;
    for (const auto& p : graph.patches) {
        current_total += static_cast<int64_t>(p.nu) * p.nv;
    }

    if (current_total == 0) return;

    // Rescale proportionally
    double ratio = std::sqrt(static_cast<double>(target) / static_cast<double>(current_total));
    current_total = 0;
    for (auto& p : graph.patches) {
        p.nu = std::max(1, static_cast<int>(std::round(p.nu * ratio)));
        p.nv = std::max(1, static_cast<int>(std::round(p.nv * ratio)));
        current_total += static_cast<int64_t>(p.nu) * p.nv;
    }

    // Exact adjustment if requested
    if (ctx.params->exact_quad_count) {
        int guard = 0;
        while (current_total != target && guard++ < 2000) {
            int64_t diff = target - current_total;
            if (diff > 0) {
                // Add quad to largest patch
                size_t best_p = 0;
                int max_dim = 0;
                for (size_t i = 0; i < graph.patches.size(); ++i) {
                    if (graph.patches[i].nu * graph.patches[i].nv > max_dim) {
                        max_dim = graph.patches[i].nu * graph.patches[i].nv;
                        best_p = i;
                    }
                }
                if (graph.patches[best_p].nu <= graph.patches[best_p].nv) graph.patches[best_p].nu++;
                else graph.patches[best_p].nv++;
                current_total = 0;
                for (const auto& p : graph.patches) current_total += static_cast<int64_t>(p.nu) * p.nv;
            } else {
                // Remove quad from largest patch
                size_t best_p = 0;
                int max_dim = 0;
                for (size_t i = 0; i < graph.patches.size(); ++i) {
                    if (graph.patches[i].nu > 1 && graph.patches[i].nv > 1 &&
                        graph.patches[i].nu * graph.patches[i].nv > max_dim) {
                        max_dim = graph.patches[i].nu * graph.patches[i].nv;
                        best_p = i;
                    }
                }
                if (max_dim == 0) break;
                if (graph.patches[best_p].nu >= graph.patches[best_p].nv) graph.patches[best_p].nu--;
                else graph.patches[best_p].nv--;
                current_total = 0;
                for (const auto& p : graph.patches) current_total += static_cast<int64_t>(p.nu) * p.nv;
            }
        }
    }
}

void Stage5ParamAndExtract::extract_patch_quads(
    const RemeshContext& ctx,
    const QuasiQuadPatch& patch,
    QuadMeshResult& out_quads
) {
    const auto& mesh = ctx.working_mesh;
    if (patch.member_faces.empty()) return;

    std::unordered_set<uint32_t> patch_face_set(patch.member_faces.begin(), patch.member_faces.end());

    // Find boundary halfedges of this patch (halfedges whose twin is not in patch)
    std::unordered_map<uint32_t, uint32_t> bnd_next;
    for (uint32_t f : patch.member_faces) {
        for (uint32_t h : mesh.get_face_halfedges(f)) {
            uint32_t tw = mesh.halfedges[h].twin;
            if (tw == INVALID_INDEX || !patch_face_set.count(mesh.halfedges[tw].face)) {
                uint32_t u = mesh.from_vertex(h);
                uint32_t v = mesh.halfedges[h].to_vertex;
                bnd_next[u] = v;
            }
        }
    }

    // Trace boundary loop
    std::vector<uint32_t> bnd_loop;
    if (!bnd_next.empty()) {
        uint32_t start_v = bnd_next.begin()->first;
        uint32_t curr = start_v;
        std::unordered_set<uint32_t> visited_bnd;
        while (visited_bnd.size() < bnd_next.size()) {
            bnd_loop.push_back(curr);
            visited_bnd.insert(curr);
            auto it = bnd_next.find(curr);
            if (it == bnd_next.end() || it->second == start_v || visited_bnd.count(it->second)) break;
            curr = it->second;
        }
    }

    int nu = std::max(1, patch.nu);
    int nv = std::max(1, patch.nv);
    std::vector<std::vector<uint32_t>> grid(nu + 1, std::vector<uint32_t>(nv + 1, INVALID_INDEX));

    // Build a patch-local BVH over patch.member_faces so interior points never snap to opposite thin walls
    BVHTree patch_bvh;
    std::vector<float> patch_pos;
    std::vector<uint32_t> patch_tris;
    std::unordered_map<uint32_t, uint32_t> local_v_map;
    for (uint32_t f : patch.member_faces) {
        auto f_v = mesh.get_face_vertices(f);
        if (f_v.size() >= 3) {
            uint32_t lv[3];
            for (int k = 0; k < 3; ++k) {
                auto it = local_v_map.find(f_v[k]);
                if (it == local_v_map.end()) {
                    uint32_t idx = static_cast<uint32_t>(patch_pos.size() / 3);
                    local_v_map[f_v[k]] = idx;
                    const Vec3f& p = mesh.vertices[f_v[k]].pos;
                    patch_pos.push_back(p.x());
                    patch_pos.push_back(p.y());
                    patch_pos.push_back(p.z());
                    lv[k] = idx;
                } else {
                    lv[k] = it->second;
                }
            }
            patch_tris.push_back(lv[0]);
            patch_tris.push_back(lv[1]);
            patch_tris.push_back(lv[2]);
        }
    }
    bool has_patch_bvh = patch_bvh.build(
        patch_pos.data(),
        static_cast<uint32_t>(patch_pos.size() / 3),
        patch_tris.data(),
        static_cast<uint32_t>(patch_tris.size() / 3)
    );

    // If boundary loop has >= 4 vertices, use arc-length Coons Transfinite Interpolation
    if (bnd_loop.size() >= 4) {
        size_t n_loop = bnd_loop.size();

        // Precompute cumulative arc length along bnd_loop (closed loop of n_loop segments)
        std::vector<float> cum_len(n_loop + 1, 0.0f);
        for (size_t k = 0; k < n_loop; ++k) {
            const Vec3f& pa = mesh.vertices[bnd_loop[k]].pos;
            const Vec3f& pb = mesh.vertices[bnd_loop[(k + 1) % n_loop]].pos;
            cum_len[k + 1] = cum_len[k] + (pb - pa).norm();
        }
        float total_perim = std::max(cum_len[n_loop], 1e-6f);

        // Find 4 corners along the loop at 0, 1/4, 2/4, 3/4 of total arc-length perimeter
        size_t i0 = 0;
        auto find_idx_at_arc = [&](float target_s, size_t min_idx) -> size_t {
            size_t best = min_idx;
            float best_diff = std::numeric_limits<float>::infinity();
            for (size_t k = min_idx; k < n_loop; ++k) {
                float d = std::abs(cum_len[k] - target_s);
                if (d < best_diff) {
                    best_diff = d;
                    best = k;
                }
            }
            return best;
        };

        size_t i1 = std::max<size_t>(1, find_idx_at_arc(0.25f * total_perim, 1));
        size_t i2 = std::max<size_t>(i1 + 1, find_idx_at_arc(0.50f * total_perim, std::min(i1 + 1, n_loop - 1)));
        size_t i3 = std::max<size_t>(i2 + 1, find_idx_at_arc(0.75f * total_perim, std::min(i2 + 1, n_loop - 1)));
        if (i3 >= n_loop) i3 = n_loop - 1;

        // Arc-length parameterized sampling between start_idx and end_idx forward along bnd_loop
        auto sample_curve_forward = [&](size_t start_idx, size_t end_idx, float t) -> Vec3f {
            t = clampf(t, 0.0f, 1.0f);
            size_t segs = (end_idx >= start_idx) ? (end_idx - start_idx) : (n_loop - start_idx + end_idx);
            if (segs == 0) return mesh.vertices[bnd_loop[start_idx]].pos;

            std::vector<float> seg_cum(segs + 1, 0.0f);
            for (size_t s = 0; s < segs; ++s) {
                const Vec3f& pa = mesh.vertices[bnd_loop[(start_idx + s) % n_loop]].pos;
                const Vec3f& pb = mesh.vertices[bnd_loop[(start_idx + s + 1) % n_loop]].pos;
                seg_cum[s + 1] = seg_cum[s] + (pb - pa).norm();
            }
            float seg_tot = seg_cum[segs];
            if (seg_tot <= 1e-7f) return mesh.vertices[bnd_loop[start_idx]].pos;

            float target_d = t * seg_tot;
            auto it = std::lower_bound(seg_cum.begin(), seg_cum.end(), target_d);
            size_t step = static_cast<size_t>(std::distance(seg_cum.begin(), it));
            if (step > 0) step--;
            if (step >= segs) step = segs - 1;

            float seg_len = seg_cum[step + 1] - seg_cum[step];
            float frac = (seg_len > 1e-7f) ? clampf((target_d - seg_cum[step]) / seg_len, 0.0f, 1.0f) : 0.0f;

            size_t idx_a = (start_idx + step) % n_loop;
            size_t idx_b = (start_idx + step + 1) % n_loop;
            return (1.0f - frac) * mesh.vertices[bnd_loop[idx_a]].pos + frac * mesh.vertices[bnd_loop[idx_b]].pos;
        };

        Vec3f p00 = mesh.vertices[bnd_loop[i0]].pos; // (u=0, v=0)
        Vec3f p10 = mesh.vertices[bnd_loop[i1]].pos; // (u=1, v=0)
        Vec3f p11 = mesh.vertices[bnd_loop[i2]].pos; // (u=1, v=1)
        Vec3f p01 = mesh.vertices[bnd_loop[i3]].pos; // (u=0, v=1)

        for (int i = 0; i <= nu; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(nu);
            Vec3f c0 = sample_curve_forward(i0, i1, u); // bottom (v=0)
            Vec3f c2 = sample_curve_forward(i2, i3, 1.0f - u); // top (v=1)

            for (int j = 0; j <= nv; ++j) {
                float v = static_cast<float>(j) / static_cast<float>(nv);
                Vec3f c3 = sample_curve_forward(i3, i0, 1.0f - v); // left (u=0)
                Vec3f c1 = sample_curve_forward(i1, i2, v); // right (u=1)

                // Coons Transfinite Interpolation
                Vec3f corner_bilinear = (1.0f - u) * (1.0f - v) * p00 +
                                         u * (1.0f - v) * p10 +
                                         u * v * p11 +
                                         (1.0f - u) * v * p01;

                Vec3f initial_p = (1.0f - v) * c0 + v * c2 + (1.0f - u) * c3 + u * c1 - corner_bilinear;
                Vec3f projected_p = initial_p;

                bool is_on_patch_border = (i == 0 || i == nu || j == 0 || j == nv);
                if (!is_on_patch_border) {
                    Vec3f patch_pt, patch_norm;
                    float d2_patch;
                    if (has_patch_bvh && patch_bvh.closest_point(initial_p, patch_pt, patch_norm, d2_patch)) {
                        projected_p = patch_pt;
                        // Refine onto original high-res BVH only if close and normal-compatible
                        Vec3f orig_pt, orig_norm;
                        float d2_orig;
                        if (ctx.original_bvh.closest_point(patch_pt, orig_pt, orig_norm, d2_orig)) {
                            if (d2_orig <= 0.25f * ctx.base_target_length * ctx.base_target_length &&
                                orig_norm.dot(patch_norm) > 0.3f) {
                                projected_p = orig_pt;
                            }
                        }
                    } else if (ctx.original_bvh.closest_point(initial_p, patch_pt, patch_norm, d2_patch)) {
                        projected_p = patch_pt;
                    }
                }

                uint32_t new_v_idx = static_cast<uint32_t>(out_quads.positions.size());
                out_quads.positions.push_back(projected_p);
                grid[i][j] = new_v_idx;
            }
        }
    } else {
        // Fallback: tangent plane projection
        Vec3f centroid = Vec3f::Zero();
        for (uint32_t f : patch.member_faces) {
            centroid += mesh.compute_face_centroid(f);
        }
        centroid /= static_cast<float>(patch.member_faces.size());

        Vec3f d1 = ctx.cross_field_dir1[patch.member_faces[0]];
        Vec3f d2 = ctx.cross_field_dir2[patch.member_faces[0]];
        float side = std::sqrt(std::max(mesh.compute_face_area(patch.member_faces[0]), 1e-5f)) * static_cast<float>(nu);

        for (int i = 0; i <= nu; ++i) {
            float u_pos = (static_cast<float>(i) / static_cast<float>(nu) - 0.5f) * side;
            for (int j = 0; j <= nv; ++j) {
                float v_pos = (static_cast<float>(j) / static_cast<float>(nv) - 0.5f) * side;
                Vec3f initial_p = centroid + u_pos * d1 + v_pos * d2;

                Vec3f projected_p, norm;
                float d2_dist;
                if (!ctx.original_bvh.closest_point(initial_p, projected_p, norm, d2_dist)) {
                    projected_p = initial_p;
                }

                uint32_t new_v_idx = static_cast<uint32_t>(out_quads.positions.size());
                out_quads.positions.push_back(projected_p);
                grid[i][j] = new_v_idx;
            }
        }
    }

    // Assemble quads
    for (int i = 0; i < nu; ++i) {
        for (int j = 0; j < nv; ++j) {
            uint32_t v0 = grid[i][j];
            uint32_t v1 = grid[i + 1][j];
            uint32_t v2 = grid[i + 1][j + 1];
            uint32_t v3 = grid[i][j + 1];

            out_quads.polygons.push_back({v0, v1, v2, v3});
            out_quads.material_ids.push_back(patch.material_id);
        }
    }
}

void Stage5ParamAndExtract::weld_and_inherit_attributes(
    RemeshContext& ctx,
    QuadMeshResult& out_quads
) {
    if (out_quads.positions.empty() || out_quads.polygons.empty()) return;

    // Weld vertices across patch boundaries
    float weld_tol = 0.15f * ctx.base_target_length;
    float inv_cell = 1.0f / std::max(weld_tol, 1e-6f);

    std::unordered_map<int64_t, std::vector<uint32_t>> spatial_hash;
    auto get_key = [inv_cell](int64_t x, int64_t y, int64_t z) -> int64_t {
        return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
    };

    std::vector<uint32_t> remap(out_quads.positions.size());
    std::vector<Vec3f> unique_pos;
    unique_pos.reserve(out_quads.positions.size());

    for (size_t i = 0; i < out_quads.positions.size(); ++i) {
        const Vec3f& p = out_quads.positions[i];
        int64_t bx = static_cast<int64_t>(std::floor(p.x() * inv_cell));
        int64_t by = static_cast<int64_t>(std::floor(p.y() * inv_cell));
        int64_t bz = static_cast<int64_t>(std::floor(p.z() * inv_cell));

        uint32_t match = INVALID_INDEX;
        float best_d2 = weld_tol * weld_tol;

        // Search 3x3x3 neighboring cells
        for (int64_t dx = -1; dx <= 1 && match == INVALID_INDEX; ++dx) {
            for (int64_t dy = -1; dy <= 1 && match == INVALID_INDEX; ++dy) {
                for (int64_t dz = -1; dz <= 1; ++dz) {
                    int64_t key = get_key(bx + dx, by + dy, bz + dz);
                    auto it = spatial_hash.find(key);
                    if (it != spatial_hash.end()) {
                        for (uint32_t cand : it->second) {
                            float d2 = (unique_pos[cand] - p).squaredNorm();
                            if (d2 <= best_d2) {
                                best_d2 = d2;
                                match = cand;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (match == INVALID_INDEX) {
            match = static_cast<uint32_t>(unique_pos.size());
            unique_pos.push_back(p);
            spatial_hash[get_key(bx, by, bz)].push_back(match);
        }
        remap[i] = match;
    }

    out_quads.positions = std::move(unique_pos);

    // Remap polygon indices and filter degenerates created by welding
    std::vector<std::vector<uint32_t>> valid_polys;
    std::vector<uint16_t> valid_mats;
    valid_polys.reserve(out_quads.polygons.size());
    valid_mats.reserve(out_quads.material_ids.size());

    for (size_t p_idx = 0; p_idx < out_quads.polygons.size(); ++p_idx) {
        auto& poly = out_quads.polygons[p_idx];
        for (auto& v : poly) {
            v = remap[v];
        }

        // Remove consecutive duplicates (including wrap-around)
        std::vector<uint32_t> cleaned;
        cleaned.reserve(poly.size());
        for (size_t i = 0; i < poly.size(); ++i) {
            uint32_t curr_v = poly[i];
            uint32_t next_v = poly[(i + 1) % poly.size()];
            if (curr_v != next_v) {
                cleaned.push_back(curr_v);
            }
        }

        // Check that all remaining vertices in polygon are distinct and count >= 3
        if (cleaned.size() >= 3) {
            bool all_distinct = true;
            for (size_t i = 0; i < cleaned.size() && all_distinct; ++i) {
                for (size_t j = i + 1; j < cleaned.size(); ++j) {
                    if (cleaned[i] == cleaned[j]) {
                        all_distinct = false;
                        break;
                    }
                }
            }
            if (all_distinct && (cleaned.size() == 4 || ctx.params->allow_triangles)) {
                valid_polys.push_back(std::move(cleaned));
                valid_mats.push_back(p_idx < out_quads.material_ids.size() ? out_quads.material_ids[p_idx] : 0);
            }
        }
    }

    out_quads.polygons = std::move(valid_polys);
    out_quads.material_ids = std::move(valid_mats);

    // Detect and inherit sharp / seam edges via Spatial Hash (O(1) per edge)
    out_quads.sharp_edges.clear();
    out_quads.crease_values.clear();
    out_quads.seam_edges.clear();

    const auto& orig_mesh = ctx.working_mesh;

    // Index only sharp and seam edges of working mesh into spatial grid
    float edge_cell_size = std::max(ctx.base_target_length, 1e-4f);
    float inv_edge_cell = 1.0f / edge_cell_size;

    auto get_edge_key = [inv_edge_cell](int64_t x, int64_t y, int64_t z) -> int64_t {
        return (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
    };

    std::unordered_map<int64_t, std::vector<uint32_t>> feature_edge_grid;
    for (size_t e = 0; e < orig_mesh.edges.size(); ++e) {
        if (!orig_mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (!orig_mesh.edges[e].is_sharp && !orig_mesh.edges[e].is_seam) continue;

        Vec3f mid = orig_mesh.compute_edge_midpoint(static_cast<uint32_t>(e));
        int64_t gx = static_cast<int64_t>(std::floor(mid.x() * inv_edge_cell));
        int64_t gy = static_cast<int64_t>(std::floor(mid.y() * inv_edge_cell));
        int64_t gz = static_cast<int64_t>(std::floor(mid.z() * inv_edge_cell));
        feature_edge_grid[get_edge_key(gx, gy, gz)].push_back(static_cast<uint32_t>(e));
    }

    std::unordered_set<uint64_t> processed_quad_edges;
    float max_dist = 0.5f * ctx.base_target_length;
    float max_dist_sq = max_dist * max_dist;

    for (const auto& poly : out_quads.polygons) {
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t va = poly[i];
            uint32_t vb = poly[(i + 1) % n];
            if (va == vb) continue;
            uint32_t u0 = std::min(va, vb);
            uint32_t u1 = std::max(va, vb);
            uint64_t edge_key = (static_cast<uint64_t>(u0) << 32) | u1;

            if (processed_quad_edges.count(edge_key)) continue;
            processed_quad_edges.insert(edge_key);

            Vec3f mid = (out_quads.positions[u0] + out_quads.positions[u1]) * 0.5f;
            int64_t gx = static_cast<int64_t>(std::floor(mid.x() * inv_edge_cell));
            int64_t gy = static_cast<int64_t>(std::floor(mid.y() * inv_edge_cell));
            int64_t gz = static_cast<int64_t>(std::floor(mid.z() * inv_edge_cell));

            bool found = false;
            for (int64_t dx = -1; dx <= 1 && !found; ++dx) {
                for (int64_t dy = -1; dy <= 1 && !found; ++dy) {
                    for (int64_t dz = -1; dz <= 1 && !found; ++dz) {
                        auto it = feature_edge_grid.find(get_edge_key(gx + dx, gy + dy, gz + dz));
                        if (it != feature_edge_grid.end()) {
                            for (uint32_t fe : it->second) {
                                Vec3f orig_mid = orig_mesh.compute_edge_midpoint(fe);
                                if ((mid - orig_mid).squaredNorm() <= max_dist_sq) {
                                    if (orig_mesh.edges[fe].is_sharp) {
                                        out_quads.sharp_edges.emplace_back(u0, u1);
                                        out_quads.crease_values.push_back(orig_mesh.edges[fe].crease);
                                    }
                                    if (orig_mesh.edges[fe].is_seam) {
                                        out_quads.seam_edges.emplace_back(u0, u1);
                                    }
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

QrmStatus Stage5ParamAndExtract::execute(
    RemeshContext& ctx,
    PatchGraph& graph,
    QuadMeshResult& out_quads
) {
    if (!ctx.notify_progress(0.70f, "Stage 5: Integer Parametrization & Quad Assembly")) {
        return QRM_ERR_CANCELLED;
    }

    balance_and_quantize_patch_sizes(ctx, graph);

    out_quads.positions.clear();
    out_quads.polygons.clear();
    out_quads.material_ids.clear();

    for (const auto& patch : graph.patches) {
        extract_patch_quads(ctx, patch, out_quads);
    }

    if (out_quads.polygons.empty()) {
        return QRM_ERR_PARAM_FAILED;
    }

    weld_and_inherit_attributes(ctx, out_quads);

    ctx.dump_quads_obj("stage5_extracted", out_quads);
    return QRM_OK;
}

} // namespace qremesh
