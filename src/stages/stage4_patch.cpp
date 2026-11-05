#include "stages/stage4_patch.h"
#include <queue>
#include <set>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace qremesh {

void Stage4PatchGraph::trace_separatrices(
    const RemeshContext& ctx,
    std::vector<std::vector<uint32_t>>& separatrices
) {
    const auto& mesh = ctx.working_mesh;
    std::unordered_set<uint32_t> trace_vertices;

    // Sources: singularities and feature corners
    std::vector<uint32_t> seed_vertices;
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_deleted) continue;
        if (ctx.vertex_singularities[v] != 0 || mesh.vertices[v].is_corner) {
            seed_vertices.push_back(static_cast<uint32_t>(v));
        }
    }

    // Also include feature endpoints / sharp junctions
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_sharp && mesh.is_boundary_vertex(static_cast<uint32_t>(v))) {
            seed_vertices.push_back(static_cast<uint32_t>(v));
        }
    }

    std::unordered_set<uint32_t> visited_edges;

    for (uint32_t seed : seed_vertices) {
        auto v_faces = mesh.get_vertex_faces(seed);
        if (v_faces.empty()) continue;

        // Trace in 4 orthogonal directions
        Vec3f d1 = ctx.cross_field_dir1[v_faces[0]];
        Vec3f d2 = ctx.cross_field_dir2[v_faces[0]];
        std::array<Vec3f, 4> dirs = { d1, d2, -d1, -d2 };

        for (int dir_idx = 0; dir_idx < 4; ++dir_idx) {
            std::vector<uint32_t> path;
            path.push_back(seed);

            uint32_t curr_v = seed;
            Vec3f cur_dir = dirs[dir_idx];

            constexpr int MAX_STEPS = 2000;
            for (int step = 0; step < MAX_STEPS; ++step) {
                // Find next neighbor best aligned with cur_dir
                auto nbrs = mesh.get_vertex_neighbors(curr_v);
                if (nbrs.empty()) break;

                uint32_t best_next = INVALID_INDEX;
                float best_dot = -1.0f;

                for (uint32_t nbr : nbrs) {
                    if (path.size() > 1 && nbr == path[path.size() - 2]) continue; // No back-tracking
                    Vec3f edge_dir = (mesh.vertices[nbr].pos - mesh.vertices[curr_v].pos).normalized();
                    float dot_val = edge_dir.dot(cur_dir);
                    if (dot_val > best_dot) {
                        best_dot = dot_val;
                        best_next = nbr;
                    }
                }

                if (best_next == INVALID_INDEX || best_dot < 0.2f) break;

                uint32_t e = mesh.find_edge(curr_v, best_next);
                if (e != INVALID_INDEX) {
                    if (visited_edges.count(e)) {
                        // Motorcycle collision! Stop at intersection
                        path.push_back(best_next);
                        break;
                    }
                    visited_edges.insert(e);
                }

                path.push_back(best_next);
                curr_v = best_next;

                // Hit a boundary, singularity, or corner -> terminate separatrix
                if (mesh.is_boundary_vertex(curr_v) ||
                    (curr_v != seed && (ctx.vertex_singularities[curr_v] != 0 || mesh.vertices[curr_v].is_corner))) {
                    break;
                }

                // Update cur_dir from local cross field
                auto curr_faces = mesh.get_vertex_faces(curr_v);
                if (!curr_faces.empty()) {
                    uint32_t f = curr_faces[0];
                    Vec3f fd1 = ctx.cross_field_dir1[f];
                    Vec3f fd2 = ctx.cross_field_dir2[f];
                    std::array<Vec3f, 4> f_dirs = { fd1, fd2, -fd1, -fd2 };

                    float max_d = -1.0f;
                    Vec3f chosen = cur_dir;
                    for (const auto& fd : f_dirs) {
                        float d = fd.dot(cur_dir);
                        if (d > max_d) {
                            max_d = d;
                            chosen = fd;
                        }
                    }
                    cur_dir = chosen;
                }
            }

            if (path.size() >= 2) {
                separatrices.push_back(std::move(path));
            }
        }
    }
}

void Stage4PatchGraph::enforce_closed_loops_on_cylinders(
    const RemeshContext& ctx,
    std::vector<std::vector<uint32_t>>& separatrices
) {
    if (!ctx.params->enforce_closed_loops) return;
    const auto& mesh = ctx.working_mesh;

    // Check separatrices that nearly close on themselves
    for (auto& path : separatrices) {
        if (path.size() < 4) continue;
        uint32_t start_v = path.front();
        uint32_t end_v = path.back();

        if (start_v != end_v) {
            float dist = (mesh.vertices[start_v].pos - mesh.vertices[end_v].pos).norm();
            float local_scale = std::max(ctx.sizing_field[start_v], ctx.sizing_field[end_v]);

            // If displacement is less than 1.5 * local_scale, force loop closure
            if (dist < 1.5f * local_scale) {
                path.push_back(start_v); // Close loop
            }
        }
    }
}

