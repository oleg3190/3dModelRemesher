#include "stages/stage5_param.h"
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues>
#include <Eigen/IterativeLinearSolvers>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <iostream>

namespace qremesh {

namespace {
// Scaled Jacobian of a quad - same convention as Stage6Optimize::compute_scaled_jacobian, kept
// local so stage5 does not depend on a stage6 class.
float sj5(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3) {
    const Vec3f pts[4] = { p0, p1, p2, p3 };
    Vec3f n = (p2 - p0).cross(p3 - p1);
    if (n.squaredNorm() < 1e-16f) return -1.0f;
    n.normalize();
    float min_j = 1.0f;
    for (int i = 0; i < 4; ++i) {
        Vec3f e1 = pts[(i + 1) % 4] - pts[i];
        Vec3f e2 = pts[(i + 3) % 4] - pts[i];
        const float l1 = e1.norm(), l2 = e2.norm();
        if (l1 < 1e-8f || l2 < 1e-8f) return -1.0f;
        min_j = std::min(min_j, (e1 / l1).cross(e2 / l2).dot(n));
    }
    return min_j;
}

// T15: pleat census with a switchable definition on ONE mesh, so the S45 claim "546..787 right
// after extraction vs 1855 in stage5_extracted.obj" decomposes into (definition gap) x (filter
// effect) instead of comparing two different metrics (S46 lesson: verify the DEFINITION on both
// sides of a comparison). gated = the [stage5/pleatmap] census: Newell-normal angle > 120 deg AND
// same side. sameside = test_out/pleat_count.py: same side only, no angle gate (folds from ~90
// deg up). Pairing matches the census: first two incident faces in insertion order, first free
// vertex = first polygon vertex that is not an edge endpoint; zero-length edges skipped by both.
struct PleatCounts { size_t gated = 0; size_t sameside = 0; };

PleatCounts count_pleats_t15(const std::vector<Vec3f>& POS,
                             const std::vector<std::vector<uint32_t>>& polys) {
    PleatCounts out;
    struct ER { uint32_t f0 = INVALID_INDEX, f1 = INVALID_INDEX;
                uint32_t w0 = INVALID_INDEX, w1 = INVALID_INDEX; };
    std::unordered_map<uint64_t, ER> emap;
    auto key2 = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };
    for (size_t fi = 0; fi < polys.size(); ++fi) {
        const auto& p = polys[fi];
        const size_t pn = p.size();
        if (pn < 3) continue;
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = p[i], b = p[(i + 1) % pn];
            if (a == b) continue;
            uint32_t free_v = INVALID_INDEX;
            for (size_t k = 0; k < pn; ++k) {
                if (p[k] != a && p[k] != b) { free_v = p[k]; break; }
            }
            uint64_t k2 = key2(a, b);
            auto it = emap.find(k2);
            if (it == emap.end()) {
                ER r; r.f0 = static_cast<uint32_t>(fi); r.w0 = free_v;
                emap.emplace(k2, r);
            } else if (it->second.f1 == INVALID_INDEX) {
                it->second.f1 = static_cast<uint32_t>(fi);
                it->second.w1 = free_v;
            }
        }
    }
    auto newell = [&](size_t fi) -> Vec3f {
        const auto& p = polys[fi];
        Vec3d n = Vec3d::Zero();
        const size_t pn = p.size();
        for (size_t i = 0; i < pn; ++i) {
            Vec3d a = POS[p[i]].cast<double>();
            Vec3d b = POS[p[(i + 1) % pn]].cast<double>();
            n += a.cross(b);
        }
        double ln = n.norm();
        return ln > 1e-14 ? (n / ln).cast<float>() : Vec3f(0, 0, 0);
    };
    std::vector<Vec3f> fn(polys.size());
    for (size_t fi = 0; fi < fn.size(); ++fi) fn[fi] = newell(fi);

    for (const auto& [k2, r] : emap) {
        if (r.f0 == INVALID_INDEX || r.f1 == INVALID_INDEX) continue;
        if (r.w0 == INVALID_INDEX || r.w1 == INVALID_INDEX) continue;
        if (r.w0 >= POS.size() || r.w1 >= POS.size()) continue;
        const double d = std::max(-1.0, std::min(1.0, static_cast<double>(fn[r.f0].dot(fn[r.f1]))));
        const float dih = static_cast<float>(std::acos(d) * 180.0 / PI_D);
        const uint32_t a = static_cast<uint32_t>(k2 >> 32);
        const uint32_t b = static_cast<uint32_t>(k2 & 0xFFFFFFFFu);
        if (a >= POS.size() || b >= POS.size()) continue;
        Vec3f e = POS[b] - POS[a];
        const float el = e.norm();
        if (el < 1e-9f) continue;
        e = e / el;
        const Vec3f mid = 0.5f * (POS[a] + POS[b]);
        Vec3f u0 = POS[r.w0] - mid, u1 = POS[r.w1] - mid;
        u0 -= u0.dot(e) * e;
        u1 -= u1.dot(e) * e;
        if (u0.dot(u1) <= 0.0f) continue;      // opposite sides = flat continuation
        out.sameside++;
        if (dih > 120.0f) out.gated++;
    }
    return out;
}
} // namespace


namespace {
// Diagnostic counters for one extraction pass
struct ExtractStats {
    size_t used_loop = 0;
    size_t used_fallback = 0;
    size_t skipped_tiny_loop = 0;
    size_t sum_border_verts = 0;
    size_t sum_interior_verts = 0;
    size_t sum_quads = 0;
    size_t sum_nu = 0;
    size_t sum_nv = 0;
    // T18/T5 metric: how much of the mesh sits on a grid too coarse to orthoganalize.
    // A 2x2 (or 2x3) patch has at most one free interior node, so its quad shape is dictated by
    // the 8 fixed border corners - the S26 ceiling. Large Delta-n cells make this worse, so the
    // fraction of patches with min(nu,nv) < 3 is what T5 has to move, and the S47 median-SJ drop
    // is the symptom it has to explain.
    size_t patches_min_dim_lt3 = 0;
    size_t patches_counted = 0;
    size_t mismatch_patches = 0;   // opposite sides with different sample counts
    size_t degenerate_slots = 0;   // border grid slots mapping to the same vertex
    size_t parity_hacks = 0;       // odd loop sample counts repaired by duplicating one
    void reset() { *this = ExtractStats{}; }
};
ExtractStats g_extract_stats;
size_t g_loop_discont_count = 0;

} // namespace

