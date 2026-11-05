#include "stages/stage6_opt.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/combinable.h>

namespace qremesh {

float Stage6Optimize::compute_scaled_jacobian(
    const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3
) {
    const Vec3f pts[4] = { p0, p1, p2, p3 };
    float min_j = 1.0f;

    // Quad normal
    Vec3f n = ((p2 - p0).cross(p3 - p1)).normalized();
    if (n.squaredNorm() < 1e-8f) return -1.0f;

    for (int i = 0; i < 4; ++i) {
        Vec3f e1 = pts[(i + 1) % 4] - pts[i];
        Vec3f e2 = pts[(i + 3) % 4] - pts[i];
        float l1 = e1.norm();
        float l2 = e2.norm();
        if (l1 < 1e-8f || l2 < 1e-8f) return -1.0f;

        Vec3f cross_vec = (e1 / l1).cross(e2 / l2);
        float j_corner = cross_vec.dot(n);
        min_j = std::min(min_j, j_corner);
    }

    return min_j;
}

float Stage6Optimize::compute_warp_angle_deg(
    const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3
) {
    Vec3f n1 = triangle_normal(p0, p1, p2);
    Vec3f n2 = triangle_normal(p0, p2, p3);
    float dot_val = clampf(n1.dot(n2), -1.0f, 1.0f);
    return std::acos(dot_val) * (180.0f / PI_F);
}

void Stage6Optimize::cleanup_topology(QuadMeshResult& qmesh) {
    if (qmesh.positions.empty() || qmesh.polygons.empty()) return;

    uint32_t nv = static_cast<uint32_t>(qmesh.positions.size());

    // Compute edge -> polygon incidences
    std::unordered_map<uint64_t, std::vector<uint32_t>> edge_to_polys;
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        const auto& poly = qmesh.polygons[p];
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i];
            uint32_t v = poly[(i + 1) % n];
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            edge_to_polys[key].push_back(static_cast<uint32_t>(p));
        }
    }

    // Valence count
    std::vector<int> valence(nv, 0);
    for (const auto& poly : qmesh.polygons) {
        for (uint32_t v : poly) {
            if (v < nv) valence[v]++;
        }
    }

    // Doublet elimination: internal vertex of valence 2 shared by 2 quads.
    // If quad 1 is (v, a, b, c) and quad 2 is (v, c, d, a), replace both with quad (a, b, c, d)
    std::vector<bool> poly_deleted(qmesh.polygons.size(), false);
    for (uint32_t v = 0; v < nv; ++v) {
        if (valence[v] == 2) {
            std::vector<uint32_t> inc;
            for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
                if (poly_deleted[p]) continue;
                for (uint32_t pv : qmesh.polygons[p]) {
                    if (pv == v) { inc.push_back(static_cast<uint32_t>(p)); break; }
                }
            }
            if (inc.size() == 2 && qmesh.polygons[inc[0]].size() == 4 && qmesh.polygons[inc[1]].size() == 4) {
                uint32_t p0 = inc[0];
                uint32_t p1 = inc[1];
                const auto& poly0 = qmesh.polygons[p0];
                const auto& poly1 = qmesh.polygons[p1];

                // Find index of v in poly0
                int idx0 = -1;
                for (int i = 0; i < 4; ++i) {
                    if (poly0[i] == v) { idx0 = i; break; }
                }
                uint32_t a0 = poly0[(idx0 + 1) % 4];
                uint32_t b0 = poly0[(idx0 + 2) % 4];
                uint32_t c0 = poly0[(idx0 + 3) % 4];

                // Find index of v in poly1
                int idx1 = -1;
                for (int i = 0; i < 4; ++i) {
                    if (poly1[i] == v) { idx1 = i; break; }
                }

                // In a true doublet, poly1 shares vertices a0 and c0
                uint32_t a1 = poly1[(idx1 + 1) % 4];
                uint32_t d1 = poly1[(idx1 + 2) % 4];
                uint32_t c1 = poly1[(idx1 + 3) % 4];

                if ((a1 == c0 && c1 == a0) || (a1 == a0 && c1 == c0)) {
                    // Merge into single quad (a0, b0, c0, d1)
                    qmesh.polygons[p0] = { a0, b0, c0, d1 };
                    poly_deleted[p1] = true;
                }
            }
        }
    }

    // Filter deleted polygons
    std::vector<std::vector<uint32_t>> clean_polys;
    std::vector<uint16_t> clean_mats;
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        if (!poly_deleted[p]) {
            clean_polys.push_back(std::move(qmesh.polygons[p]));
            clean_mats.push_back(p < qmesh.material_ids.size() ? qmesh.material_ids[p] : 0);
        }
    }
    qmesh.polygons = std::move(clean_polys);
    qmesh.material_ids = std::move(clean_mats);
}