void Stage4PatchGraph::build_quasi_quad_patches(
    const RemeshContext& ctx,
    const std::vector<std::vector<uint32_t>>& separatrices,
    PatchGraph& out_graph
) {
    const auto& mesh = ctx.working_mesh;
    uint32_t nf = static_cast<uint32_t>(mesh.faces.size());

    // Mark boundary edges in mesh
    std::unordered_set<uint32_t> cut_edges;
    for (const auto& path : separatrices) {
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            uint32_t e = mesh.find_edge(path[i], path[i + 1]);
            if (e != INVALID_INDEX) cut_edges.insert(e);
        }
    }

    // Also include sharp, seam and boundary edges as cuts
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.edges[e].is_sharp || mesh.edges[e].is_seam || mesh.is_boundary_edge(static_cast<uint32_t>(e))) {
            cut_edges.insert(static_cast<uint32_t>(e));
        }
    }

    // Flood fill faces to form patches
    std::vector<int> face_patch(nf, -1);
    std::vector<std::vector<uint32_t>> patch_faces;

    for (uint32_t f = 0; f < nf; ++f) {
        if (!mesh.is_valid_face(f) || face_patch[f] >= 0) continue;

        int cur_patch_id = static_cast<int>(patch_faces.size());
        patch_faces.emplace_back();

        std::queue<uint32_t> q;
        q.push(f);
        face_patch[f] = cur_patch_id;

        while (!q.empty()) {
            uint32_t cur = q.front();
            q.pop();
            patch_faces[cur_patch_id].push_back(cur);

            for (uint32_t h : mesh.get_face_halfedges(cur)) {
                uint32_t e = mesh.halfedges[h].edge;
                if (cut_edges.count(e)) continue; // Don't cross cut boundary

                uint32_t twin_h = mesh.halfedges[h].twin;
                if (twin_h != INVALID_INDEX) {
                    uint32_t adj_f = mesh.halfedges[twin_h].face;
                    if (adj_f != INVALID_INDEX && mesh.is_valid_face(adj_f) && face_patch[adj_f] < 0) {
                        face_patch[adj_f] = cur_patch_id;
                        q.push(adj_f);
                    }
                }
            }
        }
    }

    // Ensure every patch is a simply-connected topological disk with bounded normal variation (< ~75 deg)
    // so Coons parameterization never wraps around cylinders/spheres or self-intersects.
    auto split_into_connected_components = [&](const std::vector<uint32_t>& faces_subset) -> std::vector<std::vector<uint32_t>> {
        std::unordered_set<uint32_t> subset_set(faces_subset.begin(), faces_subset.end());
        std::unordered_set<uint32_t> visited;
        std::vector<std::vector<uint32_t>> comps;

        for (uint32_t start_f : faces_subset) {
            if (visited.count(start_f)) continue;
            std::vector<uint32_t> comp;
            std::queue<uint32_t> q;
            q.push(start_f);
            visited.insert(start_f);

            while (!q.empty()) {
                uint32_t cur = q.front();
                q.pop();
                comp.push_back(cur);

                for (uint32_t h : mesh.get_face_halfedges(cur)) {
                    uint32_t e = mesh.halfedges[h].edge;
                    if (cut_edges.count(e)) continue;
                    uint32_t tw = mesh.halfedges[h].twin;
                    if (tw != INVALID_INDEX) {
                        uint32_t adj_f = mesh.halfedges[tw].face;
                        if (adj_f != INVALID_INDEX && subset_set.count(adj_f) && !visited.count(adj_f)) {
                            visited.insert(adj_f);
                            q.push(adj_f);
                        }
                    }
                }
            }
            if (!comp.empty()) comps.push_back(std::move(comp));
        }
        return comps;
    };

    for (int refine_pass = 0; refine_pass < 4; ++refine_pass) {
        std::vector<std::vector<uint32_t>> next_patch_faces;
        bool any_split = false;

        for (const auto& p_faces : patch_faces) {
            if (p_faces.size() <= 2) {
                next_patch_faces.push_back(p_faces);
                continue;
            }

            std::unordered_set<uint32_t> p_set(p_faces.begin(), p_faces.end());
            bool has_patch_boundary = false;
            Vec3f avg_n = Vec3f::Zero();
            BoundingBox p_bbox;

            for (uint32_t f : p_faces) {
                avg_n += mesh.compute_face_normal(f) * std::max(mesh.compute_face_area(f), 1e-7f);
                p_bbox.expand(mesh.compute_face_centroid(f));
                if (!has_patch_boundary) {
                    for (uint32_t h : mesh.get_face_halfedges(f)) {
                        uint32_t tw = mesh.halfedges[h].twin;
                        if (tw == INVALID_INDEX || !p_set.count(mesh.halfedges[tw].face)) {
                            has_patch_boundary = true;
                            break;
                        }
                    }
                }
            }

            float n_len = avg_n.norm();
            if (n_len > 1e-6f) avg_n /= n_len;

            float min_dot = 1.0f;
            if (n_len > 1e-6f) {
                for (uint32_t f : p_faces) {
                    min_dot = std::min(min_dot, mesh.compute_face_normal(f).dot(avg_n));
                }
            } else {
                min_dot = -1.0f;
            }

            // Need split if patch has no boundary (closed shell) or wraps > ~75 degrees (min_dot < 0.25)
            if (!has_patch_boundary || min_dot < 0.25f) {
                Vec3f ext = p_bbox.extents();
                int split_axis = 0;
                if (ext[1] >= ext[0] && ext[1] >= ext[2]) split_axis = 1;
                else if (ext[2] >= ext[0] && ext[2] >= ext[1]) split_axis = 2;

                float mid_val = p_bbox.center()[split_axis];
                std::vector<uint32_t> side_a, side_b;
                for (uint32_t f : p_faces) {
                    if (mesh.compute_face_centroid(f)[split_axis] >= mid_val) side_a.push_back(f);
                    else side_b.push_back(f);
                }

                if (!side_a.empty() && !side_b.empty()) {
                    any_split = true;
                    for (auto& c : split_into_connected_components(side_a)) next_patch_faces.push_back(std::move(c));
                    for (auto& c : split_into_connected_components(side_b)) next_patch_faces.push_back(std::move(c));
                    continue;
                }
            }

            next_patch_faces.push_back(p_faces);
        }

        patch_faces = std::move(next_patch_faces);
        if (!any_split) break;
    }

    // Populate out_graph.edges with PatchBoundaryEdge entries
    out_graph.edges.clear();
    for (const auto& path : separatrices) {
        if (path.size() < 2) continue;
        PatchBoundaryEdge b_edge;
        b_edge.edge_id = static_cast<uint32_t>(out_graph.edges.size());
        b_edge.v_start = path.front();
        b_edge.v_end = path.back();
        b_edge.path_vertices = path;

        float int_len = 0.0f;
        for (size_t k = 0; k + 1 < path.size(); ++k) {
            uint32_t a = path[k];
            uint32_t b = path[k + 1];
            float d = (mesh.vertices[a].pos - mesh.vertices[b].pos).norm();
            float s = 0.5f * (ctx.sizing_field[a] + ctx.sizing_field[b]);
            int_len += d / std::max(s, 1e-5f);
            uint32_t e = mesh.find_edge(a, b);
            if (e != INVALID_INDEX) {
                if (mesh.edges[e].is_sharp) b_edge.is_sharp = true;
                if (mesh.edges[e].is_seam) b_edge.is_seam = true;
            }
        }
        b_edge.continuous_len = std::max(int_len, 1.0f);
        b_edge.integer_divisions = std::max(1, static_cast<int>(std::round(b_edge.continuous_len)));
        out_graph.edges.push_back(std::move(b_edge));
    }

    // Convert face clusters into QuasiQuadPatches
    out_graph.patches.clear();

    for (size_t p_id = 0; p_id < patch_faces.size(); ++p_id) {
        if (patch_faces[p_id].empty()) continue;

        QuasiQuadPatch patch;
        patch.patch_id = static_cast<uint32_t>(p_id);
        patch.member_faces = patch_faces[p_id];
        patch.material_id = mesh.faces[patch_faces[p_id][0]].material_id;

        // Calculate approximate patch area & dimensions
        float patch_area = 0.0f;
        float avg_scale = 0.0f;
        int scale_count = 0;

        for (uint32_t f : patch.member_faces) {
            patch_area += mesh.compute_face_area(f);
            for (uint32_t v : mesh.get_face_vertices(f)) {
                avg_scale += ctx.sizing_field[v];
                scale_count++;
            }
        }

        if (scale_count > 0) avg_scale /= static_cast<float>(scale_count);
        if (avg_scale < 1e-4f) avg_scale = ctx.base_target_length;

        float side_len = std::sqrt(std::max(patch_area, 1e-6f));
        int n_div = std::max(1, static_cast<int>(std::round(side_len / avg_scale)));
        patch.nu = n_div;
        patch.nv = n_div;

        out_graph.patches.push_back(std::move(patch));
    }
}

QrmStatus Stage4PatchGraph::execute(RemeshContext& ctx, PatchGraph& out_graph) {
    if (!ctx.notify_progress(0.55f, "Stage 4: Motorcycle Graph & Patch Partitioning")) {
        return QRM_ERR_CANCELLED;
    }

    std::vector<std::vector<uint32_t>> separatrices;
    trace_separatrices(ctx, separatrices);
    enforce_closed_loops_on_cylinders(ctx, separatrices);
    build_quasi_quad_patches(ctx, separatrices, out_graph);

    return QRM_OK;
}

} // namespace qremesh