void Stage5ParamAndExtract::balance_and_quantize_patch_sizes(
    RemeshContext& ctx,
    PatchGraph& graph,
    double effective_target_override
) {
    if (graph.edge_paths.empty() || graph.patches.empty()) return;

    const auto& mesh = ctx.working_mesh;
    size_t num_edges = graph.edge_paths.size();
    size_t num_patches = graph.patches.size();

    // 1. Calculate metric length for each canonical edge path weighted by local sizing field
    std::vector<float> path_metric_len(num_edges, 0.0f);
    for (size_t i = 0; i < num_edges; ++i) {
        const auto& ep = graph.edge_paths[i];
        float m_len = 0.0f;
        for (size_t k = 0; k + 1 < ep.path_vertices.size(); ++k) {
            uint32_t u = ep.path_vertices[k];
            uint32_t v = ep.path_vertices[k + 1];
            float d = (mesh.vertices[v].pos - mesh.vertices[u].pos).norm();
            float local_s = 0.5f * (ctx.sizing_field[u] + ctx.sizing_field[v]);
            float scale = (local_s > 1e-6f) ? (ctx.base_target_length / local_s) : 1.0f;
            scale = clampf(scale, 0.5f, 2.0f);
            m_len += d * scale;
        }
        path_metric_len[i] = std::max(m_len, 1e-5f);
    }

    int64_t target = ctx.params->target_quad_count;
    float h_initial = (ctx.params->target_edge_length > 0.0f) ? ctx.params->target_edge_length : ctx.base_target_length;

    // ---- T8 side-balance guard (QREMESH_SIDE_BAL=ratio, default 0=OFF) ----------------
    // Measured mechanism of the BUG-01 pleats: nu/nv of a patch grid come from the SUM of the
    // loop divisions (i2=N/2, i1~N/4), so a patch whose opposite sides differ a lot (one=3025
    // paths with d=1 against max=15) is filled with nu rows across a side that only holds 2
    // border samples. The coarse end then has to span what the neighbour patch draws as many
    // thin rows -> folded quads. Evidence: neighbour pairs with area ratio <0.25 pleat in 32%
    // of cases versus a 5.1% base rate (test_out/pleat_count.py, control sphere 0.73%).
    // The fix raises the SHORT side of an opposite pair. Raising happens on the CANONICAL PATH,
    // which both neighbouring patches share, so seam consistency (invariant #5) holds by
    // construction; paths with d=0 are never touched (that zero is the seam-matching rule,
    // invariant #4). The budget estimator applies the SAME rule (invariant #10) - the first
    // version only fixed the real pass and the polygon count ran away 9184 -> 16255.
    int side_bal = 0;
    if (const char* e = std::getenv("QREMESH_SIDE_BAL")) side_bal = std::atoi(e);
    std::vector<std::vector<uint32_t>> loop_paths(num_patches);
    if (side_bal > 1) {
        for (size_t p = 0; p < num_patches; ++p) {
            const auto& lp = graph.patches[p].boundary_loop;
            loop_paths[p].reserve(lp.size());
            for (const auto& eref : lp) {
                if (eref.edge_path_id < num_edges) loop_paths[p].push_back(eref.edge_path_id);
            }
        }
    }
    auto apply_side_balance = [&](std::vector<int>& dv) -> size_t {
        if (side_bal <= 1) return 0;
        size_t raised_total = 0;
        for (int round = 0; round < 6; ++round) {
            bool changed = false;
            for (size_t p = 0; p < num_patches; ++p) {
                const auto& paths = loop_paths[p];
                const size_t M = paths.size();
                if (M < 4) continue;
                int64_t N = 0;
                for (uint32_t pi : paths) N += dv[pi];
                if (N < 8) continue;             // nothing worth balancing on a minimal loop
                // side boundaries = path cuts nearest to the loop quarter points (same rule as
                // the corner snapping in evaluate_total_quads/extract_patch_quads)
                auto cut_for = [&](int64_t targ, size_t lo, size_t hi) -> size_t {
                    int64_t acc = 0; size_t bestk = lo; int64_t bestd = INT64_MAX;
                    for (size_t k = 0; k < M; ++k) {
                        if (k >= lo && k <= hi && k >= 1) {
                            int64_t dd = std::abs(acc - targ);
                            if (dd < bestd) { bestd = dd; bestk = k; }
                        }
                        acc += std::max(1, dv[paths[k]]);
                    }
                    return bestk;
                };
                size_t c1 = cut_for(N / 4, 1, M - 3);
                size_t c2 = cut_for(N / 2, c1 + 1, M - 2);
                size_t c3 = cut_for(3 * N / 4, c2 + 1, M - 1);
                const size_t b[5] = { 0, c1, c2, c3, M };
                int64_t s[4] = { 0, 0, 0, 0 };
                for (int q = 0; q < 4; ++q)
                    for (size_t k = b[q]; k < b[q + 1]; ++k) s[q] += dv[paths[k]];
                for (int pair = 0; pair < 2; ++pair) {
                    for (int dir = 0; dir < 2; ++dir) {
                        int lo = pair * 2 + dir, hi = pair * 2 + (1 - dir);
                        if (s[hi] <= 0) continue;
                        int guard = 0;
                        while (s[lo] * side_bal < s[hi] && guard++ < 400) {
                            size_t bestk = b[lo];
                            float bestl = -1.0f;
                            for (size_t k = b[lo]; k < b[lo + 1]; ++k) {
                                if (path_metric_len[paths[k]] > bestl) {
                                    bestl = path_metric_len[paths[k]]; bestk = k;
                                }
                            }
                            if (b[lo] >= b[lo + 1]) break;
                            dv[paths[bestk]] += 1;
                            s[lo] += 1;
                            ++raised_total;
                            changed = true;
                        }
                    }
                }
            }
            if (!changed) break;
        }
        return raised_total;
    };
    if (target <= 0) {
        for (size_t i = 0; i < num_edges; ++i) {
            graph.edge_paths[i].integer_divisions = std::clamp(static_cast<int>(std::round(path_metric_len[i] / h_initial)), 0, 100);
        }
        return;
    }

    // effective_target_override > 0 means the caller already computed the budget (count
    // calibration pass) - then symmetry must not be applied a second time.
    double effective_target = (effective_target_override > 0.0)
        ? effective_target_override
        : static_cast<double>(target) * 0.92;
    if (effective_target_override <= 0.0 &&
        ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
        effective_target *= 0.5;
    }

    // 2. Detect 4 geometric corners for each patch and rotate boundary_loop so Corner 0 is at index 0
    //    When d_opt is given, only splits whose OPPOSITE side division sums are equal are accepted
    //    (that is what keeps the quad grid regular: D0 == D2, D1 == D3).
    //    Returns true when a candidate split was found and applied.
    auto detect_four_corners = [&](QuasiQuadPatch& patch, const std::vector<int>* d_opt = nullptr) -> bool {
        size_t M = patch.boundary_loop.size();
        if (M < 4) {
            patch.corner_indices = {0, 0, 0, 0};
            return false;
        }
        if (M == 4) {
            patch.corner_indices = {0, 1, 2, 3};
            return false;
        }

        std::vector<float> path_lens(M);
        float total_perim = 0.0f;
        for (size_t k = 0; k < M; ++k) {
            path_lens[k] = graph.edge_paths[patch.boundary_loop[k].edge_path_id].continuous_len;
            total_perim += path_lens[k];
        }

        std::vector<float> turn_scores(M, 0.0f);
        for (size_t k = 0; k < M; ++k) {
            size_t prev_k = (k + M - 1) % M;
            const auto& ep_prev = graph.edge_paths[patch.boundary_loop[prev_k].edge_path_id];
            const auto& ep_curr = graph.edge_paths[patch.boundary_loop[k].edge_path_id];

            Vec3f t_in;
            if (patch.boundary_loop[prev_k].forward) {
                size_t n = ep_prev.path_vertices.size();
                size_t step = std::min(n - 1, size_t(4));
                t_in = (mesh.vertices[ep_prev.path_vertices[n - 1]].pos - 
                        mesh.vertices[ep_prev.path_vertices[n - 1 - step]].pos).normalized();
            } else {
                size_t n = ep_prev.path_vertices.size();
                size_t step = std::min(n - 1, size_t(4));
                t_in = (mesh.vertices[ep_prev.path_vertices[0]].pos - 
                        mesh.vertices[ep_prev.path_vertices[step]].pos).normalized();
            }

            Vec3f t_out;
            if (patch.boundary_loop[k].forward) {
                size_t n = ep_curr.path_vertices.size();
                size_t step = std::min(n - 1, size_t(4));
                t_out = (mesh.vertices[ep_curr.path_vertices[step]].pos - 
                         mesh.vertices[ep_curr.path_vertices[0]].pos).normalized();
            } else {
                size_t n = ep_curr.path_vertices.size();
                size_t step = std::min(n - 1, size_t(4));
                t_out = (mesh.vertices[ep_curr.path_vertices[n - 1 - step]].pos - 
                         mesh.vertices[ep_curr.path_vertices[n - 1]].pos).normalized();
            }

            float dot = clampf(t_in.dot(t_out), -1.0f, 1.0f);
            float score = std::abs(dot); // 0.0 at perfect 90-degree right turn
            if (dot < -0.5f) score += 1.5f; // Penalize sharp 180-degree spikes vs 90-degree corners
            uint32_t junc = patch.boundary_loop[k].forward ? ep_curr.node_start : ep_curr.node_end;
            if (mesh.vertices[junc].is_corner) score -= 2.0f;
            turn_scores[k] = score;
        }

        std::vector<size_t> cand_indices(M);
        for (size_t i = 0; i < M; ++i) cand_indices[i] = i;
        std::sort(cand_indices.begin(), cand_indices.end(), [&](size_t a, size_t b) {
            return turn_scores[a] < turn_scores[b];
        });

        std::vector<size_t> top_cands;
        if (d_opt && M <= 30) {
            // Feasibility search needs the full candidate set (loops are short)
            top_cands.resize(M);
            for (size_t i = 0; i < M; ++i) top_cands[i] = i;
        } else {
            // Geometric pass keeps only the most corner-like positions
            size_t num_cands = std::min(M, d_opt ? size_t(24) : size_t(16));
            top_cands.assign(cand_indices.begin(), cand_indices.begin() + num_cands);
            std::sort(top_cands.begin(), top_cands.end());
        }

        float best_total_score = 1e9f;
        std::array<size_t, 4> best_corners = {0, 1, 2, 3};
        bool found_candidate = false;

        size_t C = top_cands.size();
        for (size_t a = 0; a < C; ++a) {
            for (size_t b = a + 1; b < C; ++b) {
                for (size_t c = b + 1; c < C; ++c) {
                    for (size_t d = c + 1; d < C; ++d) {
                        size_t i0 = top_cands[a];
                        size_t i1 = top_cands[b];
                        size_t i2 = top_cands[c];
                        size_t i3 = top_cands[d];

                        if (d_opt) {
                            int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
                            for (size_t k = i0; k < i1; ++k) s0 += (*d_opt)[patch.boundary_loop[k].edge_path_id];
                            for (size_t k = i1; k < i2; ++k) s1 += (*d_opt)[patch.boundary_loop[k].edge_path_id];
                            for (size_t k = i2; k < i3; ++k) s2 += (*d_opt)[patch.boundary_loop[k].edge_path_id];
                            for (size_t k = i3; k < M; ++k)  s3 += (*d_opt)[patch.boundary_loop[k].edge_path_id];
                            for (size_t k = 0; k < i0; ++k)  s3 += (*d_opt)[patch.boundary_loop[k].edge_path_id];
                            if (s0 != s2 || s1 != s3 || s0 < 1 || s1 < 1) continue;
                        }

                        float l0 = 0.0f, l1 = 0.0f, l2 = 0.0f, l3 = 0.0f;
                        for (size_t k = i0; k < i1; ++k) l0 += path_lens[k];
                        for (size_t k = i1; k < i2; ++k) l1 += path_lens[k];
                        for (size_t k = i2; k < i3; ++k) l2 += path_lens[k];
                        for (size_t k = i3; k < M; ++k)  l3 += path_lens[k];
                        for (size_t k = 0; k < i0; ++k)  l3 += path_lens[k];

                        if (l0 <= 1e-5f || l1 <= 1e-5f || l2 <= 1e-5f || l3 <= 1e-5f) continue;

                        float geom_score = turn_scores[i0] + turn_scores[i1] + turn_scores[i2] + turn_scores[i3];
                        float bal_penalty = -0.7f * (std::log(l0 / total_perim) + std::log(l1 / total_perim) +
                                                     std::log(l2 / total_perim) + std::log(l3 / total_perim));
                        float diff02 = (l0 - l2) / std::max(1e-5f, l0 + l2);
                        float diff13 = (l1 - l3) / std::max(1e-5f, l1 + l3);
                        float opp_penalty = 3.5f * (diff02 * diff02 + diff13 * diff13);
                        float score = geom_score + bal_penalty + opp_penalty;
                        if (score < best_total_score) {
                            best_total_score = score;
                            best_corners = {i0, i1, i2, i3};
                            found_candidate = true;
                        }
                    }
                }
            }
        }

        if (!found_candidate) return false;

        size_t c0 = best_corners[0];
        if (c0 > 0) {
            std::rotate(patch.boundary_loop.begin(), patch.boundary_loop.begin() + c0, patch.boundary_loop.end());
            best_corners[1] = (best_corners[1] + M - c0) % M;
            best_corners[2] = (best_corners[2] + M - c0) % M;
            best_corners[3] = (best_corners[3] + M - c0) % M;
            best_corners[0] = 0;
        }
        patch.corner_indices = best_corners;
        return true;
    };

    for (auto& patch : graph.patches) {
        detect_four_corners(patch);
    }

    // ---- S30: are patches rectangular? the grid skew (angle dev 49deg) is inherited from
    // the CUT corners. Measure the turn angle between the two border sides at each of the
    // 4 grid corners. ~90deg -> grid lines can be orthogonal; far from 90 -> every interior
    // Laplacian inherits the slant and no re-cutting can fix it (explains S26/S29 plateau).
    {
        std::vector<float> corner_dev;
        for (const auto& patch : graph.patches) {
            size_t M = patch.boundary_loop.size();
            if (M < 4) continue;
            for (int ci = 0; ci < 4; ++ci) {
                size_t k = patch.corner_indices[ci];
                const auto& ep_prev = graph.edge_paths[patch.boundary_loop[(k + M - 1) % M].edge_path_id];
                const auto& ep_curr = graph.edge_paths[patch.boundary_loop[k].edge_path_id];
                auto endpt = [&](const CanonicalEdgePath& e, bool first_end) {
                    size_t idx = first_end ? (e.path_vertices.empty()?0:e.path_vertices.size()-1) : 0;
                    return mesh.vertices[e.path_vertices[idx]].pos;
                };
                // incoming direction into the corner, outgoing out of it
                Vec3f in_d = endpt(ep_prev, true) - endpt(ep_prev, false);
                Vec3f out_d = endpt(ep_curr, false) - endpt(ep_curr, true);
                // orient: in_d points toward corner along prev path end; keep as is
                float il = in_d.norm(), ol = out_d.norm();
                if (il < 1e-6f || ol < 1e-6f) continue;
                float dot = clampf(in_d.dot(out_d) / (il * ol), -1.0f, 1.0f);
                // angle BETWEEN the two sides as seen from the corner is 180 - turn
                float ang = std::acos(dot) * 180.0f / PI_F;
                corner_dev.push_back(std::abs(ang - 90.0f));
            }
        }
        if (!corner_dev.empty()) {
            std::sort(corner_dev.begin(), corner_dev.end());
            float med = corner_dev[corner_dev.size() / 2];
            size_t gt30 = 0, gt45 = 0;
            for (float a : corner_dev) { if (a > 30) gt30++; if (a > 45) gt45++; }
            std::cout << "[stage5/corners] n=" << corner_dev.size()
                      << " median_dev90=" << med
                      << " >30deg=" << (100.0 * gt30 / corner_dev.size())
                      << "% >45deg=" << (100.0 * gt45 / corner_dev.size()) << "%" << std::endl;
        }
    }

    // 3. Binary search for optimal h matching target quad count
    std::vector<int> d_test(num_edges);
    // Sliver guard floor (QREMESH_MIN_DIV). The BUDGET ESTIMATOR MUST MODEL IT (invariant #10):
    // the first experiment applied the floor only after the search and the polygon count blew up
    // 9184 -> 16255 (MIN_DIV=2) because h was calibrated against un-floored divisions.
    int min_div = 0;
    if (const char* e = std::getenv("QREMESH_MIN_DIV")) min_div = std::atoi(e);
    auto floor_d = [min_div](int d) -> int {
        return (min_div > 1 && d > 0 && d < min_div) ? min_div : d;
    };

    auto evaluate_total_quads = [&](float h) -> int64_t {
        for (size_t e = 0; e < num_edges; ++e) {
            d_test[e] = floor_d(std::clamp(static_cast<int>(std::round(path_metric_len[e] / h)), 0, 100));
        }
        apply_side_balance(d_test);   // the estimator must model the guard (invariant #10)
        int64_t total = 0;
        for (size_t p = 0; p < num_patches; ++p) {
            const auto& patch = graph.patches[p];
            size_t M = patch.boundary_loop.size();
            if (M < 4) {
                total += 1;
                continue;
            }
            // Sample-level corners: N = loop sample count; the u-corner is snapped to the
            // nearest path boundary (same rule as extract_patch_quads) so the estimate
            // matches what extraction actually produces.
            int64_t N = 0;
            for (size_t k = 0; k < M; ++k) {
                N += d_test[patch.boundary_loop[k].edge_path_id];
            }
            if (N < 4) N = 4;   // mirrors the "at least 4 samples" guarantee before parity
            int64_t i2 = N / 2;
            int64_t i1 = N / 4;
            {
                int64_t acc = 0;
                int64_t best = i1;
                int64_t best_diff = i1;
                for (size_t k = 0; k + 1 < M; ++k) {
                    acc += std::max(1, d_test[patch.boundary_loop[k].edge_path_id]);
                    if (acc >= i2) break;
                    if (acc < 1) continue;
                    int64_t diff = (acc > i1) ? acc - i1 : i1 - acc;
                    // Snapping to the nearest path boundary puts the corner on a geometric
                    // junction; both estimator and extraction use the SAME rule so the quad
                    // budget stays consistent (calibration then lands within a few percent).
                    if (diff <= best_diff) { best_diff = diff; best = acc; }
                }
                i1 = best;
            }
            if (i1 < 1) i1 = 1;
            if (i1 > i2 - 1) i1 = i2 - 1;
            int64_t nu = i1;
            int64_t nv = i2 - i1;
            if (nu < 1) nu = 1;
            if (nv < 1) nv = 1;
            total += nu * nv;
        }
        return total;
    };

    float h_low = 0.0001f * ctx.bbox_diag;
    float h_high = 2.0f * ctx.bbox_diag;
    int64_t min_possible_q = evaluate_total_quads(h_high);
    int64_t search_target = std::max<int64_t>(static_cast<int64_t>(effective_target), min_possible_q + 1);
    float best_h = ctx.base_target_length;
    int64_t best_diff = std::numeric_limits<int64_t>::max();
    int64_t best_q_count = 0;

    for (int iter = 0; iter < 40; ++iter) {
        float h_mid = 0.5f * (h_low + h_high);
        int64_t q_count = evaluate_total_quads(h_mid);
        int64_t diff = std::abs(q_count - search_target);

        if (diff < best_diff || (diff == best_diff && h_mid < best_h)) {
            best_diff = diff;
            best_h = h_mid;
            best_q_count = q_count;
        }

        if (q_count >= search_target) {
            h_low = h_mid;
        } else {
            h_high = h_mid;
        }
    }

    std::cout << "[stage5/bsearch] target=" << target << " effective=" << effective_target 
              << " best_q_count=" << best_q_count << " best_h=" << best_h 
              << " best_diff=" << best_diff << std::endl;

    // 4. Build linear constraint system A d = 0 for opposite sides
    // Row 2p:   sum_{Side 0} d(e) - sum_{Side 2} d(e) = 0
    // Row 2p+1: sum_{Side 1} d(e) - sum_{Side 3} d(e) = 0
    std::vector<Eigen::Triplet<double>> triplets;
    size_t num_rows = 2 * num_patches;

    struct ColEntry {
        int row;
        int coeff;
    };
    std::vector<std::vector<ColEntry>> col_entries(num_edges);

    for (size_t p = 0; p < num_patches; ++p) {
        const auto& patch = graph.patches[p];
        size_t M = patch.boundary_loop.size();
        if (M < 4) continue;

        size_t i0 = patch.corner_indices[0];
        size_t i1 = patch.corner_indices[1];
        size_t i2 = patch.corner_indices[2];
        size_t i3 = patch.corner_indices[3];

        int row_u = static_cast<int>(2 * p);
        int row_v = static_cast<int>(2 * p + 1);

        for (size_t k = i0; k < i1; ++k) {
            uint32_t e = patch.boundary_loop[k].edge_path_id;
            triplets.push_back(Eigen::Triplet<double>(row_u, e, 1.0));
            col_entries[e].push_back({row_u, 1});
        }
        for (size_t k = i2; k < i3; ++k) {
            uint32_t e = patch.boundary_loop[k].edge_path_id;
            triplets.push_back(Eigen::Triplet<double>(row_u, e, -1.0));
            col_entries[e].push_back({row_u, -1});
        }

        for (size_t k = i1; k < i2; ++k) {
            uint32_t e = patch.boundary_loop[k].edge_path_id;
            triplets.push_back(Eigen::Triplet<double>(row_v, e, 1.0));
            col_entries[e].push_back({row_v, 1});
        }
        for (size_t k = i3; k < M; ++k) {
            uint32_t e = patch.boundary_loop[k].edge_path_id;
            triplets.push_back(Eigen::Triplet<double>(row_v, e, -1.0));
            col_entries[e].push_back({row_v, -1});
        }
    }

    Eigen::SparseMatrix<double> A(num_rows, num_edges);
    A.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SparseMatrix<double> AT = A.transpose();

    // Initial unconstrained divisions
    Eigen::VectorXd d0(num_edges);
    for (size_t e = 0; e < num_edges; ++e) {
        d0[e] = std::clamp(static_cast<double>(std::round(path_metric_len[e] / best_h)), 0.0, 100.0);
    }

    // Solve (A * A^T + 1e-4 * I) * lambda = A * d0
    Eigen::SparseMatrix<double> AAT = A * AT;
    for (int i = 0; i < AAT.rows(); ++i) {
        AAT.coeffRef(i, i) += 1e-4;
    }
    AAT.makeCompressed();

    Eigen::VectorXd rhs = A * d0;
    Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper> cg;
    cg.compute(AAT);
    Eigen::VectorXd lambda = cg.solve(rhs);
    Eigen::VectorXd d_cont = d0 - AT * lambda;

    // Integer rounding + greedy + signed-graph BFS repair, run MULTI-START.
    // A single deterministic rounding gets stuck in a local optimum (on the test model:
    // residual 266 over 221 unsatisfied opposite-side pairs), which is exactly what makes
    // 167 patches emit degenerate border cells -> extra triangles and skewed quads.
    // Restarts of the same repair pipeline (seeded from params->random_seed, so runs stay
    // bit-deterministic) reach far lower residuals, usually zero. The greedy additionally
    // pays a small penalty for drifting away from the metric solution d0 so that the
    // density stays where the sizing field wants it (aspect ratio does not degrade).
    // T8b: invariant #4 says a canonical path SHORTER than half a cell must carry NO intermediate
    // sample (both patches then see the same zero, so the seam is identical by construction). The
    // distributor rounds into clamp(..., 1, 100), i.e. every path gets at least one sample - the
    // log proves it ([stage5/divs] zero=0 one=3025 on the AI model, zero=0 one=87 even on a clean
    // icosphere). A path much shorter than a cell then contributes a border sample that is almost
    // on top of its neighbour vertex -> spike quad (measured: 332 folded faces on the clean sphere,
    // the worst of them 25x under the median face area). QREMESH_ALLOW_ZERO=1 restores the rule.
    int d_floor = 1;
    if (const char* e = std::getenv("QREMESH_ALLOW_ZERO")) d_floor = (std::atoi(e) != 0) ? 0 : 1;

    std::vector<int> d_int(num_edges);
    std::vector<int> r(num_rows, 0);
    int64_t E_res = 0;

    std::vector<int> d_best;
    std::vector<int> r_best;
    int64_t E_best = -1;
    int64_t drift_best = 0;

    std::mt19937 rng(ctx.params->random_seed ? ctx.params->random_seed : 42u);
    std::normal_distribution<double> noise(0.0, 0.35);
    // Opposite-side equalities are satisfied BY CONSTRUCTION now (sample-level corners
    // with i2 = N/2), so enforcing them again in the integer solve only distorts the metric
    // densities: it raised the achievable quad-count floor (targets below ~1500 were
    // unreachable) and skewed cells. Keep the raw metric rounding unless explicitly
    // re-enabled for debugging.
    constexpr bool kEnforceSideEqualities = false;
    constexpr int NUM_RESTARTS = kEnforceSideEqualities ? 32 : 1;

    for (int restart = 0; restart < NUM_RESTARTS; ++restart) {
    for (size_t e = 0; e < num_edges; ++e) {
        double base = kEnforceSideEqualities ? d_cont[e] : d0[e];
        double v = base + ((restart == 0) ? 0.0 : noise(rng));
        d_int[e] = std::clamp(static_cast<int>(std::round(v)), d_floor, 100);
    }

    r.assign(num_rows, 0);
    for (int k = 0; k < A.outerSize(); ++k) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it) {
            r[it.row()] += static_cast<int>(it.value()) * d_int[it.col()];
        }
    }

    E_res = 0;
    for (int val : r) E_res += static_cast<int64_t>(val) * val;

    for (int iter = 0; kEnforceSideEqualities && iter < 1500 && E_res > 0; ++iter) {
        int best_e = -1;
        int best_delta = 0;
        int64_t best_dE_res = 0;
        double best_dE_total = 0.0;
        constexpr double DRIFT_W = 0.1;

        for (size_t e = 0; e < num_edges; ++e) {
            for (int delta : {-1, 1}) {
                if (d_int[e] + delta < d_floor || d_int[e] + delta > 100) continue;
                int64_t dE_res = 0;
                for (const auto& entry : col_entries[e]) {
                    dE_res += 2 * delta * r[entry.row] * entry.coeff + entry.coeff * entry.coeff;
                }
                // keep density close to the metric solution d0
                double dev = static_cast<double>(d_int[e]) - d0[e];
                double ndev = dev + delta;
                double dE_total = static_cast<double>(dE_res) +
                                  DRIFT_W * (ndev * ndev - dev * dev);
                if (dE_total < best_dE_total) {
                    best_dE_total = dE_total;
                    best_dE_res = dE_res;
                    best_e = static_cast<int>(e);
                    best_delta = delta;
                }
            }
        }

        if (best_dE_total >= 0.0) break;
        if (best_e >= 0) {
            d_int[best_e] += best_delta;
            for (const auto& entry : col_entries[best_e]) {
                r[entry.row] += best_delta * entry.coeff;
            }
            E_res += best_dE_res;
        } else {
            break;
        }
    }

    // Signed-graph BFS augmenting paths to cancel opposite-side residuals across intermediate patches
    std::vector<std::vector<std::pair<uint32_t, int>>> row_to_edges(num_rows);
    for (size_t e = 0; e < num_edges; ++e) {
        for (const auto& entry : col_entries[e]) {
            row_to_edges[entry.row].push_back({static_cast<uint32_t>(e), entry.coeff});
        }
    }

    struct BfsPrev {
        int prev_state = -1;
        uint32_t edge_id = INVALID_INDEX;
        int delta = 0;
        int sum_delta = 0;
    };

    for (int pass = 0; kEnforceSideEqualities && pass < 60 && E_res > 0; ++pass) {
        bool any_augmented = false;
        for (size_t r_start = 0; r_start < num_rows; ++r_start) {
            while (r[r_start] != 0) {
                int start_step = (r[r_start] > 0) ? -1 : 1;
                // State index: 2 * row + (step_needed == +1 ? 1 : 0)
                std::vector<BfsPrev> visited(2 * num_rows);
                std::vector<int> depth(2 * num_rows, -1);
                std::vector<int> queue;
                queue.reserve(256);

                int init_state = static_cast<int>(2 * r_start + (start_step > 0 ? 1 : 0));
                depth[init_state] = 0;
                queue.push_back(init_state);

                uint32_t final_edge = INVALID_INDEX;
                int final_delta = 0;
                int final_prev_state = -1;
                int target_row = -1;
                int target_step = 0;
                bool found_path = false;

                for (size_t qi = 0; qi < queue.size() && !found_path; ++qi) {
                    int cur_state = queue[qi];
                    int cur_row = cur_state / 2;
                    int step_in = (cur_state % 2 == 1) ? 1 : -1;
                    int cur_d = depth[cur_state];
                    int cur_sum_delta = visited[cur_state].sum_delta;
                    if (cur_d >= 4) continue;

                    for (const auto& [eid, coeff_cur] : row_to_edges[cur_row]) {
                        bool edge_on_path = false;
                        for (int s = cur_state; s != init_state && s >= 0; s = visited[s].prev_state) {
                            if (visited[s].edge_id == eid) { edge_on_path = true; break; }
                        }
                        if (edge_on_path) continue;

                        int delta = step_in * coeff_cur;
                        int new_sum_delta = cur_sum_delta + delta;
                        if (std::abs(new_sum_delta) > 2) continue;

                        int max_d = std::max(3, static_cast<int>(std::round(d0[eid])) + 2);
                        if (d_int[eid] + delta < d_floor || d_int[eid] + delta > max_d) continue;

                        if (col_entries[eid].size() == 1) {
                            if (new_sum_delta <= 2) {
                                final_edge = eid;
                                final_delta = delta;
                                final_prev_state = cur_state;
                                target_row = -1;
                                found_path = true;
                                break;
                            }
                        } else if (col_entries[eid].size() == 2) {
                            int nxt_row = (col_entries[eid][0].row == cur_row) ? col_entries[eid][1].row : col_entries[eid][0].row;
                            int coeff_nxt = (col_entries[eid][0].row == cur_row) ? col_entries[eid][1].coeff : col_entries[eid][0].coeff;
                            if (nxt_row == cur_row || nxt_row == static_cast<int>(r_start)) continue;

                            int pushed_step = delta * coeff_nxt;
                            if (r[nxt_row] * pushed_step < 0 && new_sum_delta <= 2) {
                                final_edge = eid;
                                final_delta = delta;
                                final_prev_state = cur_state;
                                target_row = nxt_row;
                                target_step = pushed_step;
                                found_path = true;
                                break;
                            }

                            int nxt_step_in = -pushed_step;
                            int nxt_state = 2 * nxt_row + (nxt_step_in > 0 ? 1 : 0);
                            if (depth[nxt_state] < 0) {
                                depth[nxt_state] = cur_d + 1;
                                visited[nxt_state] = {cur_state, eid, delta, new_sum_delta};
                                queue.push_back(nxt_state);
                            }
                        }
                    }
                }

                if (!found_path) break;

                // Apply augmenting path
                int64_t old_E = static_cast<int64_t>(r[r_start]) * r[r_start];
                r[r_start] += start_step;
                int64_t new_E = static_cast<int64_t>(r[r_start]) * r[r_start];
                if (target_row >= 0) {
                    old_E += static_cast<int64_t>(r[target_row]) * r[target_row];
                    r[target_row] += target_step;
                    new_E += static_cast<int64_t>(r[target_row]) * r[target_row];
                }
                E_res += (new_E - old_E);

                d_int[final_edge] += final_delta;
                for (int s = final_prev_state; s != init_state && s >= 0; s = visited[s].prev_state) {
                    d_int[visited[s].edge_id] += visited[s].delta;
                }
                any_augmented = true;
            }
        }
        if (!any_augmented) break;
    }

        // exact residual (guards against any drift between updates)
        E_res = 0;
        for (int val : r) E_res += static_cast<int64_t>(val) * val;

        // ---- keep the best restart ----
        int64_t drift = 0;
        for (size_t e = 0; e < num_edges; ++e) {
            int64_t diff = static_cast<int64_t>(d_int[e]) -
                           static_cast<int64_t>(std::lround(d0[e]));
            drift += (diff < 0) ? -diff : diff;
        }
        bool better = (E_best < 0) || (E_res < E_best) ||
                      (E_res == E_best && drift < drift_best);
        if (better) {
            E_best = E_res;
            drift_best = drift;
            d_best = d_int;
            r_best = r;
        }
        if (E_best == 0 && restart >= 8) break;
    } // for restart

    if (E_best >= 0) {
        d_int = std::move(d_best);
        r = std::move(r_best);
        E_res = E_best;
    }

    // 5b. Patch-local corner refinement: for patches whose opposite side sums still differ,
    //     search an alternative 4-corner split that makes both pairs exactly equal under the
    //     solved divisions. Removes degenerate border cells without touching d_int.
    size_t refined_patches = 0, still_mismatched = 0;
    {
        auto side_sums = [&](const QuasiQuadPatch& patch, int& a, int& b, int& c, int& dd) {
            size_t M = patch.boundary_loop.size();
            a = b = c = dd = 0;
            size_t i0 = patch.corner_indices[0], i1 = patch.corner_indices[1];
            size_t i2 = patch.corner_indices[2], i3 = patch.corner_indices[3];
            for (size_t k = i0; k < i1 && k < M; ++k) a += d_int[patch.boundary_loop[k].edge_path_id];
            for (size_t k = i1; k < i2 && k < M; ++k) b += d_int[patch.boundary_loop[k].edge_path_id];
            for (size_t k = i2; k < i3 && k < M; ++k) c += d_int[patch.boundary_loop[k].edge_path_id];
            for (size_t k = i3; k < M; ++k) dd += d_int[patch.boundary_loop[k].edge_path_id];
            for (size_t k = 0; k < i0 && k < M; ++k) dd += d_int[patch.boundary_loop[k].edge_path_id];
        };
        for (auto& patch : graph.patches) {
            if (patch.boundary_loop.size() < 5) continue;
            int s0, s1, s2, s3;
            side_sums(patch, s0, s1, s2, s3);
            if (s0 == s2 && s1 == s3 && s0 >= 1 && s1 >= 1) continue;
            if (detect_four_corners(patch, &d_int)) refined_patches++;
            else still_mismatched++;
        }
    }
    std::cout << "[stage5/corners] refined=" << refined_patches
              << " still_mismatched=" << still_mismatched << std::endl;

    int rows_needing_fix = 0;
    for (int val : r) if (val != 0) rows_needing_fix++;
    std::cout << "[stage5/kkt] residual_norm=" << E_res << " rows_needing_fix=" << rows_needing_fix << std::endl;

    for (size_t e = 0; e < num_edges; ++e) {
        graph.edge_paths[e].integer_divisions = d_int[e];
    }

    // A patch loop needs at least 4 samples for the sample-level 4-corner split. When
    // micro-path collapse and zero divisions leave a loop with fewer, hand the missing
    // divisions to the LONGEST paths of that loop only: a patch side must never sum to 0,
    // but raising every path back to >= 1 would restore the N >= #paths floor that caused
    // the polygon explosion.
    {
        bool boosted = true;
        int guard = 0;
        while (boosted && guard++ < 8) {
            boosted = false;
            for (const auto& patch : graph.patches) {
                if (patch.boundary_loop.size() < 2) continue;
                int64_t total = 0;
                for (const auto& eref : patch.boundary_loop) {
                    if (eref.edge_path_id < num_edges) {
                        total += graph.edge_paths[eref.edge_path_id].integer_divisions;
                    }
                }
                if (total >= 4) continue;
                uint32_t best = INVALID_INDEX;
                float best_len = -1.0f;
                for (const auto& eref : patch.boundary_loop) {
                    if (eref.edge_path_id >= num_edges) continue;
                    const auto& ep = graph.edge_paths[eref.edge_path_id];
                    if (ep.continuous_len > best_len) {
                        best_len = ep.continuous_len;
                        best = eref.edge_path_id;
                    }
                }
                if (best == INVALID_INDEX) break;
                graph.edge_paths[best].integer_divisions += 1;
                boosted = true;
            }
        }
    }

    // (Sliver loops with < 4 paths are intentionally NOT special-cased here: bumping their
    //  divisions produced isolated islands - the patch across such a path does not carry
    //  that path in its own loop, so the extra samples cannot stitch anything together.)

    // ---- Sliver guard (T8 experiment: QREMESH_MIN_DIV, default OFF) -----------------
    // Measurement behind it: 3025 paths carry d=1 while the opposite side of the same patch
    // sums to up to 15 ([stage5/divs] one=3025 max=15, [stage5/sides] ratio>2 on 432 patches),
    // and the pleat rate at stage5 output is 10.3% of interior edges vs 0.7% on a clean control
    // (test_out/pleat_count.py). A 1:15 strip is where the index-correspondence fill folds a
    // quad back over its neighbour. Raising a whole PATH (a path is shared by exactly two
    // patches, so both borders move together - invariant #5 holds; and this runs before the
    // GF(2) parity fix, so parity is repaired afterwards) removes the thinnest rows.
    // Paths with d=0 are left alone on purpose: that zero is the seam-matching mechanism
    // (invariant #4), not a sliver.
    {
        std::vector<int> dv(num_edges);
        for (size_t e = 0; e < num_edges; ++e) dv[e] = graph.edge_paths[e].integer_divisions;
        size_t raised_floor = 0;
        if (min_div > 1) {
            for (size_t e = 0; e < num_edges; ++e) {
                int fd = floor_d(dv[e]);
                if (fd != dv[e]) { dv[e] = fd; ++raised_floor; }
            }
        }
        const size_t raised_bal = apply_side_balance(dv);
        if (raised_floor || raised_bal) {
            for (size_t e = 0; e < num_edges; ++e) graph.edge_paths[e].integer_divisions = dv[e];
            std::cout << "[stage5/sidebal] ratio=" << side_bal << " min_div=" << min_div
                      << " floor_raised=" << raised_floor << " balance_raised=" << raised_bal
                      << std::endl;
        }
    }

    // ---- Parity fix: every patch loop must hold an EVEN number of samples -----------
    // The sample-level 4-corner split (i2 = N/2, i3 = i1 + N/2) needs N even. Flipping
    // one path's division count toggles the parity of exactly the two patches that share
    // it, so this is a linear system over GF(2): one row per patch, one unknown per path.
    // Solving it keeps the metric densities intact (unlike forcing every path to an even
    // count, which raised the minimum patch density and broke the quad budget).
    {
        const size_t n_paths = num_edges;
        const size_t W = (n_paths + 63) / 64;
        struct GfRow {
            std::vector<uint64_t> bits;
            uint8_t b = 0;
        };
        std::vector<GfRow> rows;
        rows.reserve(graph.patches.size());
        for (const auto& patch : graph.patches) {
            if (patch.boundary_loop.size() < 4) continue;
            GfRow row;
            row.bits.assign(W, 0);
            int64_t N = 0;
            for (const auto& eref : patch.boundary_loop) {
                uint32_t e = eref.edge_path_id;
                if (e >= n_paths) continue;
                int d = graph.edge_paths[e].integer_divisions;
                N += d;
                // Any path may be flipped; patches that stay odd afterwards are fixed
                // locally by duplicating one sample in extract_patch_quads.
                (void)d;
                row.bits[e >> 6] ^= (1ull << (e & 63));
            }
            row.b = static_cast<uint8_t>(N & 1); // odd number of flips -> N becomes even
            rows.push_back(std::move(row));
        }

        auto leading_bit = [](const GfRow& row, size_t ncols) -> size_t {
            for (size_t c = 0; c < ncols; ++c) {
                if ((row.bits[c >> 6] >> (c & 63)) & 1ull) return c;
            }
            return ncols;
        };

        size_t r = 0;
        for (size_t c = 0; c < n_paths && r < rows.size(); ++c) {
            size_t sel = r;
            bool found = false;
            for (size_t i = r; i < rows.size(); ++i) {
                if ((rows[i].bits[c >> 6] >> (c & 63)) & 1ull) { sel = i; found = true; break; }
            }
            if (!found) continue;
            std::swap(rows[r], rows[sel]);
            for (size_t i = 0; i < rows.size(); ++i) {
                if (i != r && ((rows[i].bits[c >> 6] >> (c & 63)) & 1ull)) {
                    for (size_t w = 0; w < W; ++w) rows[i].bits[w] ^= rows[r].bits[w];
                    rows[i].b ^= rows[r].b;
                }
            }
            ++r;
        }

        std::vector<uint8_t> flip(n_paths, 0);
        size_t unsolvable = 0;
        for (const auto& row : rows) {
            size_t lead = leading_bit(row, n_paths);
            if (lead == n_paths) {
                if (row.b) unsolvable++;   // no assignment can fix this patch
                continue;
            }
            if (row.b) flip[lead] = 1;
        }
        size_t flips = 0;
        for (size_t e = 0; e < n_paths; ++e) {
            if (flip[e]) {
                graph.edge_paths[e].integer_divisions += 1;
                ++flips;
            }
        }
        std::cout << "[stage5/parity] flips=" << flips
                  << " unsolvable_patches=" << unsolvable << std::endl;
    }

    {
        size_t d0c = 0, d1c = 0, d23c = 0, dgt = 0, dmax = 0;
        for (size_t e = 0; e < num_edges; ++e) {
            int d = graph.edge_paths[e].integer_divisions;
            if (d == 0) d0c++; else if (d == 1) d1c++; else if (d <= 3) d23c++; else dgt++;
            if (static_cast<size_t>(d) > dmax) dmax = static_cast<size_t>(d);
        }
        std::cout << "[stage5/divs] zero=" << d0c << " one=" << d1c
                  << " two_three=" << d23c << " gt3=" << dgt << " max=" << dmax << std::endl;
    }

    for (size_t p = 0; p < num_patches; ++p) {
        auto& patch = graph.patches[p];
        size_t M = patch.boundary_loop.size();
        if (M < 4) {
            patch.nu = 1;
            patch.nv = 1;
        } else {
            size_t i0 = patch.corner_indices[0];
            size_t i1 = patch.corner_indices[1];
            size_t i2 = patch.corner_indices[2];
            size_t i3 = patch.corner_indices[3];

            int d0_sum = 0, d1_sum = 0, d2_sum = 0, d3_sum = 0;
            for (size_t k = i0; k < i1; ++k) d0_sum += graph.edge_paths[patch.boundary_loop[k].edge_path_id].integer_divisions;
            for (size_t k = i1; k < i2; ++k) d1_sum += graph.edge_paths[patch.boundary_loop[k].edge_path_id].integer_divisions;
            for (size_t k = i2; k < i3; ++k) d2_sum += graph.edge_paths[patch.boundary_loop[k].edge_path_id].integer_divisions;
            for (size_t k = i3; k < M; ++k)  d3_sum += graph.edge_paths[patch.boundary_loop[k].edge_path_id].integer_divisions;

            patch.nu = std::max(d0_sum, d2_sum);
            patch.nv = std::max(d1_sum, d3_sum);
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

    size_t M = patch.boundary_loop.size();
    if (M == 0) {
        g_extract_stats.skipped_tiny_loop++;
        return;
    }

    g_extract_stats.used_loop++;

    auto add_oriented_face = [&](std::vector<uint32_t> poly) {
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
            out_quads.polygons.push_back(std::move(cleaned));
            out_quads.material_ids.push_back(patch.material_id);
            g_extract_stats.sum_quads++;
        }
    };

    // Helper: collect sample vertices directly from canonical edge paths without any re-sampling
    auto get_side_sample_vertices = [&](size_t k_start, size_t k_end) -> std::vector<uint32_t> {
        std::vector<uint32_t> side_verts;
        for (size_t k = k_start; k < k_end; ++k) {
            const auto& eref = patch.boundary_loop[k];
            const auto& ep = graph.edge_paths[eref.edge_path_id];
            const auto& samples = ep.out_sample_vertices;
            if (samples.empty()) continue;

            if (eref.forward) {
                size_t start_s = side_verts.empty() ? 0 : 1;
                for (size_t s = start_s; s < samples.size(); ++s) {
                    side_verts.push_back(samples[s]);
                }
            } else {
                int start_s = side_verts.empty() ? static_cast<int>(samples.size()) - 1 : static_cast<int>(samples.size()) - 2;
                for (int s = start_s; s >= 0; --s) {
                    side_verts.push_back(samples[s]);
                }
            }
        }
        return side_verts;
    };

    if (M < 4) {
        std::vector<uint32_t> loop_verts = get_side_sample_vertices(0, M);
        if (loop_verts.size() > 1 && loop_verts.front() == loop_verts.back()) {
            loop_verts.pop_back();
        }

        if (loop_verts.size() == 3 || loop_verts.size() == 4) {
            add_oriented_face(loop_verts);
        } else if (loop_verts.size() > 4) {
            Vec3f centroid = Vec3f::Zero();
            for (uint32_t v_idx : loop_verts) {
                centroid += out_quads.positions[v_idx];
            }
            centroid /= static_cast<float>(loop_verts.size());
            Vec3f cp, cn;
            float d2;
            if (ctx.original_bvh.closest_point(centroid, cp, cn, d2)) {
                centroid = cp;
            }
            uint32_t c_idx = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(centroid);
            size_t L = loop_verts.size();
            for (size_t k = 0; k + 1 < L; k += 2) {
                add_oriented_face({loop_verts[k], loop_verts[k + 1], loop_verts[(k + 2) % L], c_idx});
            }
            if (L % 2 == 1) {
                add_oriented_face({loop_verts[L - 1], loop_verts[0], c_idx});
            }
        }
        return;
    }

    // ---- Sample-level corners ---------------------------------------------------
    // The whole patch border as one canonical sample sequence (junction vertices occur
    // once). Corners are placed IN SAMPLES, not on path boundaries, and opposite corners
    // are exactly half a loop apart: side0/side2 and side1/side3 then hold the same number
    // of samples BY CONSTRUCTION. Path-level corners could not guarantee that (only ~30 of
    // 197 patches had a feasible split), which produced duplicated border samples ->
    // degenerate cells, extra triangles and skewed quads.
    std::vector<uint32_t> samples;
    samples.reserve(64);
    for (size_t k = 0; k < M; ++k) {
        const auto& eref = patch.boundary_loop[k];
        const auto& ep = graph.edge_paths[eref.edge_path_id];
        const auto& sp = ep.out_sample_vertices;
        if (sp.empty()) continue;
        if (eref.forward) {
            size_t start_s = samples.empty() ? 0 : 1;
            for (size_t s = start_s; s < sp.size(); ++s) samples.push_back(sp[s]);
        } else {
            int start_s = samples.empty() ? static_cast<int>(sp.size()) - 1
                                          : static_cast<int>(sp.size()) - 2;
            for (int s = start_s; s >= 0; --s) samples.push_back(sp[static_cast<size_t>(s)]);
        }
    }
    if (samples.size() > 1 && samples.front() == samples.back()) samples.pop_back();

    size_t N = samples.size();
    if (N < 4) {
        g_extract_stats.skipped_tiny_loop++;
        return;
    }
    if (N % 2 != 0) {
        // Even path divisions make this practically unreachable; duplicate a sample
        // instead of dropping one (dropping would break stitching with the neighbour).
        samples.push_back(samples.back());
        ++N;
        g_extract_stats.parity_hacks++;
    }

    size_t i0 = 0;
    size_t i2 = N / 2;
    size_t i1 = N / 4;
    {
        // snap the u-side corner to the canonical path boundary nearest to the quarter point
        size_t acc = 0;
        size_t best = i1;
        size_t best_diff = i1;
        for (size_t k = 0; k + 1 < M; ++k) {
            const auto& eref = patch.boundary_loop[k];
            acc += static_cast<size_t>(std::max(1, graph.edge_paths[eref.edge_path_id].integer_divisions));
            if (acc >= i2) break;
            if (acc < 1) continue;
            size_t diff = (acc > i1) ? acc - i1 : i1 - acc;
            // same rule as in evaluate_total_quads (kept in sync so the count estimate
            // matches what extraction produces)
            if (diff <= best_diff) {
                best_diff = diff;
                best = acc;
            }
        }
        i1 = best;
    }
    if (i1 < 1) i1 = 1;
    if (i1 > i2 - 1) i1 = i2 - 1;
    size_t i3 = i1 + i2;

    auto slice_cw = [&](size_t a, size_t b) -> std::vector<uint32_t> {
        std::vector<uint32_t> out;
        for (size_t idx = a;; idx = (idx + 1) % N) {
            out.push_back(samples[idx]);
            if (idx == b) break;
        }
        return out;
    };
    auto side0 = slice_cw(i0, i1);
    auto side1 = slice_cw(i1, i2);
    auto side2 = slice_cw(i2, i3);
    auto side3 = slice_cw(i3, i0);

    size_t D0 = side0.size() - 1;
    size_t D1 = side1.size() - 1;
    size_t D2 = side2.size() - 1;
    size_t D3 = side3.size() - 1;

    int nu = static_cast<int>(D0);
    int nv = static_cast<int>(D1);
    g_extract_stats.sum_nu += static_cast<size_t>(nu);
    g_extract_stats.sum_nv += static_cast<size_t>(nv);
    g_extract_stats.patches_counted++;
    if (std::min(nu, nv) < 3) g_extract_stats.patches_min_dim_lt3++;
    if (D0 != D2 || D1 != D3) g_extract_stats.mismatch_patches++;

    std::vector<std::vector<uint32_t>> grid(nu + 1, std::vector<uint32_t>(nv + 1, INVALID_INDEX));

    for (int i = 0; i <= nu; ++i) {
        size_t k = (D0 == static_cast<size_t>(nu)) ? i : std::min(D0, static_cast<size_t>(std::round(static_cast<double>(i * D0) / nu)));
        grid[i][0] = side0[k];
    }
    for (int j = 0; j <= nv; ++j) {
        size_t k = (D1 == static_cast<size_t>(nv)) ? j : std::min(D1, static_cast<size_t>(std::round(static_cast<double>(j * D1) / nv)));
        grid[nu][j] = side1[k];
    }
    for (int i = 0; i <= nu; ++i) {
        size_t k = (D2 == static_cast<size_t>(nu)) ? (nu - i) : std::min(D2, static_cast<size_t>(std::round(static_cast<double>((nu - i) * D2) / nu)));
        grid[i][nv] = side2[k];
    }
    for (int j = 0; j <= nv; ++j) {
        size_t k = (D3 == static_cast<size_t>(nv)) ? (nv - j) : std::min(D3, static_cast<size_t>(std::round(static_cast<double>((nv - j) * D3) / nv)));
        grid[0][j] = side3[k];
    }

    // Degenerate border slots: two adjacent grid nodes sharing one sample vertex
    for (int i = 0; i < nu; ++i) {
        if (grid[i][0] == grid[i + 1][0]) g_extract_stats.degenerate_slots++;
        if (grid[i][nv] == grid[i + 1][nv]) g_extract_stats.degenerate_slots++;
    }
    for (int j = 0; j < nv; ++j) {
        if (grid[nu][j] == grid[nu][j + 1]) g_extract_stats.degenerate_slots++;
        if (grid[0][j] == grid[0][j + 1]) g_extract_stats.degenerate_slots++;
    }

    // Build local patch BVH
    BVHTree patch_bvh;
    bool has_patch_bvh = false;
    {
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
        has_patch_bvh = patch_bvh.build(
            patch_pos.data(),
            static_cast<uint32_t>(patch_pos.size() / 3),
            patch_tris.data(),
            static_cast<uint32_t>(patch_tris.size() / 3)
        );
    }

    Vec3f P00 = out_quads.positions[grid[0][0]];
    Vec3f P10 = out_quads.positions[grid[nu][0]];
    Vec3f P11 = out_quads.positions[grid[nu][nv]];
    Vec3f P01 = out_quads.positions[grid[0][nv]];

    for (int i = 1; i < nu; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(nu);
        // By value, NOT by reference: the loop below push_back()s into
        // out_quads.positions, which reallocates the buffer and would leave
        // references to border points dangling (heap-use-after-free).
        const Vec3f c0_pt = out_quads.positions[grid[i][0]];
        const Vec3f c2_pt = out_quads.positions[grid[i][nv]];

        for (int j = 1; j < nv; ++j) {
            float v = static_cast<float>(j) / static_cast<float>(nv);
            const Vec3f c3_pt = out_quads.positions[grid[0][j]];
            const Vec3f c1_pt = out_quads.positions[grid[nu][j]];

            Vec3f corner_bilinear = (1.0f - u) * (1.0f - v) * P00 +
                                     u * (1.0f - v) * P10 +
                                     u * v * P11 +
                                     (1.0f - u) * v * P01;

            Vec3f coons_pt = (1.0f - v) * c0_pt + v * c2_pt + (1.0f - u) * c3_pt + u * c1_pt - corner_bilinear;
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

    g_extract_stats.sum_interior_verts += static_cast<size_t>(std::max(0, nu - 1)) *
                                           static_cast<size_t>(std::max(0, nv - 1));

    // ---- Harmonic relaxation: Laplacian smoothing + BVH reprojection ------------------
    // The Coons/transfinite initialization above is used ONLY to seed the interior 3D
    // positions; on non-convex patch borders it folds cells (min SJ was NEGATIVE, warp
    // ~180 deg). We now drive the map toward a discrete harmonic one (the linear Laplacian
    // is the discrete analogue of harmonic parametrization):
    //   * Dirichlet borders: the 4 grid sides are shared canonical-path vertices and are
    //     NEVER moved, so patch stitching is untouched.
    //   * Each interior node (i,j) is set to the arithmetic mean of its 4 topological
    //     neighbours (i±1,j),(i,j±1). Gauss-Seidel (immediate in-place update, weight 1)
    //     converges ~2x faster than the old damped Jacobi (0.5, deferred) and actually
    //     untangles folded cells instead of crawling toward them.
    //   * Anti-shrinkage: the mean is a soap-film contraction, so immediately after each
    //     move the new point is snapped back onto the SOURCE surface (original_bvh closest
    //     point). Snapping every node keeps the sheet glued to the geometry and orthogonal-
    //     ising the cells, which is what lifts Scaled Jacobian and kills aspect.
    // Fewer default sweeps preserve short-wavelength relief. The environment override
    // remains available for quality/performance A/B tests.
    int harmonic_iters = 8;
    if (const char* e = std::getenv("QREMESH_HARMONIC_ITERS")) {
        int v = std::atoi(e);
        if (v >= 0) harmonic_iters = v;
    }
    const bool snap_final_only = std::getenv("QREMESH_SNAP_FINAL") != nullptr;  // S30 theory
    // Constrain the query to this patch first. The global source BVH contains every nearby
    // shell; querying it directly can jump across a thin wall or a touching AI-generated part.
    auto project_to_local_source = [&](const Vec3f& point, Vec3f& projected) -> bool {
        if (has_patch_bvh) {
            Vec3f patch_point, patch_normal;
            float patch_d2 = 0.0f;
            if (patch_bvh.closest_point(point, patch_point, patch_normal, patch_d2)) {
                Vec3f source_point, source_normal;
                float source_d2 = 0.0f;
                const float lift_limit_sq = 0.5f * ctx.base_target_length * ctx.base_target_length;
                if (ctx.original_bvh.closest_point(patch_point, source_point, source_normal, source_d2) &&
                    source_d2 <= lift_limit_sq && source_normal.dot(patch_normal) > 0.25f) {
                    projected = source_point;
                } else {
                    projected = patch_point;
                }
                return true;
            }
        }
        // Fallback for an empty/degenerate local patch BVH.
        Vec3f source_normal;
        float source_d2 = 0.0f;
        return ctx.original_bvh.closest_point(point, projected, source_normal, source_d2);
    };
    for (int it = 0; it < harmonic_iters; ++it) {
        for (int i = 1; i < nu; ++i) {
            for (int j = 1; j < nv; ++j) {
                Vec3f avg = out_quads.positions[grid[i - 1][j]] +
                            out_quads.positions[grid[i + 1][j]] +
                            out_quads.positions[grid[i][j - 1]] +
                            out_quads.positions[grid[i][j + 1]];
                avg *= 0.25f;
                Vec3f p = avg;            // weight 1: full step to the neighbour mean
                if (!snap_final_only) {
                    Vec3f projected;
                    if (project_to_local_source(p, projected)) p = projected;
                }
                out_quads.positions[grid[i][j]] = p;   // Gauss-Seidel: apply immediately
            }
        }
    }
    // Theory 2: relax the grid in free 3D space (chordal Laplacian orthogonalizes the net,
    // since the patch corners are ~90 deg per [stage5/corners]), THEN project every node to
    // the source ONCE. Per-iteration snapping (old behaviour) pulls each node back onto the
    // curved sheet every step, which UNDOES the orthogonalization on curved patches - that is
    // why 100 iterations plateaued WORSE than 16. One final snap keeps fidelity but lets the
    // interior actually square up.
    if (snap_final_only) {
        for (int i = 1; i < nu; ++i) {
            for (int j = 1; j < nv; ++j) {
                Vec3f projected;
                if (project_to_local_source(out_quads.positions[grid[i][j]], projected))
                    out_quads.positions[grid[i][j]] = projected;
            }
        }
    }

    for (int i = 0; i < nu; ++i) {
        for (int j = 0; j < nv; ++j) {
            uint32_t v0 = grid[i][j];
            uint32_t v1 = grid[i + 1][j];
            uint32_t v2 = grid[i + 1][j + 1];
            uint32_t v3 = grid[i][j + 1];

            add_oriented_face({v0, v1, v2, v3});
        }
    }
}

void Stage5ParamAndExtract::weld_and_inherit_attributes(
    RemeshContext& ctx,
    QuadMeshResult& out_quads
) {
    if (out_quads.positions.empty() || out_quads.polygons.empty()) return;

    // T15: this step NEVER moves positions (the spatial-hash positional weld lived here until
    // 84f1049 and is long gone), so any pleat change across it is an ADJACENCY effect of dropping
    // faces - the metric re-pairs edges - and not new folded geometry. Two phases so the effect
    // is attributable: (1) consecutive-duplicate cleanup + validity drops (degenerate, or non-quad
    // when triangles are not allowed), (2) duplicate-quad drop keeping the FIRST occurrence -
    // byte-identical result to the previous single interleaved pass.
    std::vector<std::vector<uint32_t>> valid_polys;
    std::vector<uint16_t> valid_mats;
    valid_polys.reserve(out_quads.polygons.size());
    valid_mats.reserve(out_quads.material_ids.size());
    const size_t polys_in = out_quads.polygons.size();

    for (size_t p_idx = 0; p_idx < out_quads.polygons.size(); ++p_idx) {
        const auto& poly = out_quads.polygons[p_idx];

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
    {
        const PleatCounts pc = count_pleats_t15(out_quads.positions, valid_polys);
        std::cout << "[stage5/t15] stage=post_invalid polys=" << valid_polys.size()
                  << " pleats_gated=" << pc.gated << " pleats_sameside=" << pc.sameside
                  << " dropped_invalid=" << (polys_in - valid_polys.size()) << std::endl;
    }

    std::vector<std::vector<uint32_t>> final_polys;
    std::vector<uint16_t> final_mats;
    final_polys.reserve(valid_polys.size());
    final_mats.reserve(valid_mats.size());
    std::unordered_set<uint64_t> seen_quad_keys;

    auto make_quad_key = [](uint32_t a, uint32_t b, uint32_t c, uint32_t d) -> uint64_t {
        uint32_t arr[4] = { a, b, c, d };
        std::sort(arr, arr + 4);
        return (static_cast<uint64_t>(arr[0]) * 73856093) ^
               (static_cast<uint64_t>(arr[1]) * 19349663) ^
               (static_cast<uint64_t>(arr[2]) * 83492791) ^
               (static_cast<uint64_t>(arr[3]) * 31415926);
    };

    for (size_t i = 0; i < valid_polys.size(); ++i) {
        auto& poly = valid_polys[i];
        if (poly.size() == 4) {
            uint64_t qk = make_quad_key(poly[0], poly[1], poly[2], poly[3]);
            if (seen_quad_keys.insert(qk).second) {
                final_polys.push_back(std::move(poly));
                final_mats.push_back(valid_mats[i]);
            }
        } else {
            final_polys.push_back(std::move(poly));
            final_mats.push_back(valid_mats[i]);
        }
    }
    {
        const PleatCounts pc = count_pleats_t15(out_quads.positions, final_polys);
        std::cout << "[stage5/t15] stage=post_dedupe polys=" << final_polys.size()
                  << " pleats_gated=" << pc.gated << " pleats_sameside=" << pc.sameside
                  << " dropped_duplicate=" << (valid_polys.size() - final_polys.size()) << std::endl;
    }

    out_quads.polygons = std::move(final_polys);
    out_quads.material_ids = std::move(final_mats);
}

// ---------------------------------------------------------------------------
// T16: monotone re-lay of the interior nodes of a patch grid.
//
// Why this exists (measured in the [stage5/overlapmap] census): 102 of the 132 patches that hold
// a fold show BOTH >3% double-covered and >3% uncovered area inside their own border region -
// the grid does not tile its own border, it overlaps some cells and skips others. Border vertices
// are already shared between the two patches, so they must not move (invariant #5: both neighbours
// need the identical sample set); the only free DOF are the patch's INTERIOR nodes. Those are
// exactly the DOF that decide whether the tiling is monotone.
//
// Method: solve the grid in the patch's own plane - discrete Laplacian on interior nodes with the
// border held fixed (Dirichlet), sweeping until the 2D overlap measure stops improving while no
// cell may turn inside out (positive signed area for every fan triangle). A planar harmonic map of
// a disk with a simple boundary is the standard non-overlapping fill, which is what Coons-by-index
// fails to be on these borders. Then the improved planar positions are mapped back onto the source
// surface (closest_point, invariant #7) under a travel cap, and the patch is only kept if its
// fold census and its worst quad both got no worse. Deterministic: fixed sweep order over sorted
// vertex indices.
// ---------------------------------------------------------------------------
void Stage5ParamAndExtract::untangle_patch_grids(
    RemeshContext& ctx,
    QuadMeshResult& out_quads,
    const std::vector<uint32_t>& face_patch,
    const std::vector<uint32_t>& patches
) {
    if (patches.empty()) return;
    const size_t nf = out_quads.polygons.size();
    const size_t nv = out_quads.positions.size();
    if (nv == 0 || nf == 0) return;

    auto key2 = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };
    auto newell = [&](const std::vector<uint32_t>& q) -> Vec3f {
        Vec3d n = Vec3d::Zero();
        const size_t pn = q.size();
        for (size_t i = 0; i < pn; ++i) {
            Vec3d a = out_quads.positions[q[i]].cast<double>();
            Vec3d b = out_quads.positions[q[(i + 1) % pn]].cast<double>();
            n += a.cross(b);
        }
        const double ln = n.norm();
        return ln > 1e-14 ? (n / ln).cast<float>() : Vec3f(0, 0, 0);
    };
    // fold census restricted to a set of faces: two faces of the set sharing an edge, normal angle
    // > 120 deg and free vertices on the SAME side of that edge
    auto count_folds = [&](const std::vector<uint32_t>& faces) -> size_t {
        struct ER { uint32_t f0, f1, w0, w1; };
        std::unordered_map<uint64_t, ER> em;
        for (uint32_t fi : faces) {
            const auto& q = out_quads.polygons[fi];
            const size_t pn = q.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = q[i], b = q[(i + 1) % pn];
                if (a == b) continue;
                uint32_t free_v = INVALID_INDEX;
                for (size_t k = 0; k < pn; ++k)
                    if (q[k] != a && q[k] != b) { free_v = q[k]; break; }
                uint64_t k2 = key2(a, b);
                auto it = em.find(k2);
                if (it == em.end()) em.emplace(k2, ER{fi, INVALID_INDEX, free_v, INVALID_INDEX});
                else if (it->second.f1 == INVALID_INDEX) { it->second.f1 = fi; it->second.w1 = free_v; }
            }
        }
        std::unordered_map<uint32_t, Vec3f> fn;
        for (uint32_t fi : faces) fn[fi] = newell(out_quads.polygons[fi]);
        size_t folds = 0;
        for (const auto& [k2, r] : em) {
            if (r.f1 == INVALID_INDEX || r.w0 == INVALID_INDEX || r.w1 == INVALID_INDEX) continue;
            const Vec3f n0 = fn[r.f0], n1 = fn[r.f1];
            const float dih = std::acos(std::max(-1.0f, std::min(1.0f, n0.dot(n1)))) * 180.0f / PI_F;
            if (dih <= 120.0f) continue;
            const uint32_t a = static_cast<uint32_t>(k2 >> 32), b = static_cast<uint32_t>(k2 & 0xFFFFFFFFu);
            Vec3f e = out_quads.positions[b] - out_quads.positions[a];
            const float el = e.norm();
            if (el < 1e-9f) continue;
            e /= el;
            const Vec3f mid = 0.5f * (out_quads.positions[a] + out_quads.positions[b]);
            Vec3f u0 = out_quads.positions[r.w0] - mid, u1 = out_quads.positions[r.w1] - mid;
            u0 -= u0.dot(e) * e;
            u1 -= u1.dot(e) * e;
            if (u0.dot(u1) > 0.0f) folds++;
        }
        return folds;
    };

    // vertex -> faces, and the median edge of the whole mesh (travel cap reference)
    std::vector<std::vector<uint32_t>> vfaces(nv);
    std::vector<float> all_len;
    all_len.reserve(nf * 4);
    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& q = out_quads.polygons[fi];
        const size_t pn = q.size();
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = q[i], b = q[(i + 1) % pn];
            if (a < nv) vfaces[a].push_back(static_cast<uint32_t>(fi));
            if (a < nv && b < nv && a != b)
                all_len.push_back((out_quads.positions[a] - out_quads.positions[b]).norm());
        }
    }
    std::nth_element(all_len.begin(), all_len.begin() + all_len.size() / 2, all_len.end());
    const float med_e = all_len.empty() ? 1.0f : all_len[all_len.size() / 2];

    bool t16_snap = true;
    if (const char* e = std::getenv("QREMESH_T16_SNAP")) t16_snap = (std::atoi(e) != 0);
    size_t fixed_patches = 0, skipped_nointerior = 0, reverted = 0;
    size_t r_nocand = 0, r_folds = 0, r_sj = 0, r_noimprove = 0;
    size_t folds_before_total = 0, folds_after_total = 0;

    for (uint32_t p : patches) {
        std::vector<uint32_t> faces;
        for (size_t fi = 0; fi < nf; ++fi) if (face_patch[fi] == p) faces.push_back(static_cast<uint32_t>(fi));
        if (faces.size() < 2) continue;

        std::unordered_set<uint32_t> vset;
        for (uint32_t fi : faces) for (uint32_t v : out_quads.polygons[fi]) vset.insert(v);
        // patch-local adjacency
        std::vector<uint32_t> verts(vset.begin(), vset.end());
        std::sort(verts.begin(), verts.end());
        std::unordered_map<uint32_t, size_t> vpos;
        for (size_t i = 0; i < verts.size(); ++i) vpos[verts[i]] = i;
        std::vector<std::vector<size_t>> nbr(verts.size());
        for (uint32_t fi : faces) {
            const auto& q = out_quads.polygons[fi];
            const size_t pn = q.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = q[i], b = q[(i + 1) % pn];
                if (a == b) continue;
                const size_t ia = vpos[a], ib = vpos[b];
                nbr[ia].push_back(ib);
                nbr[ib].push_back(ia);
            }
        }
        for (auto& v : nbr) {
            std::sort(v.begin(), v.end());
            v.erase(std::unique(v.begin(), v.end()), v.end());
        }
        // interior = a vertex whose every incident face belongs to this patch
        std::vector<uint8_t> interior(verts.size(), 0);
        for (size_t i = 0; i < verts.size(); ++i) {
            const uint32_t v = verts[i];
            bool only_here = !vfaces[v].empty();
            for (uint32_t fi : vfaces[v]) if (fi >= nf || face_patch[fi] != p) { only_here = false; break; }
            interior[i] = only_here ? 1 : 0;
        }
        size_t n_interior = 0;
        for (size_t i = 0; i < verts.size(); ++i) n_interior += interior[i] ? 1 : 0;
        if (n_interior == 0) { skipped_nointerior++; continue; }   // 2x2 grid: nothing to re-lay

        // The fold census must include the SEAM: 55% of folds are between two patches (S45), so
        // judging a re-lay only on the patch's own faces would accept moves that shift the fold
        // across the border instead of removing it. Add the 1-ring of foreign faces.
        std::unordered_map<uint64_t, std::vector<uint32_t>> face_edge_map;
        for (uint32_t fi : faces) {
            const auto& q = out_quads.polygons[fi];
            const size_t pn = q.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = q[i], b = q[(i + 1) % pn];
                if (a != b) face_edge_map[key2(a, b)].push_back(fi);
            }
        }
        std::vector<uint32_t> scope = faces;
        {
            std::unordered_set<uint32_t> in_scope(faces.begin(), faces.end());
            for (const auto& [k2, fl] : face_edge_map) {
                for (uint32_t g : fl) {
                    if (g < nf && face_patch[g] != p && in_scope.insert(g).second) scope.push_back(g);
                }
            }
            std::sort(scope.begin(), scope.end());
        }
        const size_t folds_before = count_folds(scope);
        folds_before_total += folds_before;

        // patch plane
        Vec3d cen = Vec3d::Zero();
        for (uint32_t v : verts) cen += out_quads.positions[v].cast<double>();
        cen /= static_cast<double>(verts.size());
        Mat3d cov = Mat3d::Zero();
        for (uint32_t v : verts) { Vec3d d = out_quads.positions[v].cast<double>() - cen; cov += d * d.transpose(); }
        Eigen::SelfAdjointEigenSolver<Mat3d> es(cov);
        const Vec3d ax1 = es.eigenvectors().col(2), ax2 = es.eigenvectors().col(1);
        std::vector<std::pair<double, double>> xy(verts.size());
        std::vector<Vec3f> xyz(verts.size());
        for (size_t i = 0; i < verts.size(); ++i) {
            Vec3d d = out_quads.positions[verts[i]].cast<double>() - cen;
            xy[i] = { d.dot(ax1), d.dot(ax2) };
            xyz[i] = out_quads.positions[verts[i]];
        }

        auto measure = [&](const std::vector<std::pair<double, double>>& X, bool& any_inverted) -> double {
            // 2D overlap fraction over a coarse raster of the patch, plus inverted-cell detection
            any_inverted = false;
            double lo1 = 1e30, hi1 = -1e30, lo2 = 1e30, hi2 = -1e30;
            for (auto& q : X) { lo1 = std::min(lo1, q.first); hi1 = std::max(hi1, q.first);
                                 lo2 = std::min(lo2, q.second); hi2 = std::max(hi2, q.second); }
            const int RES = 24;
            const double s1 = (hi1 - lo1) > 1e-12 ? RES / (hi1 - lo1) : 1.0;
            const double s2 = (hi2 - lo2) > 1e-12 ? RES / (hi2 - lo2) : 1.0;
            std::vector<uint8_t> cov2(RES * RES, 0);
            for (uint32_t fi : faces) {
                const auto& q = out_quads.polygons[fi];
                for (size_t i = 1; i + 1 < q.size(); ++i) {
                    const auto A = X[vpos[q[0]]], B = X[vpos[q[i]]], C = X[vpos[q[i + 1]]];
                    const double ar = 0.5 * ((B.first - A.first) * (C.second - A.second) -
                                             (C.first - A.first) * (B.second - A.second));
                    if (std::abs(ar) < 1e-16) continue;
                    if (ar < 0.0) any_inverted = true;
                    double mnx = std::min({A.first, B.first, C.first}), mxx = std::max({A.first, B.first, C.first});
                    double mny = std::min({A.second, B.second, C.second}), mxy = std::max({A.second, B.second, C.second});
                    const int x0 = std::max(0, static_cast<int>((mnx - lo1) * s1));
                    const int x1 = std::min(RES - 1, static_cast<int>((mxx - lo1) * s1));
                    const int y0 = std::max(0, static_cast<int>((mny - lo2) * s2));
                    const int y1 = std::min(RES - 1, static_cast<int>((mxy - lo2) * s2));
                    const double d2 = 2.0 * ar;
                    for (int yy = y0; yy <= y1; ++yy)
                        for (int xx = x0; xx <= x1; ++xx) {
                            const double px = lo1 + (xx + 0.5) / s1, py = lo2 + (yy + 0.5) / s2;
                            const double w0 = ((B.second - C.second) * (px - C.first) + (C.first - B.first) * (py - C.second)) / d2;
                            const double w1 = ((C.second - A.second) * (px - C.first) + (A.first - C.first) * (py - C.second)) / d2;
                            const double w2 = 1.0 - w0 - w1;
                            if (w0 >= -1e-9 && w1 >= -1e-9 && w2 >= -1e-9) {
                                uint8_t& c = cov2[yy * RES + xx];
                                if (c < 255) c++;
                            }
                        }
                }
            }
            long single = 0, multi = 0;
            for (uint8_t c : cov2) { if (c == 1) single++; else if (c >= 2) multi++; }
            const long tot = single + multi;
            return tot > 0 ? static_cast<double>(multi) / tot : 0.0;
        };

        bool inv0 = false;
        double best_score = measure(xy, inv0) + (inv0 ? 1.0 : 0.0);
        auto best_xy = xy;
        auto cur = xy;
        for (int sweep = 0; sweep < 120; ++sweep) {
            for (size_t i = 0; i < verts.size(); ++i) {
                if (!interior[i] || nbr[i].empty()) continue;
                double sx = 0.0, sy = 0.0;
                for (size_t j : nbr[i]) { sx += cur[j].first; sy += cur[j].second; }
                cur[i] = { sx / static_cast<double>(nbr[i].size()), sy / static_cast<double>(nbr[i].size()) };
            }
            bool inv = false;
            const double ov = measure(cur, inv);
            const double score = ov + (inv ? 1.0 : 0.0);
            if (score < best_score - 1e-6) { best_score = score; best_xy = cur; }
            if (ov <= 0.005 && !inv) break;      // monotone tiling reached
        }
        if (best_score >= measure(xy, inv0) + (inv0 ? 1.0 : 0.0) - 1e-9) { reverted++; r_noimprove++; continue; }

        // map the improved planar positions back into 3D and onto the source
        std::vector<uint32_t> moved_ids;
        std::vector<Vec3f> moved_pos;
        for (size_t i = 0; i < verts.size(); ++i) {
            if (!interior[i]) continue;
            Vec3d nd = best_xy[i].first * ax1 + best_xy[i].second * ax2;
            Vec3f cand = (cen + nd).cast<float>();
            // Snap back onto the source by default (invariant #7). QREMESH_T16_SNAP=0 keeps the
            // planar position: the re-projected node can land on the OTHER sheet of a fold and
            // put the crease back, which is what made 79 of 102 candidates revert.
            if (t16_snap) {
                Vec3f snap, nrm; float d2;
                if (ctx.original_bvh.closest_point(cand, snap, nrm, d2)) cand = snap;
            }
            const float travel = (cand - xyz[i]).norm();
            if (travel > 1.5f * med_e) continue;                     // anti-migration
            moved_ids.push_back(verts[i]);
            moved_pos.push_back(cand);
        }
        if (moved_ids.empty()) { reverted++; r_nocand++; continue; }

        const std::vector<Vec3f> backup_pos = out_quads.positions;
        for (size_t i = 0; i < moved_ids.size(); ++i) out_quads.positions[moved_ids[i]] = moved_pos[i];

        const size_t folds_after = count_folds(scope);
        float sj_min_before = 1.0f, sj_min_after = 1.0f;
        for (uint32_t fi : faces) {
            const auto& q = out_quads.polygons[fi];
            if (q.size() != 4) continue;
            sj_min_after = std::min(sj_min_after, sj5(
                out_quads.positions[q[0]], out_quads.positions[q[1]],
                out_quads.positions[q[2]], out_quads.positions[q[3]]));
        }
        out_quads.positions = backup_pos;
        for (uint32_t fi : faces) {
            const auto& q = out_quads.polygons[fi];
            if (q.size() != 4) continue;
            sj_min_before = std::min(sj_min_before, sj5(
                out_quads.positions[q[0]], out_quads.positions[q[1]],
                out_quads.positions[q[2]], out_quads.positions[q[3]]));
        }
        // apply for real only if it is a strict improvement
        for (size_t i = 0; i < moved_ids.size(); ++i) out_quads.positions[moved_ids[i]] = moved_pos[i];
        if (folds_after > folds_before) {
            out_quads.positions = backup_pos;
            reverted++; r_folds++;
            continue;
        }
        if (sj_min_after < sj_min_before - 1e-3f) {
            out_quads.positions = backup_pos;
            reverted++; r_sj++;
            continue;
        }
        folds_after_total += folds_after;
        fixed_patches++;
    }

    std::cout << "[stage5/untangle] candidates=" << patches.size() << " relaid=" << fixed_patches
              << " no_interior=" << skipped_nointerior << " reverted=" << reverted
              << " rej(no_improve=" << r_noimprove << " no_move=" << r_nocand
              << " folds_up=" << r_folds << " sj_worse=" << r_sj << ")"
              << " snap=" << (t16_snap ? 1 : 0)
              << " folds_in_candidates " << folds_before_total << "->" << folds_after_total
              << std::endl;
}