void Stage6Optimize::tangential_taubin_smoothing(
    RemeshContext& ctx,
    QuadMeshResult& qmesh,
    int iters
) {
    if (iters <= 0 || qmesh.positions.empty()) return;

    uint32_t nv = static_cast<uint32_t>(qmesh.positions.size());

    // Build vertex adjacency graph
    std::vector<std::vector<uint32_t>> adj(nv);
    for (const auto& poly : qmesh.polygons) {
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i];
            uint32_t v = poly[(i + 1) % n];
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    // Deduplicate neighbors
    for (auto& nbrs : adj) {
        std::sort(nbrs.begin(), nbrs.end());
        nbrs.erase(std::unique(nbrs.begin(), nbrs.end()), nbrs.end());
    }

    // Sharp and open-boundary vertex classification
    std::unordered_map<uint64_t, int> edge_inc_count;
    for (const auto& poly : qmesh.polygons) {
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i];
            uint32_t v = poly[(i + 1) % n];
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            edge_inc_count[key]++;
        }
    }

    std::unordered_set<uint64_t> feature_edge_set;
    for (const auto& edge : qmesh.sharp_edges) {
        if (edge.first < nv && edge.second < nv && edge.first != edge.second) {
            uint32_t u = std::min(edge.first, edge.second);
            uint32_t v = std::max(edge.first, edge.second);
            feature_edge_set.insert((static_cast<uint64_t>(u) << 32) | v);
        }
    }
    if (ctx.params->preserve_open_borders) {
        for (const auto& [key, cnt] : edge_inc_count) {
            if (cnt == 1) {
                feature_edge_set.insert(key);
            }
        }
    }

    std::vector<int> sharp_deg(nv, 0);
    std::vector<std::vector<uint32_t>> sharp_nbrs(nv);
    for (uint64_t key : feature_edge_set) {
        uint32_t u = static_cast<uint32_t>(key >> 32);
        uint32_t v = static_cast<uint32_t>(key & 0xFFFFFFFF);
        if (u < nv && v < nv) {
            sharp_deg[u]++;
            sharp_deg[v]++;
            sharp_nbrs[u].push_back(v);
            sharp_nbrs[v].push_back(u);
        }
    }

    constexpr float lambda = 0.33f;
    constexpr float mu = -0.34f;
    float max_dev = ctx.params->max_surface_deviation * 0.01f * ctx.bbox_diag;
    float shape_w = clampf(ctx.params->shape_preservation, 0.0f, 1.0f);

    for (int iter = 0; iter < iters; ++iter) {
        for (float step_factor : { lambda, mu }) {
            std::vector<Vec3f> new_positions = qmesh.positions;

            tbb::parallel_for(tbb::blocked_range<uint32_t>(0, nv), [&](const tbb::blocked_range<uint32_t>& r) {
                for (uint32_t v = r.begin(); v != r.end(); ++v) {
                    // If corner (>= 3 sharp edges), keep fixed
                    if (sharp_deg[v] >= 3) continue;

                    if (sharp_deg[v] == 2) {
                        // Feature curve: slide only along 1D curve
                        uint32_t na = sharp_nbrs[v][0];
                        uint32_t nb = sharp_nbrs[v][1];
                        Vec3f curve_mid = (qmesh.positions[na] + qmesh.positions[nb]) * 0.5f;
                        Vec3f shift = curve_mid - qmesh.positions[v];
                        Vec3f curve_dir = (qmesh.positions[nb] - qmesh.positions[na]).normalized();
                        Vec3f tangent_shift = shift.dot(curve_dir) * curve_dir;
                        new_positions[v] += step_factor * tangent_shift;
                    } else if (sharp_deg[v] == 0 && !adj[v].empty()) {
                        // Surface vertex: tangential Taubin
                        Vec3f centroid = Vec3f::Zero();
                        for (uint32_t nbr : adj[v]) centroid += qmesh.positions[nbr];
                        centroid /= static_cast<float>(adj[v].size());

                        Vec3f shift = centroid - qmesh.positions[v];

                        // Project onto tangent plane
                        Vec3f normal, closest_orig;
                        float d2;
                        if (ctx.original_bvh.closest_point(qmesh.positions[v], closest_orig, normal, d2)) {
                            Vec3f tan_shift = shift - shift.dot(normal) * normal;
                            new_positions[v] += step_factor * tan_shift;
                        }
                    }
                }
            });

            // Project to BVH with shape_preservation weight and deviation clamping in parallel
            tbb::parallel_for(tbb::blocked_range<uint32_t>(0, nv), [&](const tbb::blocked_range<uint32_t>& r) {
                for (uint32_t v = r.begin(); v != r.end(); ++v) {
                    if (sharp_deg[v] >= 3) continue;
                    Vec3f cp, norm;
                    float dist_sq;
                    if (ctx.original_bvh.closest_point(new_positions[v], cp, norm, dist_sq)) {
                        Vec3f blended = shape_w * cp + (1.0f - shape_w) * new_positions[v];
                        float dist = (blended - cp).norm();
                        if (dist > max_dev && dist > 1e-6f) {
                            new_positions[v] = cp + (blended - cp) * (max_dev / dist);
                        } else {
                            new_positions[v] = blended;
                        }
                    }
                }
            });

            qmesh.positions = std::move(new_positions);
        }
    }
}

