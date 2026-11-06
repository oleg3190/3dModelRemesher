#include "stages/stage5_param.h"
#include <Eigen/Sparse>
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

    // 3. Binary search for optimal h matching target quad count
    std::vector<int> d_test(num_edges);
    auto evaluate_total_quads = [&](float h) -> int64_t {
        for (size_t e = 0; e < num_edges; ++e) {
            d_test[e] = std::clamp(static_cast<int>(std::round(path_metric_len[e] / h)), 0, 100);
        }
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
        d_int[e] = std::clamp(static_cast<int>(std::round(v)), 1, 100);
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
                if (d_int[e] + delta < 1 || d_int[e] + delta > 100) continue;
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
                        if (d_int[eid] + delta < 1 || d_int[eid] + delta > max_d) continue;

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
        const Vec3f& c0_pt = out_quads.positions[grid[i][0]];
        const Vec3f& c2_pt = out_quads.positions[grid[i][nv]];

        for (int j = 1; j < nv; ++j) {
            float v = static_cast<float>(j) / static_cast<float>(nv);
            const Vec3f& c3_pt = out_quads.positions[grid[0][j]];
            const Vec3f& c1_pt = out_quads.positions[grid[nu][j]];

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
    // QREMESH_HARMONIC_ITERS overrides the iteration count.
    int harmonic_iters = 16;
    if (const char* e = std::getenv("QREMESH_HARMONIC_ITERS")) {
        int v = std::atoi(e);
        if (v >= 0) harmonic_iters = v;
    }
    for (int it = 0; it < harmonic_iters; ++it) {
        for (int i = 1; i < nu; ++i) {
            for (int j = 1; j < nv; ++j) {
                Vec3f avg = out_quads.positions[grid[i - 1][j]] +
                            out_quads.positions[grid[i + 1][j]] +
                            out_quads.positions[grid[i][j - 1]] +
                            out_quads.positions[grid[i][j + 1]];
                avg *= 0.25f;
                Vec3f p = avg;            // weight 1: full step to the neighbour mean
                Vec3f cp, cn;
                float d2;
                if (ctx.original_bvh.closest_point(p, cp, cn, d2)) p = cp;  // snap (no shrink)
                out_quads.positions[grid[i][j]] = p;   // Gauss-Seidel: apply immediately
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

    // Filter duplicate quads and zero-area degenerates (vertices are already topologically shared)
    std::vector<std::vector<uint32_t>> valid_polys;
    std::vector<uint16_t> valid_mats;
    valid_polys.reserve(out_quads.polygons.size());
    valid_mats.reserve(out_quads.material_ids.size());
    std::unordered_set<uint64_t> seen_quad_keys;

    auto make_quad_key = [](uint32_t a, uint32_t b, uint32_t c, uint32_t d) -> uint64_t {
        uint32_t arr[4] = { a, b, c, d };
        std::sort(arr, arr + 4);
        return (static_cast<uint64_t>(arr[0]) * 73856093) ^
               (static_cast<uint64_t>(arr[1]) * 19349663) ^
               (static_cast<uint64_t>(arr[2]) * 83492791) ^
               (static_cast<uint64_t>(arr[3]) * 31415926);
    };

    for (size_t p_idx = 0; p_idx < out_quads.polygons.size(); ++p_idx) {
        auto& poly = out_quads.polygons[p_idx];

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
                if (cleaned.size() == 4) {
                    uint64_t qk = make_quad_key(cleaned[0], cleaned[1], cleaned[2], cleaned[3]);
                    if (seen_quad_keys.insert(qk).second) {
                        valid_polys.push_back(std::move(cleaned));
                        valid_mats.push_back(p_idx < out_quads.material_ids.size() ? out_quads.material_ids[p_idx] : 0);
                    }
                } else {
                    valid_polys.push_back(std::move(cleaned));
                    valid_mats.push_back(p_idx < out_quads.material_ids.size() ? out_quads.material_ids[p_idx] : 0);
                }
            }
        }
    }

    out_quads.polygons = std::move(valid_polys);
    out_quads.material_ids = std::move(valid_mats);
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

    for (const auto& patch : graph.patches) {
        size_t before = out_quads.polygons.size();
        extract_patch_quads(ctx, patch, graph, out_quads);
        quads_from_loops += out_quads.polygons.size() - before;
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