// ---------------------------------------------------------------------------
// T17: paired probe - the seam-side counterpart of T16.
//
// Why: the [stage5/pleatmap] census splits pleats into INTRA (a patch folds over itself -
// T16's monotone re-lay addresses those) and CROSS (the two faces live in DIFFERENT patches,
// 431 of 787 on the AI model, S45/S46). T16 never moves a seam vertex (invariant #5), so the
// cross half stays. Hypothesis under test: a seam pair whose BOTH patches measure >3% overlap
// AND >3% gap still has slack, and rotating the free vertices of side A and side B around the
// shared edge (a,b) by OPPOSITE angles - the symmetric "flag reflection" - unfolds the crease
// while the seam loop itself never moves.
//
// Measure-first (invariant #16): runs only under QREMESH_T17 (default OFF), iterates records
// strictly in vector order, greedy (an accepted move is visible to later records), no random.
// Per candidate: rotate -> snap onto the source (invariant #7) -> travel cap 1.5*median edge;
// the chosen candidate must then clear the pleat, create NO new inverted quad (sj5), and must
// not raise the pleat count in the moved-faces 1-ring. Telemetry is printed even on rejects.
// ---------------------------------------------------------------------------
void Stage5ParamAndExtract::t17_paired_probe(
    RemeshContext& ctx,
    QuadMeshResult& out_quads,
    const std::vector<uint32_t>& face_patch,
    const std::vector<T17CrossPleat>& records,
    const std::vector<float>& ov,
    const std::vector<float>& gp
) {
    if (records.empty()) return;
    const size_t nf = out_quads.polygons.size();
    const size_t nv = out_quads.positions.size();
    if (nf == 0 || nv == 0) return;

    auto key2 = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };
    auto newell = [&](uint32_t fi) -> Vec3f {
        const auto& p = out_quads.polygons[fi];
        Vec3d n = Vec3d::Zero();
        const size_t pn = p.size();
        for (size_t i = 0; i < pn; ++i) {
            Vec3d a = out_quads.positions[p[i]].cast<double>();
            Vec3d b = out_quads.positions[p[(i + 1) % pn]].cast<double>();
            n += a.cross(b);
        }
        const double ln = n.norm();
        return ln > 1e-14 ? (n / ln).cast<float>() : Vec3f(0, 0, 0);
    };
    auto ang_deg = [](const Vec3f& n0, const Vec3f& n1) -> float {
        return std::acos(std::max(-1.0f, std::min(1.0f, n0.dot(n1)))) * 180.0f / PI_F;
    };
    // The SAME winding-independent pleat test as the [stage5/pleatmap] census: two faces of
    // the set sharing an edge, normal angle > 120 deg, free vertices on the SAME side of it.
    auto count_pleats = [&](const std::vector<uint32_t>& faces) -> size_t {
        struct ER { uint32_t f0, f1, w0, w1; };
        std::unordered_map<uint64_t, ER> em;
        for (uint32_t fi : faces) {
            if (fi >= nf) continue;
            const auto& q = out_quads.polygons[fi];
            const size_t pn = q.size();
            if (pn < 3) continue;
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = q[i], b = q[(i + 1) % pn];
                if (a == b) continue;
                uint32_t free_v = INVALID_INDEX;
                for (size_t k = 0; k < pn; ++k)
                    if (q[k] != a && q[k] != b) { free_v = q[k]; break; }
                uint64_t k2 = key2(a, b);
                auto it = em.find(k2);
                if (it == em.end()) em.emplace(k2, ER{fi, INVALID_INDEX, free_v, INVALID_INDEX});
                else if (it->second.f1 == INVALID_INDEX) { it->second.f1 = fi; it->second.w1 = free_v; }
            }
        }
        std::unordered_map<uint32_t, Vec3f> fn;
        for (uint32_t fi : faces) if (fi < nf) fn[fi] = newell(fi);
        size_t folds = 0;
        for (const auto& [k2, r] : em) {
            if (r.f1 == INVALID_INDEX || r.w0 == INVALID_INDEX || r.w1 == INVALID_INDEX) continue;
            const auto i0 = fn.find(r.f0), i1 = fn.find(r.f1);
            if (i0 == fn.end() || i1 == fn.end()) continue;
            if (ang_deg(i0->second, i1->second) <= 120.0f) continue;
            const uint32_t a = static_cast<uint32_t>(k2 >> 32), b = static_cast<uint32_t>(k2 & 0xFFFFFFFFu);
            Vec3f e = out_quads.positions[b] - out_quads.positions[a];
            const float el = e.norm();
            if (el < 1e-9f) continue;
            e /= el;
            const Vec3f mid = 0.5f * (out_quads.positions[a] + out_quads.positions[b]);
            Vec3f u0 = out_quads.positions[r.w0] - mid, u1 = out_quads.positions[r.w1] - mid;
            u0 -= u0.dot(e) * e;
            u1 -= u1.dot(e) * e;
            if (u0.dot(u1) > 0.0f) folds++;
        }
        return folds;
    };

    // vertex -> faces, edge -> faces (for the 1-ring scope) and the median edge length
    // (travel-cap reference, same as in untangle_patch_grids)
    std::vector<std::vector<uint32_t>> vfaces(nv);
    std::unordered_map<uint64_t, std::vector<uint32_t>> efaces;
    std::vector<float> all_len;
    all_len.reserve(nf * 4);
    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& q = out_quads.polygons[fi];
        const size_t pn = q.size();
        for (size_t i = 0; i < pn; ++i) {
            const uint32_t a = q[i], b = q[(i + 1) % pn];
            if (a < nv) vfaces[a].push_back(static_cast<uint32_t>(fi));
            if (a < nv && b < nv && a != b) {
                all_len.push_back((out_quads.positions[a] - out_quads.positions[b]).norm());
                efaces[key2(a, b)].push_back(static_cast<uint32_t>(fi));
            }
        }
    }
    float med_e = 1.0f;
    if (!all_len.empty()) {
        std::nth_element(all_len.begin(), all_len.begin() + all_len.size() / 2, all_len.end());
        med_e = all_len[all_len.size() / 2];
    }

    size_t trials = 0, og = 0, og_accepted = 0, nonog_accepted = 0;
    size_t r_no_dof = 0, r_travel = 0, r_pleat_remains = 0, r_new_inv = 0, r_folds_up = 0, r_stale = 0;
    size_t theta_reduced = 0, sj_scope_dropped = 0, moved_verts = 0;

    std::vector<uint32_t> all_faces(nf);
    for (size_t i = 0; i < nf; ++i) all_faces[i] = static_cast<uint32_t>(i);
    const size_t pleats_before_total = count_pleats(all_faces);

    auto has_og = [&](uint32_t p) -> bool {
        return p < ov.size() && p < gp.size() && ov[p] > 0.03f && gp[p] > 0.03f;
    };

    for (const auto& rec : records) {
        ++trials;
        const bool is_og = has_og(rec.pa) && has_og(rec.pb);
        if (is_og) ++og;
        // defensive: records come from this very mesh and T16 changes no topology
        if (rec.f0 >= nf || rec.f1 >= nf || rec.a >= nv || rec.b >= nv || rec.a == rec.b) continue;

        const uint32_t a = rec.a, b = rec.b;
        // side A = free vertices of f0 (not on the seam loop a,b) whose EVERY incident face
        // belongs to pa; side B likewise with pb - the interior definition T16 uses. A vertex
        // with a foreign incident face sits ON some border and must not be dragged along.
        auto interior_to = [&](uint32_t v, uint32_t p) -> bool {
            if (v >= vfaces.size() || vfaces[v].empty()) return false;
            for (uint32_t fi : vfaces[v])
                if (fi >= nf || fi >= face_patch.size() || face_patch[fi] != p) return false;
            return true;
        };
        std::vector<uint32_t> setA, setB;
        for (uint32_t v : out_quads.polygons[rec.f0]) {
            if (v == a || v == b) continue;
            if (std::find(setA.begin(), setA.end(), v) != setA.end()) continue;
            if (interior_to(v, rec.pa)) setA.push_back(v);
        }
        for (uint32_t v : out_quads.polygons[rec.f1]) {
            if (v == a || v == b) continue;
            if (std::find(setB.begin(), setB.end(), v) != setB.end()) continue;
            if (interior_to(v, rec.pb)) setB.push_back(v);
        }
        // degenerate pair sharing a 3rd vertex: side A wins, a vertex is rotated at most once
        setB.erase(std::remove_if(setB.begin(), setB.end(),
                                  [&](uint32_t v) {
                                      return std::find(setA.begin(), setA.end(), v) != setA.end();
                                  }),
                   setB.end());
        if (setA.empty() || setB.empty()) { ++r_no_dof; continue; }

        const float dih = ang_deg(newell(rec.f0), newell(rec.f1));
        if (dih <= 120.0f) { ++r_stale; continue; }   // already unfolded by an earlier move

        const Vec3f pivot = out_quads.positions[a];
        Vec3f axis = out_quads.positions[b] - pivot;
        const float el = axis.norm();
        if (el < 1e-9f) { ++r_stale; continue; }
        axis /= el;
        const Vec3f mid = 0.5f * (pivot + out_quads.positions[b]);
        uint32_t w0 = INVALID_INDEX, w1 = INVALID_INDEX;
        for (uint32_t v : out_quads.polygons[rec.f0]) if (v != a && v != b) { w0 = v; break; }
        for (uint32_t v : out_quads.polygons[rec.f1]) if (v != a && v != b) { w1 = v; break; }
        if (w0 == INVALID_INDEX || w1 == INVALID_INDEX) { ++r_no_dof; continue; }

        // gamma targets: with BOTH sides turning by gamma the dihedral closes by 2*gamma, so
        // these two values aim at 100 resp. 70 deg final angle (120 is the pleat threshold)
        const float gammas[2] = {
            std::max(0.0f, (dih - 100.0f) * 0.5f) * (PI_F / 180.0f),
            std::max(0.0f, (dih - 70.0f) * 0.5f) * (PI_F / 180.0f)
        };
        const int signs[2] = { +1, -1 };   // +1: side A -gamma / side B +gamma, -1 mirrored

        const std::vector<Vec3f> base_pos = out_quads.positions;   // greedy state at trial start
        struct Cand {
            bool cleans = false;
            float th_after = 180.0f;
            std::vector<uint32_t> mids;
            std::vector<Vec3f> mpos;
        };
        Cand best;
        bool have_best = false, any_valid = false;
        float min_th_valid = 180.0f;
        for (float gam : gammas) {
            for (int s : signs) {
                Cand c;
                bool ok = true;
                for (int side = 0; side < 2 && ok; ++side) {
                    const std::vector<uint32_t>& ids = (side == 0) ? setA : setB;
                    const float ang = (side == 0 ? -s : s) * gam;
                    for (uint32_t v : ids) {
                        const Vec3f orig = base_pos[v];
                        const Vec3f d = orig - pivot;
                        Vec3f r = d * std::cos(ang) + axis.cross(d) * std::sin(ang) +
                                  axis * (axis.dot(d)) * (1.0f - std::cos(ang));
                        Vec3f cand = pivot + r;
                        // snap onto the source (invariant #7, as in untangle_patch_grids)
                        Vec3f snap, nrm; float d2;
                        if (ctx.original_bvh.closest_point(cand, snap, nrm, d2)) cand = snap;
                        if ((cand - orig).norm() > 1.5f * med_e) { ok = false; break; }  // travel-reject
                        c.mids.push_back(v);
                        c.mpos.push_back(cand);
                    }
                }
                if (!ok) continue;
                // measure the candidate on the mesh, then restore the base state
                for (size_t i = 0; i < c.mids.size(); ++i) out_quads.positions[c.mids[i]] = c.mpos[i];
                c.th_after = ang_deg(newell(rec.f0), newell(rec.f1));
                Vec3f u0 = out_quads.positions[w0] - mid, u1 = out_quads.positions[w1] - mid;
                u0 -= u0.dot(axis) * axis;
                u1 -= u1.dot(axis) * axis;
                c.cleans = (c.th_after <= 120.0f) || (u0.dot(u1) <= 0.0f);
                for (size_t i = 0; i < c.mids.size(); ++i) out_quads.positions[c.mids[i]] = base_pos[c.mids[i]];
                any_valid = true;
                min_th_valid = std::min(min_th_valid, c.th_after);
                if (c.cleans && (!have_best || c.th_after < best.th_after)) {
                    best = std::move(c);
                    have_best = true;
                }
            }
        }
        if (!any_valid) { ++r_travel; continue; }
        if (!have_best) {
            // telemetry: how close the least-bad VALID candidate came to clearing the pleat
            if (min_th_valid < dih - 1.0f) ++theta_reduced;
            ++r_pleat_remains;
            continue;
        }
        if (best.th_after < dih - 1.0f) ++theta_reduced;   // counted even if a gate rejects below

        // scope: faces incident to moved vertices + the 1-ring over shared edges - the seam
        // fold must be judged together with its neighbours (same reasoning as in T16)
        std::vector<uint32_t> direct;
        {
            std::unordered_set<uint32_t> seen;
            for (uint32_t v : best.mids)
                for (uint32_t fi : vfaces[v])
                    if (fi < nf && seen.insert(fi).second) direct.push_back(fi);
        }
        std::vector<uint32_t> scope = direct;
        {
            std::unordered_set<uint32_t> seen(direct.begin(), direct.end());
            for (uint32_t fi : direct) {
                const auto& q = out_quads.polygons[fi];
                const size_t pn = q.size();
                for (size_t i = 0; i < pn; ++i) {
                    const uint32_t x = q[i], y = q[(i + 1) % pn];
                    if (x == y) continue;
                    auto it = efaces.find(key2(x, y));
                    if (it == efaces.end()) continue;
                    for (uint32_t g : it->second)
                        if (g < nf && seen.insert(g).second) scope.push_back(g);
                }
            }
        }

        auto quad_sj = [&](uint32_t fi) -> float {
            const auto& q = out_quads.polygons[fi];
            return sj5(out_quads.positions[q[0]], out_quads.positions[q[1]],
                       out_quads.positions[q[2]], out_quads.positions[q[3]]);
        };
        // BEFORE on the base state
        std::unordered_map<uint32_t, float> sj_before;
        float scope_sj_before = 1.0f;
        for (uint32_t fi : direct)
            if (out_quads.polygons[fi].size() == 4) sj_before[fi] = quad_sj(fi);
        for (uint32_t fi : scope)
            if (out_quads.polygons[fi].size() == 4) scope_sj_before = std::min(scope_sj_before, quad_sj(fi));
        const size_t folds_before = count_pleats(scope);

        for (size_t i = 0; i < best.mids.size(); ++i) out_quads.positions[best.mids[i]] = best.mpos[i];

        // gate 1: a quad that was valid (sj >= 0) must not turn inverted (sj < 0)
        bool new_inv = false;
        for (const auto& [fi, sb] : sj_before) {
            if (quad_sj(fi) < 0.0f && sb >= 0.0f) { new_inv = true; break; }
        }
        if (new_inv) {
            for (size_t i = 0; i < best.mids.size(); ++i) out_quads.positions[best.mids[i]] = base_pos[best.mids[i]];
            ++r_new_inv;
            continue;
        }
        // telemetry: scope quality after the move - only reachable with NO new inverse
        float scope_sj_after = 1.0f;
        for (uint32_t fi : scope)
            if (out_quads.polygons[fi].size() == 4) scope_sj_after = std::min(scope_sj_after, quad_sj(fi));
        if (scope_sj_after < scope_sj_before - 1e-6f) ++sj_scope_dropped;

        // gate 2: the move must not raise the pleat count inside the scope
        if (count_pleats(scope) > folds_before) {
            for (size_t i = 0; i < best.mids.size(); ++i) out_quads.positions[best.mids[i]] = base_pos[best.mids[i]];
            ++r_folds_up;
            continue;
        }

        // accepted: positions stay - greedy sequence, the next record sees this state
        moved_verts += best.mids.size();
        if (is_og) ++og_accepted; else ++nonog_accepted;
    }

    const size_t pleats_after_total = count_pleats(all_faces);
    std::cout << "[stage5/t17/probe] pleats_total_before=" << pleats_before_total
              << " after=" << pleats_after_total << std::endl;
    std::cout << "[stage5/t17/probe] trials=" << trials
              << " og=" << og
              << " og_accepted=" << og_accepted
              << " (" << (og ? 100.0 * static_cast<double>(og_accepted) /
                               static_cast<double>(og) : 0.0) << "%)"
              << " nonog_accepted=" << nonog_accepted
              << " rej(no_dof=" << r_no_dof << " travel=" << r_travel
              << " pleat_remains=" << r_pleat_remains << " new_inv=" << r_new_inv
              << " folds_up=" << r_folds_up << " stale=" << r_stale << ")"
              << " theta_reduced=" << theta_reduced
              << " sj_scope_dropped=" << sj_scope_dropped
              << " moved_verts=" << moved_verts
              << std::endl;
}

