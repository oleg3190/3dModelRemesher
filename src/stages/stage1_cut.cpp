#include "stages/stage1_cut.h"
#include <cmath>
#include <vector>
#include <queue>
#include <unordered_set>

namespace qremesh {

void Stage1FeatureCut::detect_sharp_dihedral_edges(HalfEdgeMesh& mesh, float threshold_deg) {
    float threshold_rad = threshold_deg * (PI_F / 180.0f);
    float cos_threshold = std::cos(threshold_rad);

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.is_boundary_edge(static_cast<uint32_t>(e))) {
            mesh.edges[e].is_sharp = true;
            continue;
        }

        uint32_t h0 = mesh.edges[e].halfedge;
        uint32_t h1 = mesh.halfedges[h0].twin;
        if (h1 == INVALID_INDEX) continue;

        uint32_t f0 = mesh.halfedges[h0].face;
        uint32_t f1 = mesh.halfedges[h1].face;
        if (!mesh.is_valid_face(f0) || !mesh.is_valid_face(f1)) continue;

        Vec3f n0 = mesh.compute_face_normal(f0);
        Vec3f n1 = mesh.compute_face_normal(f1);

        float dot_val = clampf(n0.dot(n1), -1.0f, 1.0f);
        if (dot_val < cos_threshold) {
            mesh.edges[e].is_sharp = true;
            mesh.edges[e].crease = 1.0f;
            uint32_t u = mesh.from_vertex(h0);
            uint32_t v = mesh.halfedges[h0].to_vertex;
            mesh.vertices[u].is_sharp = true;
            mesh.vertices[v].is_sharp = true;
        }
    }
}

void Stage1FeatureCut::mark_material_boundaries(HalfEdgeMesh& mesh) {
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.is_boundary_edge(static_cast<uint32_t>(e))) continue;

        uint32_t h0 = mesh.edges[e].halfedge;
        uint32_t h1 = mesh.halfedges[h0].twin;
        if (h1 == INVALID_INDEX) continue;

        uint32_t f0 = mesh.halfedges[h0].face;
        uint32_t f1 = mesh.halfedges[h1].face;
        if (!mesh.is_valid_face(f0) || !mesh.is_valid_face(f1)) continue;

        if (mesh.faces[f0].material_id != mesh.faces[f1].material_id) {
            mesh.edges[e].is_sharp = true;
            mesh.edges[e].is_seam = true;
            uint32_t u = mesh.from_vertex(h0);
            uint32_t v = mesh.halfedges[h0].to_vertex;
            mesh.vertices[u].is_sharp = true;
            mesh.vertices[v].is_sharp = true;
            mesh.vertices[u].is_seam = true;
            mesh.vertices[v].is_seam = true;
        }
    }
}

void Stage1FeatureCut::project_and_embed_curves(HalfEdgeMesh& mesh, const RemeshContext& ctx) {
    if (!ctx.in_mesh->user_curves || ctx.in_mesh->num_curves == 0) return;

    BVHTree working_bvh;
    if (!working_bvh.build_from_halfedge(mesh)) return;

    for (uint32_t c_idx = 0; c_idx < ctx.in_mesh->num_curves; ++c_idx) {
        const QrmCurve& curve = ctx.in_mesh->user_curves[c_idx];
        if (curve.point_count < 2 || !curve.points) continue;
        if (curve.role == QRM_CURVE_FLOW_GUIDE) continue; // Flow guides handled in Stage 3

        std::vector<uint32_t> mapped_vertices;
        mapped_vertices.reserve(curve.point_count);

        for (uint32_t p = 0; p < curve.point_count; ++p) {
            Vec3f pt(curve.points[3 * p + 0], curve.points[3 * p + 1], curve.points[3 * p + 2]);
            Vec3f closest_pt, n;
            float dist_sq;
            uint32_t hit_tri = INVALID_INDEX;
            if (working_bvh.closest_point(pt, closest_pt, n, dist_sq, &hit_tri) && hit_tri != INVALID_INDEX) {
                // Find nearest vertex among the 3 vertices of hit_tri
                uint32_t best_v = INVALID_INDEX;
                float best_d2 = std::numeric_limits<float>::infinity();

                for (uint32_t v : mesh.get_face_vertices(hit_tri)) {
                    if (mesh.vertices[v].is_deleted) continue;
                    float d2 = (mesh.vertices[v].pos - closest_pt).squaredNorm();
                    if (d2 < best_d2) {
                        best_d2 = d2;
                        best_v = v;
                    }
                }

                if (best_v != INVALID_INDEX) {
                    if (mapped_vertices.empty() || mapped_vertices.back() != best_v) {
                        mapped_vertices.push_back(best_v);
                    }
                }
            }
        }

        // Connect path on mesh between sequential mapped vertices via A*
        for (size_t i = 0; i + 1 < mapped_vertices.size(); ++i) {
            uint32_t u = mapped_vertices[i];
            uint32_t v = mapped_vertices[i + 1];
            if (u == v) continue;

            const Vec3f& target_pos = mesh.vertices[v].pos;
            float direct_dist = (mesh.vertices[u].pos - target_pos).norm();
            float max_search_radius = std::max(3.0f * direct_dist, 5.0f * ctx.base_target_length);

            std::unordered_map<uint32_t, float> g_score;
            std::unordered_map<uint32_t, uint32_t> parent;
            using PairFScoreV = std::pair<float, uint32_t>;
            std::priority_queue<PairFScoreV, std::vector<PairFScoreV>, std::greater<PairFScoreV>> open_set;

            g_score[u] = 0.0f;
            open_set.push({direct_dist, u});

            bool reached = false;
            while (!open_set.empty()) {
                auto [f, curr] = open_set.top();
                open_set.pop();

                if (curr == v) {
                    reached = true;
                    break;
                }

                float curr_g = g_score[curr];
                float h_curr = (mesh.vertices[curr].pos - target_pos).norm();
                if (curr_g + h_curr > max_search_radius) continue;

                for (uint32_t nbr : mesh.get_vertex_neighbors(curr)) {
                    float step = (mesh.vertices[curr].pos - mesh.vertices[nbr].pos).norm();
                    float tentative_g = curr_g + step;
                    if (!g_score.count(nbr) || tentative_g < g_score[nbr]) {
                        g_score[nbr] = tentative_g;
                        parent[nbr] = curr;
                        float h_nbr = (mesh.vertices[nbr].pos - target_pos).norm();
                        open_set.push({tentative_g + h_nbr, nbr});
                    }
                }
            }

            if (reached) {
                uint32_t step_v = v;
                while (step_v != u) {
                    uint32_t p_v = parent[step_v];
                    uint32_t e = mesh.find_edge(p_v, step_v);
                    if (e != INVALID_INDEX) {
                        if (curve.role == QRM_CURVE_HARD_LOOP || curve.role == QRM_CURVE_QUAD_STRIP) {
                            mesh.edges[e].is_sharp = true;
                            mesh.edges[e].crease = 1.0f;
                            mesh.vertices[p_v].is_sharp = true;
                            mesh.vertices[step_v].is_sharp = true;
                        }
                        if (curve.role == QRM_CURVE_SEAM_SPLIT) {
                            mesh.edges[e].is_seam = true;
                            mesh.vertices[p_v].is_seam = true;
                            mesh.vertices[step_v].is_seam = true;
                        }
                    }
                    step_v = p_v;
                }
            }
        }
    }
}

