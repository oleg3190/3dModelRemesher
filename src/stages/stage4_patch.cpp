#include "stages/stage4_patch.h"
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace qremesh {

void Stage4PatchGraph::trace_separatrices(
    const RemeshContext& ctx,
    std::vector<std::vector<uint32_t>>& separatrices
) {
    const auto& mesh = ctx.working_mesh;

    // Deterministic seed collection sorted by vertex ID
    std::vector<uint32_t> seed_vertices;
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_deleted) continue;
        if (ctx.vertex_singularities[v] != 0 || mesh.vertices[v].is_corner) {
            seed_vertices.push_back(static_cast<uint32_t>(v));
        }
    }

    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_sharp && mesh.is_boundary_vertex(static_cast<uint32_t>(v))) {
            seed_vertices.push_back(static_cast<uint32_t>(v));
        }
    }
    std::sort(seed_vertices.begin(), seed_vertices.end());
    seed_vertices.erase(std::unique(seed_vertices.begin(), seed_vertices.end()), seed_vertices.end());

    std::unordered_set<uint32_t> visited_edges;

    for (uint32_t seed : seed_vertices) {
        auto v_faces = mesh.get_vertex_faces(seed);
        if (v_faces.empty()) continue;

        Vec3f d1 = ctx.cross_field_dir1[v_faces[0]];
        Vec3f d2 = ctx.cross_field_dir2[v_faces[0]];
        std::array<Vec3f, 4> dirs = { d1, d2, -d1, -d2 };

        for (int dir_idx = 0; dir_idx < 4; ++dir_idx) {
            std::vector<uint32_t> path;
            path.push_back(seed);

            uint32_t curr_v = seed;
            Vec3f cur_dir = dirs[dir_idx];

            constexpr int MAX_STEPS = 1000;
            for (int step = 0; step < MAX_STEPS; ++step) {
                auto nbrs = mesh.get_vertex_neighbors(curr_v);
                if (nbrs.empty()) break;
                // Sort neighbors for determinism
                std::sort(nbrs.begin(), nbrs.end());

                uint32_t best_next = INVALID_INDEX;
                float best_dot = -1.0f;

                for (uint32_t nbr : nbrs) {
                    if (path.size() > 1 && nbr == path[path.size() - 2]) continue;
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
                        path.push_back(best_next);
                        break;
                    }
                    visited_edges.insert(e);
                }

                path.push_back(best_next);
                curr_v = best_next;

                if (mesh.is_boundary_vertex(curr_v) ||
                    (curr_v != seed && (ctx.vertex_singularities[curr_v] != 0 || mesh.vertices[curr_v].is_corner))) {
                    break;
                }

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

    for (auto& path : separatrices) {
        if (path.size() < 4) continue;
        uint32_t start_v = path.front();
        uint32_t end_v = path.back();

        if (start_v != end_v) {
            float dist = (mesh.vertices[start_v].pos - mesh.vertices[end_v].pos).norm();
            float local_scale = std::max(ctx.sizing_field[start_v], ctx.sizing_field[end_v]);

            if (dist < 1.5f * local_scale) {
                path.push_back(start_v);
            }
        }
    }
}

void Stage4PatchGraph::build_quasi_quad_patches(
    RemeshContext& ctx,
    const std::vector<std::vector<uint32_t>>& separatrices,
    PatchGraph& out_graph
) {
    const auto& mesh = ctx.working_mesh;
    uint32_t nf = static_cast<uint32_t>(mesh.faces.size());

    // 1. Initial cut edges: separatrices, sharp, seams, mesh boundary
    std::vector<bool> is_cut_edge(mesh.edges.size(), false);
    for (const auto& path : separatrices) {
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            uint32_t e = mesh.find_edge(path[i], path[i + 1]);
            if (e != INVALID_INDEX) is_cut_edge[e] = true;
        }
    }

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.edges[e].is_sharp || mesh.edges[e].is_seam || mesh.is_boundary_edge(static_cast<uint32_t>(e))) {
            is_cut_edge[e] = true;
        }
    }

    // 2. Flood fill connected face components bounded by cut edges
    auto flood_fill_patches = [&](const std::vector<bool>& cuts) -> std::vector<int> {
        std::vector<int> f_patch(nf, -1);
        int patch_counter = 0;
        for (uint32_t f = 0; f < nf; ++f) {
            if (!mesh.is_valid_face(f) || f_patch[f] >= 0) continue;
            std::queue<uint32_t> q;
            q.push(f);
            f_patch[f] = patch_counter;

            while (!q.empty()) {
                uint32_t cur = q.front();
                q.pop();

                for (uint32_t h : mesh.get_face_halfedges(cur)) {
                    uint32_t e = mesh.halfedges[h].edge;
                    if (cuts[e]) continue;
                    uint32_t tw = mesh.halfedges[h].twin;
                    if (tw != INVALID_INDEX) {
                        uint32_t adj_f = mesh.halfedges[tw].face;
                        if (adj_f != INVALID_INDEX && mesh.is_valid_face(adj_f) && f_patch[adj_f] < 0) {
                            f_patch[adj_f] = patch_counter;
                            q.push(adj_f);
                        }
                    }
                }
            }
            patch_counter++;
        }
        return f_patch;
    };

    std::vector<int> face_patch = flood_fill_patches(is_cut_edge);

    // 3. For any patch with extreme curvature (> 90 deg) or closed shell without boundaries,
    // add cut edges along true mesh edges separating the components
    for (int pass = 0; pass < 3; ++pass) {
        int num_patches = 0;
        for (int p : face_patch) if (p >= num_patches) num_patches = p + 1;
        std::vector<std::vector<uint32_t>> patch_faces(num_patches);
        for (uint32_t f = 0; f < nf; ++f) {
            if (face_patch[f] >= 0) patch_faces[face_patch[f]].push_back(f);
        }

        bool added_cuts = false;
        for (int p = 0; p < num_patches; ++p) {
            const auto& p_f = patch_faces[p];
            if (p_f.size() < 10) continue;

            Vec3f avg_n = Vec3f::Zero();
            BoundingBox p_box;
            for (uint32_t f : p_f) {
                avg_n += mesh.compute_face_normal(f) * mesh.compute_face_area(f);
                p_box.expand(mesh.compute_face_centroid(f));
            }
            if (avg_n.norm() > 1e-6f) avg_n.normalize();

            float min_dot = 1.0f;
            for (uint32_t f : p_f) {
                min_dot = std::min(min_dot, mesh.compute_face_normal(f).dot(avg_n));
            }

            if (min_dot < 0.2f) {
                Vec3f ext = p_box.extents();
                int split_axis = (ext[1] >= ext[0] && ext[1] >= ext[2]) ? 1 : ((ext[2] >= ext[0] && ext[2] >= ext[1]) ? 2 : 0);
                float mid_val = p_box.center()[split_axis];

                // Mark cutting edges that cross mid_val
                for (uint32_t f : p_f) {
                    for (uint32_t h : mesh.get_face_halfedges(f)) {
                        uint32_t tw = mesh.halfedges[h].twin;
                        if (tw != INVALID_INDEX) {
                            uint32_t adj_f = mesh.halfedges[tw].face;
                            if (adj_f != INVALID_INDEX && face_patch[adj_f] == p) {
                                float c1 = mesh.compute_face_centroid(f)[split_axis];
                                float c2 = mesh.compute_face_centroid(adj_f)[split_axis];
                                if ((c1 >= mid_val && c2 < mid_val) || (c1 < mid_val && c2 >= mid_val)) {
                                    uint32_t e = mesh.halfedges[h].edge;
                                    if (!is_cut_edge[e]) {
                                        is_cut_edge[e] = true;
                                        added_cuts = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (added_cuts) {
            face_patch = flood_fill_patches(is_cut_edge);
        } else {
            break;
        }
    }

    // 4. Merge tiny patches (< 0.25 * h^2 area) into adjacent neighbor with closest normal
    // unless separated by sharp / seam
    int total_patches = 0;
    for (int p : face_patch) if (p >= total_patches) total_patches = p + 1;

    std::vector<float> patch_areas(total_patches, 0.0f);
    std::vector<Vec3f> patch_normals(total_patches, Vec3f::Zero());
    for (uint32_t f = 0; f < nf; ++f) {
        int p = face_patch[f];
        if (p >= 0) {
            float a = mesh.compute_face_area(f);
            patch_areas[p] += a;
            patch_normals[p] += mesh.compute_face_normal(f) * a;
        }
    }
    for (int p = 0; p < total_patches; ++p) {
        if (patch_normals[p].norm() > 1e-6f) patch_normals[p].normalize();
    }

    float min_area_thresh = 0.25f * ctx.base_target_length * ctx.base_target_length;
    std::vector<int> patch_remap(total_patches);
    for (int p = 0; p < total_patches; ++p) patch_remap[p] = p;

    // Cycle-safe representative lookup. patch_remap is written *while* we iterate, so a naive
    // "follow until self" walk can end up in a p -> q -> p cycle and spin forever.
    auto remap_root = [&patch_remap](int start) {
        if (start < 0 || start >= static_cast<int>(patch_remap.size())) return start;
        int root = start;
        int guard = 0;
        while (patch_remap[root] != root && guard++ < static_cast<int>(patch_remap.size())) {
            root = patch_remap[root];
        }
        return root;
    };

    for (int p = 0; p < total_patches; ++p) {
        if (patch_areas[p] < min_area_thresh) {
            // Find best neighbor
            int best_adj = -1;
            float best_dot = -2.0f;

            for (uint32_t f = 0; f < nf; ++f) {
                if (face_patch[f] != p) continue;
                for (uint32_t h : mesh.get_face_halfedges(f)) {
                    uint32_t tw = mesh.halfedges[h].twin;
                    if (tw != INVALID_INDEX) {
                        uint32_t adj_f = mesh.halfedges[tw].face;
                        if (adj_f != INVALID_INDEX && mesh.is_valid_face(adj_f)) {
                            int fp = face_patch[adj_f];
                            if (fp < 0) continue;
                            int adj_p = remap_root(fp);
                            if (adj_p != p) {
                                uint32_t e = mesh.halfedges[h].edge;
                                if (!mesh.edges[e].is_sharp && !mesh.edges[e].is_seam) {
                                    float dot = patch_normals[p].dot(patch_normals[adj_p]);
                                    if (dot > best_dot) {
                                        best_dot = dot;
                                        best_adj = adj_p;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (best_adj >= 0) {
                patch_remap[p] = remap_root(best_adj);
            }
        }
    }

    // Apply patch remap and re-index compactly
    std::vector<int> final_remap(total_patches, -1);
    int active_patch_count = 0;
    for (int p = 0; p < total_patches; ++p) {
        int root = remap_root(p);
        if (final_remap[root] < 0) {
            final_remap[root] = active_patch_count++;
        }
        patch_remap[p] = final_remap[root];
    }

    for (uint32_t f = 0; f < nf; ++f) {
        if (face_patch[f] >= 0) {
            face_patch[f] = patch_remap[face_patch[f]];
        }
    }

    // Recompute boundary cut edges based on final patch IDs
    is_cut_edge.assign(mesh.edges.size(), false);
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h0 = mesh.edges[e].halfedge;
        uint32_t h1 = mesh.halfedges[h0].twin;
        if (h1 == INVALID_INDEX) {
            is_cut_edge[e] = true;
        } else {
            uint32_t f0 = mesh.halfedges[h0].face;
            uint32_t f1 = mesh.halfedges[h1].face;
            if (f0 != INVALID_INDEX && f1 != INVALID_INDEX) {
                if (face_patch[f0] != face_patch[f1]) {
                    is_cut_edge[e] = true;
                }
            }
        }
    }

    // 5. Build canonical graph: find junctions (degree != 2 in cut subgraph, corners, or patch boundary changes)
    // Cut subgraph degree per vertex:
    std::vector<int> cut_deg(mesh.vertices.size(), 0);
    std::vector<std::vector<uint32_t>> cut_nbrs(mesh.vertices.size());
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (is_cut_edge[e]) {
            uint32_t h = mesh.edges[e].halfedge;
            uint32_t u = mesh.from_vertex(h);
            uint32_t v = mesh.halfedges[h].to_vertex;
            cut_deg[u]++;
            cut_deg[v]++;
            cut_nbrs[u].push_back(v);
            cut_nbrs[v].push_back(u);
        }
    }

    std::vector<bool> is_junction(mesh.vertices.size(), false);
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (cut_deg[v] == 0) continue;
        if (cut_deg[v] != 2 || mesh.vertices[v].is_corner || ctx.vertex_singularities[v] != 0) {
            is_junction[v] = true;
        }
    }

    // 6. Trace canonical paths between junctions
    std::vector<bool> edge_visited(mesh.edges.size(), false);
    std::vector<CanonicalEdgePath> paths;

    // Helper lambda to get left and right patches along directed edge (u -> v)
    auto get_edge_patches = [&](uint32_t u, uint32_t v) -> std::pair<uint32_t, uint32_t> {
        uint32_t e = mesh.find_edge(u, v);
        if (e == INVALID_INDEX) return {INVALID_INDEX, INVALID_INDEX};
        uint32_t h = mesh.edges[e].halfedge;
        if (mesh.from_vertex(h) != u) h = mesh.halfedges[h].twin;

        uint32_t left_p = INVALID_INDEX;
        uint32_t right_p = INVALID_INDEX;
        if (h != INVALID_INDEX && mesh.halfedges[h].face != INVALID_INDEX) {
            left_p = face_patch[mesh.halfedges[h].face];
        }
        uint32_t tw = (h != INVALID_INDEX) ? mesh.halfedges[h].twin : INVALID_INDEX;
        if (tw != INVALID_INDEX && mesh.halfedges[tw].face != INVALID_INDEX) {
            right_p = face_patch[mesh.halfedges[tw].face];
        }
        return {left_p, right_p};
    };

    // First trace from all known junctions
    std::vector<uint32_t> junctions;
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (is_junction[v]) junctions.push_back(static_cast<uint32_t>(v));
    }
    std::sort(junctions.begin(), junctions.end());

    for (uint32_t junc : junctions) {
        auto nbrs = cut_nbrs[junc];
        std::sort(nbrs.begin(), nbrs.end());

        for (uint32_t nbr : nbrs) {
            uint32_t e = mesh.find_edge(junc, nbr);
            if (e == INVALID_INDEX || edge_visited[e]) continue;

            CanonicalEdgePath path;
            path.edge_path_id = static_cast<uint32_t>(paths.size());
            path.node_start = junc;
            path.path_vertices.push_back(junc);
            path.path_vertices.push_back(nbr);
            edge_visited[e] = true;

            float cur_len = (mesh.vertices[nbr].pos - mesh.vertices[junc].pos).norm();
            if (mesh.edges[e].is_sharp) path.is_sharp = true;
            if (mesh.edges[e].is_seam) path.is_seam = true;
            if (mesh.is_boundary_edge(e)) path.is_open_border = true;

            uint32_t prev_v = junc;
            uint32_t curr_v = nbr;
            size_t walk_guard = 0;

            // Guard against cut cycles that contain no junction (would otherwise never terminate)
            while (!is_junction[curr_v] && ++walk_guard <= mesh.vertices.size()) {
                uint32_t next_v = INVALID_INDEX;
                for (uint32_t cand : cut_nbrs[curr_v]) {
                    if (cand != prev_v) {
                        next_v = cand;
                        break;
                    }
                }
                if (next_v == INVALID_INDEX) break;

                uint32_t next_e = mesh.find_edge(curr_v, next_v);
                if (next_e != INVALID_INDEX) {
                    edge_visited[next_e] = true;
                    if (mesh.edges[next_e].is_sharp) path.is_sharp = true;
                    if (mesh.edges[next_e].is_seam) path.is_seam = true;
                    if (mesh.is_boundary_edge(next_e)) path.is_open_border = true;
                    cur_len += (mesh.vertices[next_v].pos - mesh.vertices[curr_v].pos).norm();
                }

                path.path_vertices.push_back(next_v);
                prev_v = curr_v;
                curr_v = next_v;
            }

            path.node_end = curr_v;
            path.continuous_len = std::max(cur_len, 1e-5f);
            auto [lp, rp] = get_edge_patches(path.path_vertices[0], path.path_vertices[1]);
            path.left_patch = lp;
            path.right_patch = rp;

            paths.push_back(std::move(path));
        }
    }

    // Now trace any isolated closed loops where cut_deg == 2 everywhere (no junction was hit)
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!is_cut_edge[e] || edge_visited[e]) continue;

        uint32_t h = mesh.edges[e].halfedge;
        uint32_t start_v = mesh.from_vertex(h);
        uint32_t next_v = mesh.halfedges[h].to_vertex;

        // Trace full loop to find smallest vertex ID to use as canonical start
        std::vector<uint32_t> loop;
        loop.push_back(start_v);
        uint32_t curr = next_v;
        uint32_t prev = start_v;
        while (curr != start_v && loop.size() < mesh.vertices.size()) {
            loop.push_back(curr);
            uint32_t step_next = INVALID_INDEX;
            for (uint32_t cand : cut_nbrs[curr]) {
                if (cand != prev) {
                    step_next = cand;
                    break;
                }
            }
            if (step_next == INVALID_INDEX) break;
            prev = curr;
            curr = step_next;
        }

        if (loop.size() >= 3) {
            // Find vertex with minimum index
            auto min_it = std::min_element(loop.begin(), loop.end());
            std::rotate(loop.begin(), min_it, loop.end());
            loop.push_back(loop.front()); // Close

            CanonicalEdgePath path;
            path.edge_path_id = static_cast<uint32_t>(paths.size());
            path.node_start = loop.front();
            path.node_end = loop.back();
            path.path_vertices = loop;

            float cur_len = 0.0f;
            for (size_t k = 0; k + 1 < loop.size(); ++k) {
                uint32_t e_sub = mesh.find_edge(loop[k], loop[k + 1]);
                if (e_sub != INVALID_INDEX) {
                    edge_visited[e_sub] = true;
                    if (mesh.edges[e_sub].is_sharp) path.is_sharp = true;
                    if (mesh.edges[e_sub].is_seam) path.is_seam = true;
                    if (mesh.is_boundary_edge(e_sub)) path.is_open_border = true;
                }
                cur_len += (mesh.vertices[loop[k + 1]].pos - mesh.vertices[loop[k]].pos).norm();
            }
            path.continuous_len = std::max(cur_len, 1e-5f);
            auto [lp, rp] = get_edge_patches(path.path_vertices[0], path.path_vertices[1]);
            path.left_patch = lp;
            path.right_patch = rp;
            is_junction[path.node_start] = true;
            paths.push_back(std::move(path));
        }
    }

    // 7. Assemble QuasiQuadPatch boundary loops from canonical paths
    // Build directed adjacency for each patch
    std::vector<QuasiQuadPatch> patches(active_patch_count);
    for (int p = 0; p < active_patch_count; ++p) {
        patches[p].patch_id = static_cast<uint32_t>(p);
    }
    for (uint32_t f = 0; f < nf; ++f) {
        int p = face_patch[f];
        if (p >= 0 && p < active_patch_count) {
            patches[p].member_faces.push_back(f);
            if (patches[p].material_id == 0) patches[p].material_id = mesh.faces[f].material_id;
            patches[p].avg_normal += mesh.compute_face_normal(f) * mesh.compute_face_area(f);
        }
    }
    for (int p = 0; p < active_patch_count; ++p) {
        if (patches[p].avg_normal.norm() > 1e-6f) patches[p].avg_normal.normalize();
        else patches[p].avg_normal = Vec3f(0.0f, 0.0f, 1.0f);
    }

    // Map directed edge (u, v) in canonical path to OrientedEdgeRef
    std::map<std::pair<uint32_t, uint32_t>, OrientedEdgeRef> directed_edge_to_path;
    for (const auto& path : paths) {
        directed_edge_to_path[{path.node_start, path.node_end}] = { path.edge_path_id, true };
        directed_edge_to_path[{path.node_end, path.node_start}] = { path.edge_path_id, false };
    }

    // For each patch, find boundary halfedges whose twin is not in the same patch
    size_t loops_missing = 0, path_lookup_miss = 0, border_steps = 0;
    for (int p = 0; p < active_patch_count; ++p) {
        std::unordered_set<uint32_t> p_faces(patches[p].member_faces.begin(), patches[p].member_faces.end());
        std::unordered_map<uint32_t, uint32_t> next_v_on_border;

        for (uint32_t f : patches[p].member_faces) {
            for (uint32_t h : mesh.get_face_halfedges(f)) {
                uint32_t tw = mesh.halfedges[h].twin;
                if (tw == INVALID_INDEX || !p_faces.count(mesh.halfedges[tw].face)) {
                    uint32_t u = mesh.from_vertex(h);
                    uint32_t v = mesh.halfedges[h].to_vertex;
                    next_v_on_border[u] = v;
                }
            }
        }

        if (next_v_on_border.empty()) continue;

        // Find a start vertex that is a junction if possible
        uint32_t start_v = next_v_on_border.begin()->first;
        for (const auto& kv : next_v_on_border) {
            if (is_junction[kv.first]) {
                start_v = kv.first;
                break;
            }
        }

        // Trace boundary loop through junctions
        std::vector<uint32_t> junction_cycle;
        uint32_t curr = start_v;
        std::unordered_set<uint32_t> visited_border;

        while (visited_border.size() < next_v_on_border.size()) {
            // Revisiting an already-seen vertex without closing the loop: stop instead of spinning
            if (!visited_border.insert(curr).second) break;
            if (is_junction[curr]) junction_cycle.push_back(curr);

            auto it = next_v_on_border.find(curr);
            if (it == next_v_on_border.end()) break;
            curr = it->second;
            if (curr == start_v) break;
        }

        // Reconstruct boundary_loop in order
        if (junction_cycle.size() >= 2) {
            for (size_t i = 0; i < junction_cycle.size(); ++i) {
                uint32_t u = junction_cycle[i];
                uint32_t v = junction_cycle[(i + 1) % junction_cycle.size()];
                border_steps++;
                auto it = directed_edge_to_path.find({u, v});
                if (it != directed_edge_to_path.end()) {
                    patches[p].boundary_loop.push_back(it->second);
                } else {
                    path_lookup_miss++;
                }
            }
        } else {
            loops_missing++;
        }
    }

    std::cout << "[stage4] junctions=" << junctions.size()
              << " canonical_paths=" << paths.size()
              << " patches=" << active_patch_count
              << " patches_without_loop=" << loops_missing
              << " path_lookup_miss=" << path_lookup_miss
              << " border_steps=" << border_steps
              << std::endl;

    out_graph.edge_paths = std::move(paths);
    out_graph.patches = std::move(patches);
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