QrmStatus Stage5ParamAndExtract::execute(
    RemeshContext& ctx,
    PatchGraph& graph,
    QuadMeshResult& out_quads
) {
    if (!ctx.notify_progress(0.70f, "Stage 5: Integer Parametrization & Quad Assembly")) {
        return QRM_ERR_CANCELLED;
    }

    // Count calibration: plan -> sample -> extract -> measure. The planner estimates the
    // polygon count from path divisions, but corner snapping and the parity fix-up add a
    // few percent on top, so one correction pass keeps the actual output within ~5% of
    // the requested target.
    double search_eff = -1.0;
    double eff_used = 0.0;
    const double eff_goal = static_cast<double>(ctx.params->target_quad_count) * 0.92 *
        ((ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) ? 0.5 : 1.0);
    double prev_want = -1.0;
    int64_t prev_actual = -1;
    size_t empty_loops = 0, short_loops = 0, loop_paths_total = 0, quads_from_loops = 0;
    int64_t polys_before_weld = 0, verts_before_weld = 0;

    // Survive the calibration passes: the last pass is the shipped mesh, and the T16 re-lay runs
    // after the loop, so both the face->patch attribution and the candidate list live out here.
    std::vector<uint32_t> face_patch;
    std::vector<uint32_t> t16_candidates;
    // T17 (paired seam probe): cross-pleat records and per-patch overlap/gap must survive the
    // calibration passes for the same reason as face_patch - the last pass is the shipped mesh.
    std::vector<float> t17_ov, t17_gp;
    std::vector<T17CrossPleat> t17_cross;

    for (int calib = 0; calib < 3; ++calib) {
    balance_and_quantize_patch_sizes(ctx, graph, search_eff);
    eff_used = (search_eff > 0.0)
        ? search_eff
        : static_cast<double>(ctx.params->target_quad_count) * 0.92 *
          ((ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) ? 0.5 : 1.0);

    out_quads.positions.clear();
    out_quads.polygons.clear();
    out_quads.material_ids.clear();
    out_quads.sharp_edges.clear();
    out_quads.crease_values.clear();
    out_quads.seam_edges.clear();

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

    // ---- S27: cross-side correspondence imbalance (the REAL skew source) --------------
    // For each patch, opposite grid sides (0<->2 and 1<->3) must have proportional arc
    // length so the i-th border node on one side FACES the i-th on the other. When the two
    // sides differ in length but share the same division count (parity forces equal counts),
    // the i<->i pairing slants every cell -> SJ<0.5 tail. Path spacing is already uniform
    // (mean_cv 0.067), so THIS imbalance, not resampling, is what caps SJ.
    {
        double sum_ratio = 0.0; int np = 0; int bad = 0, verybad = 0;
        for (size_t p = 0; p < graph.patches.size(); ++p) {
            const auto& patch = graph.patches[p];
            size_t M = patch.boundary_loop.size();
            if (M < 4) continue;
            size_t i0 = patch.corner_indices[0], i1 = patch.corner_indices[1];
            size_t i2 = patch.corner_indices[2], i3 = patch.corner_indices[3];
            auto side_len = [&](size_t a, size_t b) {
                double L = 0.0;
                for (size_t k = a; k < b; ++k)
                    L += graph.edge_paths[patch.boundary_loop[k].edge_path_id].continuous_len;
                return L;
            };
            double L0 = side_len(i0, i1), L2 = side_len(i2, i3);
            double L1 = side_len(i1, i2), L3 = side_len(i3, M) + side_len(0, i0);
            (void)L3;
            double r = 1.0;
            if (L0 > 1e-9 && L2 > 1e-9) r = std::max(r, std::max(L0, L2) / std::min(L0, L2));
            if (L1 > 1e-9 && L3 > 1e-9) r = std::max(r, std::max(L1, L3) / std::min(L1, L3));
            sum_ratio += r; ++np;
            if (r > 1.5) bad++;
            if (r > 2.0) verybad++;
        }
        std::cout << "[stage5/sides] patches=" << np
                  << " mean_lenratio=" << (np ? sum_ratio / np : 0.0)
                  << " ratio>1.5=" << bad << " ratio>2=" << verybad << std::endl;
    }

    // 2. Canonical arc-length sampling for every path in graph.edge_paths
    for (auto& ep : graph.edge_paths) {
        int M = ep.integer_divisions;
        if (M <= 0) {
            // Shorter than half a cell: this path contributes NO samples at all. Both
            // neighbours of the path see the same zero, so the seam stays identical, and
            // together with the micro-path collapse in stage4 this is what finally breaks
            // N >= #paths per patch loop (the source of the polygon explosion).
            ep.out_sample_vertices.clear();
            continue;
        }
        ep.out_sample_vertices.resize(M + 1);
        ep.out_sample_vertices[0] = junc_to_out[ep.node_start];
        ep.out_sample_vertices[M] = junc_to_out[ep.node_end];

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

            Vec3f cp, cn;
            float d2;
            if (ctx.original_bvh.closest_point(pos, cp, cn, d2)) {
                if (d2 <= 0.25f * ctx.base_target_length * ctx.base_target_length) {
                    pos = cp;
                }
            }

            uint32_t out_idx = static_cast<uint32_t>(out_quads.positions.size());
            out_quads.positions.push_back(pos);
            ep.out_sample_vertices[s] = out_idx;
        }

        if (ep.is_sharp && ctx.params->preserve_sharp_edges) {
            for (int s = 0; s < M; ++s) {
                out_quads.sharp_edges.emplace_back(ep.out_sample_vertices[s], ep.out_sample_vertices[s + 1]);
                out_quads.crease_values.push_back(1.0f);
            }
        }
        if (ep.is_seam && ctx.params->preserve_uv_seams) {
            for (int s = 0; s < M; ++s) {
                out_quads.seam_edges.emplace_back(ep.out_sample_vertices[s], ep.out_sample_vertices[s + 1]);
            }
        }
    }

    // ---- S27 measure-first: per-path step uniformity AFTER resampling -----------------
    // CV = stddev / mean of consecutive sample-segment lengths along each path. If it is
    // already ~0 the path is equidistant and the 37% skewed cells come from OPPOSITE-SIDE
    // correspondence (L_side0 != L_side2 at equal division count -> slanted grid lines),
    // which no single-path resampling can fix. Measure before choosing the fix.
    {
        double sum_cv = 0.0; int npath = 0;
        double worst_cv = 0.0; uint32_t worst_id = 0; int worst_m = 0;
        int cv_gt_20 = 0, cv_gt_40 = 0;
        for (size_t pi = 0; pi < graph.edge_paths.size(); ++pi) {
            const auto& ep = graph.edge_paths[pi];
            const auto& sv = ep.out_sample_vertices;
            if (sv.size() < 3) continue;
            double sum = 0.0, ss = 0.0; int cnt = 0;
            for (size_t s = 0; s + 1 < sv.size(); ++s) {
                float dl = (out_quads.positions[sv[s + 1]] - out_quads.positions[sv[s]]).norm();
                sum += dl; ss += (double)dl * dl; ++cnt;
            }
            if (cnt < 2) continue;
            double mean = sum / cnt;
            if (mean <= 1e-9) continue;
            double var = ss / cnt - mean * mean;
            double cv = (var > 0 ? std::sqrt(var) : 0.0) / mean;
            sum_cv += cv; ++npath;
            if (cv > 0.20) cv_gt_20++;
            if (cv > 0.40) cv_gt_40++;
            if (cv > worst_cv) { worst_cv = cv; worst_id = (uint32_t)pi; worst_m = cnt; }
        }
        std::cout << "[stage5/pathcv] paths=" << npath
                  << " mean_cv=" << (npath ? sum_cv / npath : 0.0)
                  << " cv>20%=" << cv_gt_20 << " cv>40%=" << cv_gt_40
                  << " worst_cv=" << worst_cv << " (path" << worst_id
                  << " segs=" << worst_m << ")" << std::endl;
    }

    // 3. Extract quads for each patch
    g_extract_stats.reset();
    empty_loops = 0; short_loops = 0; loop_paths_total = 0; quads_from_loops = 0;
    for (const auto& patch : graph.patches) {
        if (patch.boundary_loop.empty()) empty_loops++;
        else if (patch.boundary_loop.size() < 4) short_loops++;
        loop_paths_total += patch.boundary_loop.size();
    }

    // Diagnostic for S45 (where do the folds come from): face -> patch attribution, so a pleat
    // edge can be classified as INTRA-patch (the fill itself folds) or CROSS-patch (a seam).
    face_patch.clear();
    for (size_t p = 0; p < graph.patches.size(); ++p) {
        const auto& patch = graph.patches[p];
        size_t before = out_quads.polygons.size();
        extract_patch_quads(ctx, patch, graph, out_quads);
        quads_from_loops += out_quads.polygons.size() - before;
        for (size_t f = before; f < out_quads.polygons.size(); ++f) face_patch.push_back(static_cast<uint32_t>(p));
    }

    // Winding-independent pleat census: two faces on the SAME side of their shared edge with a
    // normal angle > 120 deg are folded onto each other (see test_out/pleat_count.py).
    // NOTE: stage5 runs more than once (the budget calibration pass), so this line repeats -
    // the LAST occurrence is the shipped mesh. Kept unconditional because execute() has no
    // knowledge of which pass it is (the override flag lives in balance_and_quantize_patch_sizes).
    {
        t17_cross.clear();   // rebuilt every calibration pass, like face_patch
        struct EdgeRec { uint32_t f0 = INVALID_INDEX, f1 = INVALID_INDEX;
                         uint32_t w0 = INVALID_INDEX, w1 = INVALID_INDEX; };
        std::unordered_map<uint64_t, EdgeRec> emap;
        auto key2 = [](uint32_t a, uint32_t b) -> uint64_t {
            return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
        };
        const auto& POS = out_quads.positions;
        for (size_t fi = 0; fi < out_quads.polygons.size(); ++fi) {
            const auto& p = out_quads.polygons[fi];
            const size_t pn = p.size();
            if (pn < 3) continue;
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = p[i], b = p[(i + 1) % pn];
                if (a == b) continue;
                uint32_t free_v = INVALID_INDEX;
                for (size_t k = 0; k < pn; ++k) {
                    if (p[k] != a && p[k] != b) { free_v = p[k]; break; }
                }
                uint64_t k2 = key2(a, b);
                auto it = emap.find(k2);
                if (it == emap.end()) {
                    EdgeRec r; r.f0 = static_cast<uint32_t>(fi); r.w0 = free_v;
                    emap.emplace(k2, r);
                } else if (it->second.f1 == INVALID_INDEX) {
                    it->second.f1 = static_cast<uint32_t>(fi);
                    it->second.w1 = free_v;
                }
            }
        }
        auto newell = [&](size_t fi) -> Vec3f {
            const auto& p = out_quads.polygons[fi];
            Vec3d n = Vec3d::Zero();
            const size_t pn = p.size();
            for (size_t i = 0; i < pn; ++i) {
                Vec3d a = POS[p[i]].cast<double>();
                Vec3d b = POS[p[(i + 1) % pn]].cast<double>();
                n += a.cross(b);
            }
            double ln = n.norm();
            return ln > 1e-14 ? (n / ln).cast<float>() : Vec3f(0, 0, 0);
        };
        std::vector<Vec3f> fn(out_quads.polygons.size());
        for (size_t fi = 0; fi < fn.size(); ++fi) fn[fi] = newell(fi);

        size_t pleat = 0, pleat_intra = 0, pleat_cross = 0, folded_faces = 0;
        std::unordered_map<uint32_t, uint32_t> per_patch;
        for (const auto& [k2, r] : emap) {
            if (r.f0 == INVALID_INDEX || r.f1 == INVALID_INDEX) continue;
            if (r.w0 == INVALID_INDEX || r.w1 == INVALID_INDEX) continue;
            double dot = std::max(-1.0, std::min(1.0, static_cast<double>(fn[r.f0].dot(fn[r.f1]))));
            float dih = static_cast<float>(std::acos(dot) * 180.0 / PI_D);
            if (dih <= 120.0f) continue;
            uint32_t a = static_cast<uint32_t>(k2 >> 32), b = static_cast<uint32_t>(k2 & 0xFFFFFFFFu);
            Vec3f e = POS[b] - POS[a];
            float el = e.norm();
            if (el < 1e-9f) continue;
            e = e / el;
            Vec3f mid = 0.5f * (POS[a] + POS[b]);
            Vec3f u0 = POS[r.w0] - mid, u1 = POS[r.w1] - mid;
            u0 -= u0.dot(e) * e;
            u1 -= u1.dot(e) * e;
            if (u0.dot(u1) <= 0.0f) continue;      // opposite sides = flat continuation
            pleat++;
            const uint32_t pa = (r.f0 < face_patch.size()) ? face_patch[r.f0] : INVALID_INDEX;
            const uint32_t pb = (r.f1 < face_patch.size()) ? face_patch[r.f1] : INVALID_INDEX;
            if (pa == pb) { pleat_intra++; if (pa != INVALID_INDEX) per_patch[pa]++; }
            else {
                pleat_cross++;
                // T17: keep the seam pleats with a valid two-patch attribution - those are the
                // ones a single-patch re-lay (T16) can never touch (it moves no seam vertex).
                if (pa != INVALID_INDEX && pb != INVALID_INDEX)
                    t17_cross.push_back(T17CrossPleat{r.f0, r.f1, a, b, pa, pb});
            }
            folded_faces += 2;
        }
        size_t worst_n = 0; uint32_t worst_p = INVALID_INDEX; double sum_sq = 0.0;
        for (const auto& [p, c] : per_patch) { sum_sq += static_cast<double>(c) * c; if (c > worst_n) { worst_n = c; worst_p = p; } }
        std::cout << "[stage5/pleatmap] pleats=" << pleat
                  << " intra_patch=" << pleat_intra << " cross_patch=" << pleat_cross
                  << " patches_with_pleats=" << per_patch.size()
                  << "/" << graph.patches.size()
                  << " worst_patch=" << worst_p << "(" << worst_n << ")"
                  << " giniish_sum_sq=" << sum_sq << std::endl;

        // ---- T16 premise test: does a patch that folds also OVERLAP ITSELF? ------------------
        // A fold pair lies on the same side of its shared edge, so if the patch grid were merely
        // "wavy" the cells would still tile the patch's own border region. If instead
        // sum(quad areas) > area(hull of the patch's own vertices) the grid covers part of its
        // border twice and leaves another part uncovered - which is exactly what a monotone
        // re-fill can fix (and what no vertex smoothing can, because the vertices are already
        // on the surface). Measured per patch, correlated with the pleat census above.
        size_t overlap_patches = 0, pleated_overlap = 0, pleated_clean = 0;
        t16_candidates.clear();
        // T17: keep the measured per-patch overlap/gap for the paired probe. Patches with
        // inside_cells < 8 are never rasterized and stay 0 (= no measured slack).
        t17_ov.assign(graph.patches.size(), 0.0f);
        t17_gp.assign(graph.patches.size(), 0.0f);
        double worst_ratio = 0.0; uint32_t worst_ratio_patch = INVALID_INDEX;
        std::vector<std::vector<uint32_t>> faces_of_patch(graph.patches.size());
        for (size_t fi = 0; fi < face_patch.size(); ++fi)
            if (face_patch[fi] < faces_of_patch.size()) faces_of_patch[face_patch[fi]].push_back(static_cast<uint32_t>(fi));
        for (size_t p = 0; p < graph.patches.size(); ++p) {
            const std::vector<uint32_t>& mine = faces_of_patch[p];
            if (mine.size() < 2) continue;
            // PCA plane of the patch's own vertices
            Vec3d cen = Vec3d::Zero();
            std::vector<uint32_t> verts;
            { std::unordered_set<uint32_t> s;
              for (uint32_t fi : mine) for (uint32_t v : out_quads.polygons[fi]) s.insert(v);
              verts.assign(s.begin(), s.end());
              std::sort(verts.begin(), verts.end());
              for (uint32_t v : verts) cen += POS[v].cast<double>();
              cen /= static_cast<double>(verts.size()); }
            Mat3d cov = Mat3d::Zero();
            for (uint32_t v : verts) { Vec3d d = POS[v].cast<double>() - cen; cov += d * d.transpose(); }
            Eigen::SelfAdjointEigenSolver<Mat3d> es(cov);
            Vec3d ax1 = es.eigenvectors().col(2), ax2 = es.eigenvectors().col(1);
            auto proj = [&](uint32_t v) -> std::pair<double, double> {
                Vec3d d = POS[v].cast<double>() - cen;
                return { d.dot(ax1), d.dot(ax2) };
            };
            double sum_area = 0.0;
            for (uint32_t fi : mine) {
                const auto& q = out_quads.polygons[fi];
                double a = 0.0;
                for (size_t i = 1; i + 1 < q.size(); ++i) {
                    auto A = proj(q[0]), B = proj(q[i]), D = proj(q[i + 1]);
                    a += 0.5 * std::abs((B.first - A.first) * (D.second - A.second) -
                                        (D.first - A.first) * (B.second - A.second));
                }
                sum_area += a;
            }
            // convex hull area (Andrew monotone chain) of the patch's projected vertices
            using P2 = std::pair<double, double>;
            std::vector<P2> pts;
            pts.reserve(verts.size());
            for (uint32_t v : verts) pts.push_back(proj(v));
            std::sort(pts.begin(), pts.end());
            auto cross2 = [](const P2& o, const P2& a, const P2& b) {
                return (a.first - o.first) * (b.second - o.second) -
                       (a.second - o.second) * (b.first - o.first);
            };
            std::vector<P2> lower, upper;
            for (const P2& q : pts) {
                while (lower.size() >= 2 && cross2(lower[lower.size() - 2], lower.back(), q) <= 0.0)
                    lower.pop_back();
                lower.push_back(q);
            }
            for (size_t i = pts.size(); i-- > 0;) {
                const P2& q = pts[i];
                while (upper.size() >= 2 && cross2(upper[upper.size() - 2], upper.back(), q) <= 0.0)
                    upper.pop_back();
                upper.push_back(q);
            }
            std::vector<P2> hull;
            hull.insert(hull.end(), lower.begin(), lower.end());
            hull.insert(hull.end(), upper.begin() + 1, upper.end() - 1);
            double hull_area = 0.0;
            for (size_t i = 0; i + 2 < hull.size(); ++i) {
                hull_area += 0.5 * std::abs((hull[i + 1].first - hull[0].first) * (hull[i + 2].second - hull[0].second) -
                                            (hull[i + 2].first - hull[0].first) * (hull[i + 1].second - hull[0].second));
            }
            if (hull_area <= 1e-12) continue;
            const double ratio = sum_area / hull_area;

            // Raster census (the sharp version of the ratio test above): a fold that overlaps a
            // neighbour can be exactly compensated by an equal uncovered gap, so sum==hull proves
            // nothing. Rasterize the patch's own quads on a 2D grid in its PCA plane and count
            // cells covered 2+ times (overlap) and cells inside the border covered 0 times (gap).
            // monotone re-fill can only help if BOTH are present.
            double lo1 = 1e30, hi1 = -1e30, lo2 = 1e30, hi2 = -1e30;
            for (uint32_t v : verts) {
                auto q = proj(v);
                lo1 = std::min(lo1, q.first); hi1 = std::max(hi1, q.first);
                lo2 = std::min(lo2, q.second); hi2 = std::max(hi2, q.second);
            }
            const int RES = 40;
            const double sc1 = (hi1 - lo1) > 1e-12 ? static_cast<double>(RES) / (hi1 - lo1) : 1.0;
            const double sc2 = (hi2 - lo2) > 1e-12 ? static_cast<double>(RES) / (hi2 - lo2) : 1.0;
            std::vector<uint8_t> cover(RES * RES, 0);        // patch quads
            std::vector<uint8_t> inside(RES * RES, 0);       // border polygon (hull) fill
            auto fill_tri = [&](const P2& a, const P2& b, const P2& c, std::vector<uint8_t>& dst,
                                bool increment) {
                double mnx = std::min({a.first, b.first, c.first});
                double mxx = std::max({a.first, b.first, c.first});
                double mny = std::min({a.second, b.second, c.second});
                double mxy = std::max({a.second, b.second, c.second});
                const int x0 = std::max(0, static_cast<int>((mnx - lo1) * sc1));
                const int x1 = std::min(RES - 1, static_cast<int>((mxx - lo1) * sc1));
                const int y0 = std::max(0, static_cast<int>((mny - lo2) * sc2));
                const int y1 = std::min(RES - 1, static_cast<int>((mxy - lo2) * sc2));
                const double d = (b.second - c.second) * (a.first - c.first) +
                                 (c.first - b.first) * (a.second - c.second);
                if (std::abs(d) < 1e-18) return;
                for (int yy = y0; yy <= y1; ++yy)
                    for (int xx = x0; xx <= x1; ++xx) {
                        const double px = lo1 + (xx + 0.5) / sc1;
                        const double py = lo2 + (yy + 0.5) / sc2;
                        const double w0 = ((b.second - c.second) * (px - c.first) +
                                          (c.first - b.first) * (py - c.second)) / d;
                        const double w1 = ((c.second - a.second) * (px - c.first) +
                                          (a.first - c.first) * (py - c.second)) / d;
                        const double w2 = 1.0 - w0 - w1;
                        if (w0 >= -1e-9 && w1 >= -1e-9 && w2 >= -1e-9) {
                            int id = yy * RES + xx;
                            if (increment) { if (dst[id] < 255) dst[id]++; } else dst[id] = 1;
                        }
                    }
            };
            for (uint32_t fi : mine) {
                const auto& q = out_quads.polygons[fi];
                for (size_t i = 1; i + 1 < q.size(); ++i)
                    fill_tri(proj(q[0]), proj(q[i]), proj(q[i + 1]), cover, true);
            }
            for (size_t i = 0; i + 2 < hull.size(); ++i)
                fill_tri(hull[0], hull[i + 1], hull[i + 2], inside, false);
            long n_double = 0, n_gap = 0, n_ok = 0;
            for (int i = 0; i < RES * RES; ++i) {
                if (!inside[i]) continue;
                if (cover[i] == 0) n_gap++;
                else if (cover[i] >= 2) n_double++;
                else n_ok++;
            }
            const long inside_cells = n_gap + n_double + n_ok;
            if (inside_cells >= 8) {
                const double ov = static_cast<double>(n_double) / inside_cells;
                const double gp = static_cast<double>(n_gap) / inside_cells;
                t17_ov[p] = static_cast<float>(ov);
                t17_gp[p] = static_cast<float>(gp);
                if (ov > 0.03 && gp > 0.03) overlap_patches++;
                const bool pleated0 = per_patch.count(static_cast<uint32_t>(p)) > 0;
                if (pleated0) {
                    if (ov > 0.03 && gp > 0.03) { pleated_overlap++; t16_candidates.push_back(static_cast<uint32_t>(p)); }
                    else pleated_clean++;
                }
                if (ov + gp > worst_ratio) { worst_ratio = ov + gp; worst_ratio_patch = static_cast<uint32_t>(p); }
            }
            const bool overlapped = false;   // (superseded by the raster test above)
            (void)overlapped; (void)ratio;
        }
        std::cout << "[stage5/overlapmap] patches=" << graph.patches.size()
                  << " overlap_AND_gap_gt3%=" << overlap_patches
                  << " pleated&both=" << pleated_overlap
                  << " pleated&clean=" << pleated_clean
                  << " worst_overlap_plus_gap=" << worst_ratio << " (patch " << worst_ratio_patch << ")"
                  << std::endl;

        // T17 premise (invariant #16: measure first): of the seam pleats (pa != pb), how many
        // sit on pairs where BOTH patches hold >3% double coverage AND >3% gap inside their own
        // border - only those pairs have mesh slack for a monotone PAIRED re-lay (T16 showed
        // that a single patch with both measures can re-lay; the seam side needs the pair).
        {
            std::unordered_map<uint64_t, bool> pair_og;
            size_t pleats_in_og = 0, both_og_pairs = 0;
            auto og_patch = [&](uint32_t p) -> bool {
                return p < t17_ov.size() && t17_ov[p] > 0.03f && t17_gp[p] > 0.03f;
            };
            for (const auto& r : t17_cross) {
                const uint64_t pk = (static_cast<uint64_t>(std::min(r.pa, r.pb)) << 32) |
                                    static_cast<uint32_t>(std::max(r.pa, r.pb));
                auto it = pair_og.find(pk);
                if (it == pair_og.end()) {
                    it = pair_og.emplace(pk, og_patch(r.pa) && og_patch(r.pb)).first;
                    if (it->second) both_og_pairs++;
                }
                if (it->second) pleats_in_og++;
            }
            const size_t np = t17_cross.size();
            std::cout << "[stage5/t17/census] cross_pleats=" << np
                      << " pairs=" << pair_og.size()
                      << " both_og_pairs=" << both_og_pairs
                      << " pleats_in_both_og=" << pleats_in_og
                      << " (" << (np ? 100.0 * static_cast<double>(pleats_in_og) /
                                       static_cast<double>(np) : 0.0) << "%)"
                      << std::endl;
        }
    }

    polys_before_weld = static_cast<int64_t>(out_quads.polygons.size());
    verts_before_weld = static_cast<int64_t>(out_quads.positions.size());
    std::cout << "[stage5/extract] used_loop=" << g_extract_stats.used_loop
              << " discont_loops=" << g_loop_discont_count
              << " used_fallback=" << g_extract_stats.used_fallback
              << " skipped_tiny=" << g_extract_stats.skipped_tiny_loop
              << " sum_border=" << g_extract_stats.sum_border_verts
              << " sum_interior=" << g_extract_stats.sum_interior_verts
              << " sum_quads=" << g_extract_stats.sum_quads
              << " avg_nu=" << (graph.patches.empty() ? 0.0 : static_cast<double>(g_extract_stats.sum_nu) / graph.patches.size())
              << " avg_nv=" << (graph.patches.empty() ? 0.0 : static_cast<double>(g_extract_stats.sum_nv) / graph.patches.size())
              << " mismatch_patches=" << g_extract_stats.mismatch_patches
              << " coarse_min_dim_lt3=" << g_extract_stats.patches_min_dim_lt3
              << "/" << g_extract_stats.patches_counted
              << " (" << (g_extract_stats.patches_counted
                  ? 100.0 * static_cast<double>(g_extract_stats.patches_min_dim_lt3)
                        / static_cast<double>(g_extract_stats.patches_counted)
                  : 0.0) << "%)"
              << " degenerate_slots=" << g_extract_stats.degenerate_slots
              << std::endl;

        {
            int64_t actual = static_cast<int64_t>(out_quads.polygons.size());
            int64_t want = static_cast<int64_t>(eff_used);
            int64_t goal = static_cast<int64_t>(eff_goal);
            int64_t diff = actual - goal;
            if (diff < 0) diff = -diff;
            std::cout << "[stage5/calib] pass=" << calib << " actual=" << actual
                      << " want=" << want << " goal=" << goal << std::endl;

            if (calib >= 2 || goal <= 0) break;
            if (diff * 20 <= goal) break;                 // within 5% of the budget
            if (actual == prev_actual) break;             // density floor: no response

            // Newton-style correction with the measured local exponent
            //   actual ~ c * want^beta   =>   want' = want * (goal/actual)^(1/beta)
            double beta = 1.0;
            if (calib >= 1 && want != prev_want && prev_actual > 0) {
                beta = std::log(static_cast<double>(actual) / static_cast<double>(prev_actual)) /
                       std::log(static_cast<double>(want) / static_cast<double>(prev_want));
            }
            if (!(beta > 0.15) || beta > 3.0) beta = 1.0;

            double next = static_cast<double>(want) *
                std::pow(static_cast<double>(goal) / static_cast<double>(actual), 1.0 / beta);
            double lo = 0.35 * static_cast<double>(want);
            double hi = 2.50 * static_cast<double>(want);
            if (next < lo) next = lo;
            if (next > hi) next = hi;

            prev_want = static_cast<double>(want);
            prev_actual = actual;
            search_eff = next;
            continue;
        }
    } // for calib

    if (out_quads.polygons.empty()) {
        return QRM_ERR_PARAM_FAILED;
    }

    // T16 (BUG-01): monotone re-lay of the interior nodes of the patches whose own border region
    // is over- AND under-covered (the census above). Borders are Dirichlet, so seams and welding
    // are untouched. Default OFF: QREMESH_T16=0 disables, baseline stays bit-identical.
    if (const char* e = std::getenv("QREMESH_T16")) {
        if (std::atoi(e) != 0) {
            untangle_patch_grids(ctx, out_quads, face_patch, t16_candidates);
        }
    }

    // T17 (BUG-01, seam side): paired probe - rotate the free vertices of BOTH patches around
    // the shared edge of a cross-patch pleat, symmetric angles, seam loop untouched. Default
    // OFF: QREMESH_T17 unset/0 skips the call and the mesh stays byte-identical.
    if (const char* e = std::getenv("QREMESH_T17")) {
        if (std::atoi(e) != 0)
            t17_paired_probe(ctx, out_quads, face_patch, t17_cross, t17_ov, t17_gp);
    }

    // T15 (S45 claim "the weld doubles the folds"): measure the SAME mesh with BOTH pleat
    // definitions on both sides of the filter, so the 546-vs-1855 gap decomposes into a
    // definition gap and a filter effect instead of being quoted from two different metrics.
    ctx.dump_quads_obj("stage5_preweld", out_quads);
    {
        const PleatCounts pc = count_pleats_t15(out_quads.positions, out_quads.polygons);
        std::cout << "[stage5/t15] stage=pre_weld polys=" << out_quads.polygons.size()
                  << " pleats_gated=" << pc.gated << " pleats_sameside=" << pc.sameside
                  << " verts=" << out_quads.positions.size() << std::endl;
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