void Stage6Optimize::untangle_low_jacobians(
    RemeshContext& ctx,
    QuadMeshResult& qmesh
) {
    constexpr int MAX_PASSES = 5;

    for (int pass = 0; pass < MAX_PASSES; ++pass) {
        bool fixed_any = false;

        for (auto& poly : qmesh.polygons) {
            if (poly.size() != 4) continue;

            float j = compute_scaled_jacobian(
                qmesh.positions[poly[0]],
                qmesh.positions[poly[1]],
                qmesh.positions[poly[2]],
                qmesh.positions[poly[3]]
            );

            if (j < 0.20f) {
                // Untangle by moving corners slightly towards diamond/parallelogram configuration
                Vec3f c = (qmesh.positions[poly[0]] + qmesh.positions[poly[1]] +
                           qmesh.positions[poly[2]] + qmesh.positions[poly[3]]) * 0.25f;

                for (int i = 0; i < 4; ++i) {
                    Vec3f opp = qmesh.positions[poly[(i + 2) % 4]];
                    Vec3f ideal = 2.0f * c - opp;
                    qmesh.positions[poly[i]] = 0.8f * qmesh.positions[poly[i]] + 0.2f * ideal;

                    Vec3f cp, n;
                    float d2;
                    if (ctx.original_bvh.closest_point(qmesh.positions[poly[i]], cp, n, d2)) {
                        qmesh.positions[poly[i]] = cp;
                    }
                }
                fixed_any = true;
            }
        }

        if (!fixed_any) break;
    }
}

