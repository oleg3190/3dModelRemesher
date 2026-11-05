#include "stages/stage5_param.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace qremesh {

namespace {
// Diagnostic counters for one extraction pass (single-threaded call order)
struct ExtractStats {
    size_t used_loop = 0;
    size_t used_fallback = 0;
    size_t skipped_tiny_loop = 0;   // N < 4 after collecting loop samples
    size_t sum_border_verts = 0;
    size_t sum_interior_verts = 0;
    size_t sum_quads = 0;
    size_t sum_nu = 0;
    size_t sum_nv = 0;
    void reset() { *this = ExtractStats{}; }
};
ExtractStats g_extract_stats;
} // namespace

void Stage5ParamAndExtract::balance_and_quantize_patch_sizes(
    RemeshContext& ctx,
    PatchGraph& graph
) {
    if (graph.edge_paths.empty() || graph.patches.empty()) return;

    int64_t target = ctx.params->target_quad_count;
    if (target <= 0) {
        // Use target edge length
        float h = (ctx.params->target_edge_length > 0.0f) ? ctx.params->target_edge_length : ctx.base_target_length;
        for (auto& path : graph.edge_paths) {
            int segs = static_cast<int>(std::round(path.continuous_len / h));
            if (segs % 2 != 0) segs++;
            path.integer_divisions = std::max(2, segs);
        }
        return;
    }

    // Account for symmetry doubling if symmetry axis is active
    double effective_target = static_cast<double>(target);
    if (ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
        effective_target *= 0.5;
    }

    // Function to calculate total quads given global step size h
    auto evaluate_total_quads = [&](float h) -> std::pair<int64_t, std::vector<int>> {
        std::vector<int> path_divs(graph.edge_paths.size());
        for (size_t i = 0; i < graph.edge_paths.size(); ++i) {
            int segs = static_cast<int>(std::round(graph.edge_paths[i].continuous_len / h));
            if (segs % 2 != 0) segs++;
            path_divs[i] = std::max(2, segs);
        }

        int64_t total_quads = 0;
        for (const auto& patch : graph.patches) {
            int64_t perim_samples = 0;
            if (!patch.boundary_loop.empty()) {
                for (const auto& eref : patch.boundary_loop) {
                    perim_samples += path_divs[eref.edge_path_id];
                }
            } else {
                // Approximate from patch area
                float side = std::sqrt(std::max(1e-6f, patch.member_faces.empty() ? 0.0f : 1.0f));
                perim_samples = 4 * std::max(2, 2 * static_cast<int>(std::round(side / (2.0f * h))));
            }

            int K = static_cast<int>(perim_samples / 2);
            int nu = std::max(1, K / 2);
            int nv = std::max(1, K - nu);
            total_quads += static_cast<int64_t>(nu) * nv;
        }

        return { total_quads, path_divs };
    };

    // Binary search for optimal h
    float h_low = 0.001f * ctx.bbox_diag;
    float h_high = 0.5f * ctx.bbox_diag;

    std::vector<int> best_divs;
    int64_t best_diff = std::numeric_limits<int64_t>::max();

    for (int iter = 0; iter < 25; ++iter) {
        float h_mid = 0.5f * (h_low + h_high);
        auto [q_count, divs] = evaluate_total_quads(h_mid);

        int64_t diff = std::abs(q_count - static_cast<int64_t>(effective_target));
        if (diff < best_diff) {
            best_diff = diff;
            best_divs = divs;
        }

        if (q_count > effective_target) {
            h_low = h_mid; // need larger h to reduce quad count
        } else {
            h_high = h_mid; // need smaller h to increase quad count
        }
    }

    if (!best_divs.empty()) {
        for (size_t i = 0; i < graph.edge_paths.size(); ++i) {
            graph.edge_paths[i].integer_divisions = best_divs[i];
        }
    }

    // Exact quad count adjustment if requested
    if (ctx.params->exact_quad_count) {
        // Fine tune by adjusting divisions by +/- 2 on largest paths
        auto get_current_quads = [&]() -> int64_t {
            int64_t total = 0;
            for (const auto& patch : graph.patches) {
                int64_t perim = 0;
                for (const auto& eref : patch.boundary_loop) {
                    perim += graph.edge_paths[eref.edge_path_id].integer_divisions;
                }
                int K = std::max(2, static_cast<int>(perim / 2));
                int nu = std::max(1, K / 2);
                int nv = std::max(1, K - nu);
                total += static_cast<int64_t>(nu) * nv;
            }
            return total;
        };

        int guard = 0;
        while (guard++ < 200) {
            int64_t curr = get_current_quads();
            int64_t diff = static_cast<int64_t>(effective_target) - curr;
            if (std::abs(diff) <= 2) break;

            if (diff > 0) {
                // Add 2 to the longest path
                size_t best_p = 0;
                float max_l = 0.0f;
                for (size_t i = 0; i < graph.edge_paths.size(); ++i) {
                    if (graph.edge_paths[i].continuous_len > max_l) {
                        max_l = graph.edge_paths[i].continuous_len;
                        best_p = i;
                    }
                }
                graph.edge_paths[best_p].integer_divisions += 2;
            } else {
                // Subtract 2 from longest path with divs > 2
                size_t best_p = 0;
                float max_l = 0.0f;
                for (size_t i = 0; i < graph.edge_paths.size(); ++i) {
                    if (graph.edge_paths[i].integer_divisions > 2 && graph.edge_paths[i].continuous_len > max_l) {
                        max_l = graph.edge_paths[i].continuous_len;
                        best_p = i;
                    }
                }
                if (max_l == 0.0f) break;
                graph.edge_paths[best_p].integer_divisions -= 2;
            }
        }
    }
}