void Stage1FeatureCut::filter_micro_features(HalfEdgeMesh& mesh, float min_feature_len) {
    // Find sharp vertices degree in sharp edge subgraph
    std::vector<int> sharp_deg(mesh.vertices.size(), 0);
    std::vector<std::vector<uint32_t>> sharp_adj(mesh.vertices.size());

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.edges[e].is_sharp) {
            uint32_t h = mesh.edges[e].halfedge;
            uint32_t u = mesh.from_vertex(h);
            uint32_t v = mesh.halfedges[h].to_vertex;
            sharp_deg[u]++;
            sharp_deg[v]++;
            sharp_adj[u].push_back(static_cast<uint32_t>(e));
            sharp_adj[v].push_back(static_cast<uint32_t>(e));
        }
    }

    // Mark corner vertices (sharp valences >= 3)
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (sharp_deg[v] >= 3) {
            mesh.vertices[v].is_corner = true;
        }
    }

    // Prune branches starting at degree 1 (dead ends) shorter than min_feature_len
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (sharp_deg[v] == 1) {
            // Walk branch
            uint32_t curr_v = static_cast<uint32_t>(v);
            uint32_t prev_e = INVALID_INDEX;
            float branch_len = 0.0f;
            std::vector<uint32_t> branch_edges;

            while (curr_v != INVALID_INDEX && sharp_deg[curr_v] <= 2) {
                uint32_t next_e = INVALID_INDEX;
                for (uint32_t e : sharp_adj[curr_v]) {
                    if (e != prev_e && mesh.edges[e].is_sharp) {
                        next_e = e;
                        break;
                    }
                }
                if (next_e == INVALID_INDEX) break;

                branch_edges.push_back(next_e);
                branch_len += mesh.compute_edge_length(next_e);

                uint32_t h = mesh.edges[next_e].halfedge;
                uint32_t u = mesh.from_vertex(h);
                uint32_t w = mesh.halfedges[h].to_vertex;
                uint32_t next_v = (u == curr_v) ? w : u;

                prev_e = next_e;
                curr_v = next_v;

                if (sharp_deg[curr_v] >= 3) break; // Junction reached
            }

            if (branch_len < min_feature_len) {
                for (uint32_t e : branch_edges) {
                    mesh.edges[e].is_sharp = false;
                    mesh.edges[e].crease = 0.0f;
                }
            }
        }
    }
}

QrmStatus Stage1FeatureCut::execute(RemeshContext& ctx) {
    if (!ctx.notify_progress(0.15f, "Stage 1: Feature Cut & Boundary Embedding")) {
        return QRM_ERR_CANCELLED;
    }

    // 1. Detect auto-sharp edges
    if (ctx.params->preserve_sharp_edges) {
        detect_sharp_dihedral_edges(ctx.working_mesh, ctx.params->sharp_angle_threshold);
    }

    // 2. Mark material boundaries
    if (ctx.params->preserve_material_borders) {
        mark_material_boundaries(ctx.working_mesh);
    }

    // 3. Project and embed user curves
    project_and_embed_curves(ctx.working_mesh, ctx);

    // 4. Filter micro-features
    filter_micro_features(ctx.working_mesh, 0.5f * ctx.base_target_length);

    ctx.dump_stage_obj("stage1_cut", ctx.working_mesh);
    return QRM_OK;
}

} // namespace qremesh