void Stage6Optimize::mirror_symmetry(
    RemeshContext& /*ctx*/,
    QuadMeshResult& qmesh,
    int axis,
    float tolerance
) {
    if (axis < 0 || axis > 2) return;

    uint32_t orig_nv = static_cast<uint32_t>(qmesh.positions.size());
    std::vector<uint32_t> mirror_map(orig_nv);

    // Identify seam vertices on plane
    for (uint32_t i = 0; i < orig_nv; ++i) {
        if (std::abs(qmesh.positions[i][axis]) <= tolerance) {
            qmesh.positions[i][axis] = 0.0f;
            mirror_map[i] = i; // Shared on seam
        } else {
            uint32_t mirrored_v = static_cast<uint32_t>(qmesh.positions.size());
            Vec3f mp = qmesh.positions[i];
            mp[axis] = -mp[axis];
            qmesh.positions.push_back(mp);
            mirror_map[i] = mirrored_v;
        }
    }

    size_t orig_np = qmesh.polygons.size();
    for (size_t p = 0; p < orig_np; ++p) {
        const auto& poly = qmesh.polygons[p];
        std::vector<uint32_t> mirrored_poly(poly.size());
        // Reverse winding for correct normal
        for (size_t i = 0; i < poly.size(); ++i) {
            mirrored_poly[i] = mirror_map[poly[poly.size() - 1 - i]];
        }
        qmesh.polygons.push_back(mirrored_poly);
        qmesh.material_ids.push_back(qmesh.material_ids[p]);
    }

    // Mirror sharp edges
    size_t orig_sharp = qmesh.sharp_edges.size();
    for (size_t e = 0; e < orig_sharp; ++e) {
        uint32_t ma = mirror_map[qmesh.sharp_edges[e].first];
        uint32_t mb = mirror_map[qmesh.sharp_edges[e].second];
        if (ma != mb) {
            qmesh.sharp_edges.emplace_back(ma, mb);
            qmesh.crease_values.push_back(qmesh.crease_values[e]);
        }
    }

    // Mirror seam edges
    size_t orig_seams = qmesh.seam_edges.size();
    for (size_t s = 0; s < orig_seams; ++s) {
        uint32_t ma = mirror_map[qmesh.seam_edges[s].first];
        uint32_t mb = mirror_map[qmesh.seam_edges[s].second];
        if (ma != mb) {
            qmesh.seam_edges.emplace_back(ma, mb);
        }
    }
}