void Stage5ParamAndExtract::extract_patch_quads(
    const RemeshContext& ctx,
    const QuasiQuadPatch& patch,
    PatchGraph& graph,
    QuadMeshResult& out_quads
) {
    const auto& mesh = ctx.working_mesh;
    if (patch.member_faces.empty()) return;

    // 1. Collect full ordered boundary vertex sequence from canonical paths
    std::vector<uint32_t> bnd_out_verts;
    std::vector<Vec3f> bnd_positions;
    bool used_fallback_branch = false;

    if (!patch.boundary_loop.empty()) {
        for (const auto& eref : patch.boundary_loop) {
            const auto& ep = graph.edge_paths[eref.edge_path_id];
            const auto& samples = ep.out_sample_vertices;
            size_t n_samp = samples.size();
            if (n_samp < 2) continue;

            if (eref.forward) {
                for (size_t i = 0; i + 1 < n_samp; ++i) {
                    bnd_out_verts.push_back(samples[i]);
                    bnd_positions.push_back(out_quads.positions[samples[i]]);
                }
            } else {
                for (size_t i = n_samp - 1; i > 0; --i) {
                    bnd_out_verts.push_back(samples[i]);
                    bnd_positions.push_back(out_quads.positions[samples[i]]);
                }
            }
        }
    }

    // Fallback if boundary loop could not be traced as closed cycle
    if (bnd_out_verts.size() < 4) {
        // Collect boundary halfedges directly from patch member faces
        std::unordered_set<uint32_t> patch_face_set(patch.member_faces.begin(), patch.member_faces.end());
        std::map<uint32_t, uint32_t> bnd_next;
        for (uint32_t f : patch.member_faces) {
            for (uint32_t h : mesh.get_face_halfedges(f)) {
                uint32_t tw = mesh.halfedges[h].twin;
                if (tw == INVALID_INDEX || !patch_face_set.count(mesh.halfedges[tw].face)) {
                    bnd_next[mesh.from_vertex(h)] = mesh.halfedges[h].to_vertex;
                }
            }
        }

        if (!bnd_next.empty()) {
            used_fallback_branch = true;
            bnd_out_verts.clear();
            bnd_positions.clear();
            uint32_t curr = bnd_next.begin()->first;
            uint32_t start = curr;
            std::unordered_set<uint32_t> vis;
            while (vis.size() < bnd_next.size()) {
                vis.insert(curr);
                uint32_t v_idx = static_cast<uint32_t>(out_quads.positions.size());
                out_quads.positions.push_back(mesh.vertices[curr].pos);
                bnd_out_verts.push_back(v_idx);
                bnd_positions.push_back(mesh.vertices[curr].pos);

                auto it = bnd_next.find(curr);
                if (it == bnd_next.end() || it->second == start || vis.count(it->second)) break;
                curr = it->second;
            }
        }
    }

    size_t N = bnd_out_verts.size();
    if (N < 4) {
        g_extract_stats.skipped_tiny_loop++;
        return;
    }
    if (used_fallback_branch) g_extract_stats.used_fallback++;
    else g_extract_stats.used_loop++;
    g_extract_stats.sum_border_verts += N;

    // Ensure total perimeter is even
    if (N % 2 != 0) {
        // Duplicate one position to make even
        uint32_t last = bnd_out_verts.back();
        bnd_out_verts.push_back(last);
        bnd_positions.push_back(bnd_positions.back());
        N++;
    }

    // Build a patch-local BVH over patch.member_faces for interior point projection
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

    // Divide perimeter N into 4 sides: side0, side1, side2, side3
    // Half perimeter K = N / 2. Set nu = K / 2, nv = K - nu.
    int K = static_cast<int>(N / 2);
    int nu = std::max(1, K / 2);
    int nv = std::max(1, K - nu);
    g_extract_stats.sum_nu += static_cast<size_t>(nu);
    g_extract_stats.sum_nv += static_cast<size_t>(nv);

    // 4 corners on boundary loop
    size_t i0 = 0;
    size_t i1 = nu;
    size_t i2 = nu + nv;
    size_t i3 = 2 * nu + nv;

    // Helper for boundary point retrieval along the 4 edges
    auto get_bottom = [&](int i) -> uint32_t { return bnd_out_verts[(i0 + i) % N]; };
    auto get_right  = [&](int j) -> uint32_t { return bnd_out_verts[(i1 + j) % N]; };
    auto get_top    = [&](int i) -> uint32_t { return bnd_out_verts[(i2 + (nu - i)) % N]; };
    auto get_left   = [&](int j) -> uint32_t { return bnd_out_verts[(i3 + (nv - j)) % N]; };

    std::vector<std::vector<uint32_t>> grid(nu + 1, std::vector<uint32_t>(nv + 1, INVALID_INDEX));

    // Place boundary vertices onto grid edges
    for (int i = 0; i <= nu; ++i) {
        grid[i][0] = get_bottom(i);
        grid[i][nv] = get_top(i);
    }
    for (int j = 0; j <= nv; ++j) {
        grid[0][j] = get_left(j);
        grid[nu][j] = get_right(j);
    }

    Vec3f p00 = out_quads.positions[grid[0][0]];
    Vec3f p10 = out_quads.positions[grid[nu][0]];
    Vec3f p11 = out_quads.positions[grid[nu][nv]];
    Vec3f p01 = out_quads.positions[grid[0][nv]];

    // Internal grid nodes via Coons Transfinite Interpolation
    for (int i = 1; i < nu; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(nu);
        const Vec3f& c0 = out_quads.positions[grid[i][0]];  // bottom
        const Vec3f& c2 = out_quads.positions[grid[i][nv]]; // top

        for (int j = 1; j < nv; ++j) {
            float v = static_cast<float>(j) / static_cast<float>(nv);
            const Vec3f& c3 = out_quads.positions[grid[0][j]];  // left
            const Vec3f& c1 = out_quads.positions[grid[nu][j]]; // right

            Vec3f corner_bilinear = (1.0f - u) * (1.0f - v) * p00 +
                                     u * (1.0f - v) * p10 +
                                     u * v * p11 +
                                     (1.0f - u) * v * p01;

            Vec3f coons_pt = (1.0f - v) * c0 + v * c2 + (1.0f - u) * c3 + u * c1 - corner_bilinear;
            Vec3f projected_p = coons_pt;

            Vec3f patch_pt, patch_norm;
            float d2_patch;
            if (has_patch_bvh && patch_bvh.closest_point(coons_pt, patch_pt, patch_norm, d2_patch)) {
                projected_p = patch_pt;
                Vec3f orig_pt, orig_norm;
                float d2_orig;
                if (ctx.original_bvh.closest_point(patch_pt, orig_pt, orig_norm, d2_orig)) {
                    if (d2_orig <= 0.5f * ctx.base_target_length * ctx.base_target_length &&
                        orig_norm.dot(patch_norm) > 0.25f) {
                        projected_p = orig_pt;
                    }
                }
            } else if (ctx.original_bvh.closest_point(coons_pt, patch_pt, patch_norm, d2_patch)) {
                projected_p = patch_pt;
            }

            uint32_t new_v = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(projected_p);
            grid[i][j] = new_v;
        }
    }

    // Assemble quads with winding aligned with patch.avg_normal
    g_extract_stats.sum_interior_verts += static_cast<size_t>(std::max(0, nu - 1)) *
                                           static_cast<size_t>(std::max(0, nv - 1));
    g_extract_stats.sum_quads += static_cast<size_t>(nu) * static_cast<size_t>(nv);
    for (int i = 0; i < nu; ++i) {
        for (int j = 0; j < nv; ++j) {
            uint32_t v0 = grid[i][j];
            uint32_t v1 = grid[i + 1][j];
            uint32_t v2 = grid[i + 1][j + 1];
            uint32_t v3 = grid[i][j + 1];

            const Vec3f& p0 = out_quads.positions[v0];
            const Vec3f& p1 = out_quads.positions[v1];
            const Vec3f& p2 = out_quads.positions[v2];
            const Vec3f& p3 = out_quads.positions[v3];

            Vec3f q_normal = (p2 - p0).cross(p3 - p1);
            if (q_normal.dot(patch.avg_normal) < 0.0f) {
                // Reverse winding to match patch orientation
                out_quads.polygons.push_back({v0, v3, v2, v1});
            } else {
                out_quads.polygons.push_back({v0, v1, v2, v3});
            }
            out_quads.material_ids.push_back(patch.material_id);
        }
    }
}