void Stage6Optimize::compute_final_quality_report(
    RemeshContext& ctx,
    QuadMeshResult& qmesh
) {
    QrmQualityReport& r = qmesh.report;
    r.quad_count = 0;
    r.tri_count = 0;
    r.ngon_count = 0;

    std::vector<float> jacobians;
    jacobians.reserve(qmesh.polygons.size());
    float min_j = 1.0f;
    float max_warp = 0.0f;

    for (const auto& poly : qmesh.polygons) {
        if (poly.size() == 4) {
            r.quad_count++;
            float j = compute_scaled_jacobian(
                qmesh.positions[poly[0]], qmesh.positions[poly[1]],
                qmesh.positions[poly[2]], qmesh.positions[poly[3]]
            );
            jacobians.push_back(j);
            min_j = std::min(min_j, j);

            float warp = compute_warp_angle_deg(
                qmesh.positions[poly[0]], qmesh.positions[poly[1]],
                qmesh.positions[poly[2]], qmesh.positions[poly[3]]
            );
            max_warp = std::max(max_warp, warp);
        } else if (poly.size() == 3) {
            r.tri_count++;
        } else {
            r.ngon_count++;
        }
    }

    r.scaled_jacobian_min = min_j;
    if (!jacobians.empty()) {
        std::sort(jacobians.begin(), jacobians.end());
        r.scaled_jacobian_median = jacobians[jacobians.size() / 2];
    } else {
        r.scaled_jacobian_median = 0.0f;
    }
    r.max_warp_angle_deg = max_warp;

    // Valence and internal singularity count (excluding boundaries & sharp edges)
    uint32_t nv = static_cast<uint32_t>(qmesh.positions.size());
    std::vector<int> valence(nv, 0);
    std::unordered_map<uint64_t, int> edge_counts;
    for (const auto& poly : qmesh.polygons) {
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i];
            uint32_t v = poly[(i + 1) % n];
            if (u < nv) valence[u]++;
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            edge_counts[key]++;
        }
    }

    std::vector<bool> is_boundary_v(nv, false);
    for (const auto& [key, count] : edge_counts) {
        if (count == 1) {
            uint32_t u = static_cast<uint32_t>(key >> 32);
            uint32_t v = static_cast<uint32_t>(key & 0xFFFFFFFF);
            if (u < nv) is_boundary_v[u] = true;
            if (v < nv) is_boundary_v[v] = true;
        }
    }

    std::vector<bool> is_sharp_v(nv, false);
    for (const auto& [u, v] : qmesh.sharp_edges) {
        if (u < nv) is_sharp_v[u] = true;
        if (v < nv) is_sharp_v[v] = true;
    }

    uint32_t singularities = 0;
    for (uint32_t v = 0; v < nv; ++v) {
        if (!is_boundary_v[v] && !is_sharp_v[v] && valence[v] != 4 && valence[v] > 0) {
            singularities++;
        }
    }
    r.singularity_count = singularities;

    // Hausdorff & RMS error relative to original BVH in parallel
    tbb::combinable<float> max_dev_sq_comb(0.0f);
    tbb::combinable<double> sum_dev_sq_comb(0.0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, qmesh.positions.size()), [&](const tbb::blocked_range<size_t>& r_range) {
        float local_max = max_dev_sq_comb.local();
        double local_sum = sum_dev_sq_comb.local();
        for (size_t i = r_range.begin(); i != r_range.end(); ++i) {
            Vec3f cp, n;
            float d2;
            if (ctx.original_bvh.closest_point(qmesh.positions[i], cp, n, d2)) {
                local_max = std::max(local_max, d2);
                local_sum += d2;
            }
        }
        max_dev_sq_comb.local() = local_max;
        sum_dev_sq_comb.local() = local_sum;
    });

    float max_dev_sq = max_dev_sq_comb.combine([](float a, float b) { return std::max(a, b); });
    double sum_dev_sq = sum_dev_sq_comb.combine([](double a, double b) { return a + b; });

    float diag = std::max(ctx.bbox_diag, 1e-4f);
    r.hausdorff_max_rel = (std::sqrt(max_dev_sq) / diag) * 100.0f;
    r.rms_deviation_rel = (std::sqrt(sum_dev_sq / std::max(1.0, (double)nv)) / diag) * 100.0f;

    auto end_time = std::chrono::high_resolution_clock::now();
    r.total_time_sec = std::chrono::duration<float>(end_time - ctx.start_time).count();
}

QrmStatus Stage6Optimize::execute(RemeshContext& ctx, QuadMeshResult& qmesh) {
    if (!ctx.notify_progress(0.85f, "Stage 6: Topological Cleanup & Tangential Relaxation")) {
        return QRM_ERR_CANCELLED;
    }

    cleanup_topology(qmesh);
    tangential_taubin_smoothing(ctx, qmesh, ctx.params->vertex_relax_iters);
    untangle_low_jacobians(ctx, qmesh);

    if (ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
        float tol = ctx.params->symmetry_tolerance * 0.01f * ctx.bbox_diag;
        mirror_symmetry(ctx, qmesh, ctx.active_symmetry_axis, tol);
    }

    compute_final_quality_report(ctx, qmesh);

    ctx.dump_quads_obj("stage6_optimized", qmesh);

    ctx.notify_progress(1.0f, "Completed");
    return QRM_OK;
}

} // namespace qremesh