void Stage5ParamAndExtract::weld_and_inherit_attributes(
    RemeshContext& ctx,
    QuadMeshResult& out_quads
) {
    if (out_quads.positions.empty() || out_quads.polygons.empty()) return;

    // Conservative weld tolerance: strictly for true identical vertices
    float weld_tol = 1e-5f * ctx.bbox_diag;
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

    // Remap polygon indices
    std::vector<std::vector<uint32_t>> valid_polys;
    std::vector<uint16_t> valid_mats;
    valid_polys.reserve(out_quads.polygons.size());
    valid_mats.reserve(out_quads.material_ids.size());

    for (size_t p_idx = 0; p_idx < out_quads.polygons.size(); ++p_idx) {
        auto& poly = out_quads.polygons[p_idx];
        for (auto& v : poly) {
            v = remap[v];
        }

        // Deduplicate consecutive
        std::vector<uint32_t> cleaned;
        cleaned.reserve(poly.size());
        for (size_t i = 0; i < poly.size(); ++i) {
            uint32_t curr_v = poly[i];
            uint32_t next_v = poly[(i + 1) % poly.size()];
            if (curr_v != next_v) {
                cleaned.push_back(curr_v);
            }
        }

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

    // Detect and inherit sharp / seam edges via Spatial Hash
    out_quads.sharp_edges.clear();
    out_quads.crease_values.clear();
    out_quads.seam_edges.clear();

    const auto& orig_mesh = ctx.working_mesh;
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

    const auto& mesh = ctx.working_mesh;

    // 1. Create global output vertices for all junctions (once, shared)
    std::unordered_map<uint32_t, uint32_t> junc_to_out;
    for (const auto& ep : graph.edge_paths) {
        if (!junc_to_out.count(ep.node_start)) {
            uint32_t out_idx = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(mesh.vertices[ep.node_start].pos);
            junc_to_out[ep.node_start] = out_idx;
        }
        if (!junc_to_out.count(ep.node_end)) {
            uint32_t out_idx = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(mesh.vertices[ep.node_end].pos);
            junc_to_out[ep.node_end] = out_idx;
        }
    }

    // 2. Canonical arc-length sampling for every path in graph.edge_paths
    for (auto& ep : graph.edge_paths) {
        int M = ep.integer_divisions;
        ep.out_sample_vertices.resize(M + 1);
        ep.out_sample_vertices[0] = junc_to_out[ep.node_start];
        ep.out_sample_vertices[M] = junc_to_out[ep.node_end];

        // Cumulative arc length along ep.path_vertices
        size_t n_pv = ep.path_vertices.size();
        std::vector<float> cum_len(n_pv, 0.0f);
        for (size_t k = 0; k + 1 < n_pv; ++k) {
            float d = (mesh.vertices[ep.path_vertices[k + 1]].pos - mesh.vertices[ep.path_vertices[k]].pos).norm();
            cum_len[k + 1] = cum_len[k] + d;
        }
        float tot_l = std::max(cum_len.back(), 1e-6f);

        for (int s = 1; s < M; ++s) {
            float target_d = (static_cast<float>(s) / static_cast<float>(M)) * tot_l;
            auto it = std::lower_bound(cum_len.begin(), cum_len.end(), target_d);
            size_t idx = static_cast<size_t>(std::distance(cum_len.begin(), it));
            if (idx == 0) idx = 1;
            if (idx >= n_pv) idx = n_pv - 1;

            float seg_l = cum_len[idx] - cum_len[idx - 1];
            float frac = (seg_l > 1e-7f) ? (target_d - cum_len[idx - 1]) / seg_l : 0.0f;
            frac = clampf(frac, 0.0f, 1.0f);

            Vec3f pos = (1.0f - frac) * mesh.vertices[ep.path_vertices[idx - 1]].pos +
                        frac * mesh.vertices[ep.path_vertices[idx]].pos;

            uint32_t out_idx = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(pos);
            ep.out_sample_vertices[s] = out_idx;
        }
    }

    // 3. Extract quads for each patch
    g_extract_stats.reset();
    {
        size_t total_samples = 0, big_m = 0, max_m = 0;
        for (const auto& ep : graph.edge_paths) {
            total_samples += static_cast<size_t>(ep.integer_divisions) + 1;
            if (ep.integer_divisions > 50) big_m++;
            max_m = std::max(max_m, static_cast<size_t>(ep.integer_divisions));
        }
        std::vector<bool> used_path(graph.edge_paths.size(), false);
        size_t used_paths = 0, samples_of_used = 0, sum_div_used = 0;
        for (const auto& patch : graph.patches) {
            for (const auto& eref : patch.boundary_loop) {
                if (eref.edge_path_id < used_path.size()) used_path[eref.edge_path_id] = true;
            }
        }
        for (size_t i = 0; i < used_path.size(); ++i) {
            if (used_path[i]) {
                used_paths++;
                samples_of_used += static_cast<size_t>(graph.edge_paths[i].integer_divisions) + 1;
                sum_div_used += static_cast<size_t>(graph.edge_paths[i].integer_divisions);
            }
        }
        std::cout << "[stage5/paths] all_paths=" << graph.edge_paths.size()
                  << " samples_all=" << total_samples
                  << " max_div=" << max_m
                  << " paths_div_gt50=" << big_m
                  << " | used_by_loops=" << used_paths
                  << " samples_used=" << samples_of_used
                  << " sum_div_used=" << sum_div_used
                  << std::endl;
    }
    size_t empty_loops = 0, short_loops = 0, loop_paths_total = 0, quads_from_loops = 0;
    for (const auto& patch : graph.patches) {
        if (patch.boundary_loop.empty()) empty_loops++;
        else if (patch.boundary_loop.size() < 4) short_loops++;
        loop_paths_total += patch.boundary_loop.size();
    }

    for (const auto& patch : graph.patches) {
        size_t before = out_quads.polygons.size();
        extract_patch_quads(ctx, patch, graph, out_quads);
        quads_from_loops += out_quads.polygons.size() - before;
    }

    size_t polys_before_weld = out_quads.polygons.size();
    size_t verts_before_weld = out_quads.positions.size();
    std::cout << "[stage5/extract] used_loop=" << g_extract_stats.used_loop
              << " used_fallback=" << g_extract_stats.used_fallback
              << " skipped_tiny=" << g_extract_stats.skipped_tiny_loop
              << " sum_border=" << g_extract_stats.sum_border_verts
              << " sum_interior=" << g_extract_stats.sum_interior_verts
              << " sum_quads=" << g_extract_stats.sum_quads
              << " avg_nu=" << (graph.patches.empty() ? 0.0 : static_cast<double>(g_extract_stats.sum_nu) / graph.patches.size())
              << " avg_nv=" << (graph.patches.empty() ? 0.0 : static_cast<double>(g_extract_stats.sum_nv) / graph.patches.size())
              << std::endl;

    if (out_quads.polygons.empty()) {
        return QRM_ERR_PARAM_FAILED;
    }

    weld_and_inherit_attributes(ctx, out_quads);

    uint32_t referenced = 0;
    {
        std::vector<bool> seen(out_quads.positions.size(), false);
        for (const auto& poly : out_quads.polygons) {
            for (uint32_t v : poly) if (v < seen.size()) seen[v] = true;
        }
        for (bool b : seen) if (b) referenced++;
    }
    std::cout << "[stage5] patches=" << graph.patches.size()
              << " edge_paths=" << graph.edge_paths.size()
              << " empty_loops=" << empty_loops
              << " loops_lt4=" << short_loops
              << " avg_loop_paths="
              << (graph.patches.empty() ? 0.0 : static_cast<double>(loop_paths_total) / graph.patches.size())
              << "\n[stage5] grid_verts=" << verts_before_weld
              << " verts_after_weld=" << out_quads.positions.size()
              << " referenced=" << referenced
              << " orphans=" << (out_quads.positions.size() - referenced)
              << " polys_before_weld=" << polys_before_weld
              << " polys_after_weld=" << out_quads.polygons.size()
              << " loop_quads=" << quads_from_loops
              << std::endl;

    ctx.dump_quads_obj("stage5_extracted", out_quads);
    return QRM_OK;
}

} // namespace qremesh
