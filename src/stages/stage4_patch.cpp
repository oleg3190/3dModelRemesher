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
    // Spatially filter all seeds to prevent micro-clusters and ray explosion
    float min_seed_dist = 0.6f * ctx.base_target_length;
    std::vector<uint32_t> seed_vertices;
    std::vector<Vec3f> accepted_pos;

    auto try_add_seed = [&](uint32_t s, float min_d) {
        if (mesh.vertices[s].is_deleted) return;
        const Vec3f& p = mesh.vertices[s].pos;
        for (const auto& ap : accepted_pos) {
            if ((p - ap).squaredNorm() < min_d * min_d) return;
        }
        seed_vertices.push_back(s);
        accepted_pos.push_back(p);
    };

    // 1. Feature corners first (highest priority)
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_corner) {
            try_add_seed(static_cast<uint32_t>(v), min_seed_dist);
        }
    }

    // 2. Significant boundary turns
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (!mesh.is_boundary_vertex(static_cast<uint32_t>(v))) continue;
        auto nbrs = mesh.get_vertex_neighbors(static_cast<uint32_t>(v));
        std::vector<uint32_t> bnd_nbrs;
        for (uint32_t n : nbrs) {
            if (mesh.is_boundary_vertex(n)) bnd_nbrs.push_back(n);
        }
        if (bnd_nbrs.size() == 2) {
            Vec3f d1 = (mesh.vertices[bnd_nbrs[0]].pos - mesh.vertices[v].pos).normalized();
            Vec3f d2 = (mesh.vertices[bnd_nbrs[1]].pos - mesh.vertices[v].pos).normalized();
            if (d1.dot(d2) > -0.7f) {
                try_add_seed(static_cast<uint32_t>(v), min_seed_dist);
            }
        }
    }

    // 3. Singularities spaced by local sizing field
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (ctx.vertex_singularities[v] != 0) {
            float local_r = (v < ctx.sizing_field.size() && ctx.sizing_field[v] > 1e-4f) ?
                            1.5f * ctx.sizing_field[v] : min_seed_dist;
            try_add_seed(static_cast<uint32_t>(v), local_r);
        }
    }

    std::cout << "[stage4/diag] seed_vertices_filtered=" << seed_vertices.size() << std::endl;

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

    // 3. Fast iterative merge of small patches using an explicit Patch Adjacency Graph (O(N_f + P log P))
    int initial_patches = 0;
    for (int p : face_patch) if (p >= initial_patches) initial_patches = p + 1;

    std::vector<float> patch_areas(initial_patches, 0.0f);
    std::vector<Vec3f> patch_normals(initial_patches, Vec3f::Zero());
    std::vector<size_t> patch_face_counts(initial_patches, 0);
    float total_area = 0.0f;

    for (uint32_t f = 0; f < nf; ++f) {
        int p = face_patch[f];
        if (p >= 0) {
            float a = mesh.compute_face_area(f);
            patch_areas[p] += a;
            patch_normals[p] += mesh.compute_face_normal(f) * a;
            patch_face_counts[p]++;
            total_area += a;
        }
    }
    for (int p = 0; p < initial_patches; ++p) {
        if (patch_normals[p].norm() > 1e-6f) patch_normals[p].normalize();
        else patch_normals[p] = Vec3f(0.0f, 0.0f, 1.0f);
    }

    // Merge small patches based on target quad size to preserve natural quasi-quad layout
    int target_patches = initial_patches;
    float target_patch_area = total_area / static_cast<float>(std::max(1, initial_patches));
    float min_area_thresh = 0.5f * ctx.base_target_length * ctx.base_target_length;

    if (ctx.params->target_quad_count > 0) {
        target_patches = std::max(30, static_cast<int>(ctx.params->target_quad_count / 5));
        target_patch_area = total_area / static_cast<float>(target_patches);
        min_area_thresh = 0.22f * target_patch_area;
    }

    std::cout << "[stage4] initial_patches=" << initial_patches 
              << " target_patches=" << target_patches
              << " min_area_thresh=" << min_area_thresh 
              << std::endl;

    // Build dynamic patch adjacency list
    struct PatchAdjInfo {
        float shared_len = 0.0f;
        bool is_feature = false; // separated by sharp or seam
    };
    std::vector<std::unordered_map<int, PatchAdjInfo>> patch_adj(initial_patches);

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h0 = mesh.edges[e].halfedge;
        uint32_t h1 = mesh.halfedges[h0].twin;
        if (h1 == INVALID_INDEX) continue;
        uint32_t f0 = mesh.halfedges[h0].face;
        uint32_t f1 = mesh.halfedges[h1].face;
        if (f0 == INVALID_INDEX || f1 == INVALID_INDEX) continue;
        int p0 = face_patch[f0];
        int p1 = face_patch[f1];
        if (p0 >= 0 && p1 >= 0 && p0 != p1) {
            float elen = mesh.compute_edge_length(static_cast<uint32_t>(e));
            bool is_feat = (mesh.edges[e].is_sharp || mesh.edges[e].is_seam);
            patch_adj[p0][p1].shared_len += elen;
            patch_adj[p0][p1].is_feature |= is_feat;
            patch_adj[p1][p0].shared_len += elen;
            patch_adj[p1][p0].is_feature |= is_feat;
        }
    }

    std::vector<float> patch_perimeters(initial_patches, 0.0f);
    for (int p = 0; p < initial_patches; ++p) {
        for (const auto& [nbr, info] : patch_adj[p]) {
            patch_perimeters[p] += info.shared_len;
        }
    }

    // Disjoint set for patch merging
    std::vector<int> patch_parent(initial_patches);
    for (int p = 0; p < initial_patches; ++p) patch_parent[p] = p;
    auto find_root = [&](int x) -> int {
        int r = x;
        while (patch_parent[r] != r) r = patch_parent[r];
        int curr = x;
        while (curr != r) {
            int nxt = patch_parent[curr];
            patch_parent[curr] = r;
            curr = nxt;
        }
        return r;
    };

    // Priority queue to merge smallest patches first
    using PatchAreaPair = std::pair<float, int>;
    std::priority_queue<PatchAreaPair, std::vector<PatchAreaPair>, std::greater<PatchAreaPair>> pq;
    for (int p = 0; p < initial_patches; ++p) {
        if (patch_areas[p] < min_area_thresh || patch_face_counts[p] < 3 || initial_patches > target_patches) {
            pq.push({patch_areas[p], p});
        }
    }

    int remaining_patches = initial_patches;

    while (!pq.empty() && remaining_patches > 10) {
        auto [a, p] = pq.top();
        pq.pop();

        int root_p = find_root(p);
        if (root_p != p) continue; // Already merged
        if (patch_areas[root_p] >= min_area_thresh && patch_face_counts[root_p] >= 4) {
            if (remaining_patches <= target_patches) continue;
        }

        int best_neighbor = -1;
        float best_score = -1e9f;

        // Pass 1: find neighbor strictly respecting max area and max degree (degree <= 6)
        for (const auto& [nbr, info] : patch_adj[root_p]) {
            int root_nbr = find_root(nbr);
            if (root_nbr == root_p) continue;
            if (patch_adj[root_nbr].size() > 6) continue;
            if (patch_areas[root_nbr] + patch_areas[root_p] > 2.0f * target_patch_area) continue;

            float dot = patch_normals[root_p].dot(patch_normals[root_nbr]);
            if (info.is_feature && patch_areas[root_p] > 0.05f * min_area_thresh) continue;
            if (dot < 0.35f && patch_areas[root_p] > 0.05f * min_area_thresh) continue;

            float perim_p = std::max(1e-5f, patch_perimeters[root_p]);
            float perim_nbr = std::max(1e-5f, patch_perimeters[root_nbr]);
            float contact = info.shared_len / std::min(perim_p, perim_nbr);

            float score = dot * 3.0f + contact * 8.0f 
                        - 2.0f * (patch_areas[root_nbr] / target_patch_area) 
                        - 1.0f * static_cast<float>(patch_adj[root_nbr].size());
            if (score > best_score) {
                best_score = score;
                best_neighbor = root_nbr;
            }
        }

        // Pass 2: fallback for small patches that couldn't merge in pass 1
        if (best_neighbor < 0) {
            float min_nbr_metric = 1e9f;
            for (const auto& [nbr, info] : patch_adj[root_p]) {
                int root_nbr = find_root(nbr);
                if (root_nbr == root_p) continue;
                if (patch_adj[root_nbr].size() > 6) continue;
                if (patch_areas[root_nbr] + patch_areas[root_p] > 2.0f * target_patch_area) continue;
                float dot = patch_normals[root_p].dot(patch_normals[root_nbr]);
                if (info.is_feature && patch_face_counts[root_p] > 4) continue;
                if (dot < -0.05f && patch_face_counts[root_p] > 4) continue;
                float metric = patch_areas[root_nbr] + 0.2f * target_patch_area * static_cast<float>(patch_adj[root_nbr].size());
                if (metric < min_nbr_metric) {
                    min_nbr_metric = metric;
                    best_neighbor = root_nbr;
                }
            }
        }

        // Pass 3: merge small patches (< min_area_thresh or < 6 faces) with compatible neighbor
        if (best_neighbor < 0 && (patch_areas[root_p] < min_area_thresh || patch_face_counts[root_p] < 6)) {
            float max_shared = -1.0f;
            for (const auto& [nbr, info] : patch_adj[root_p]) {
                int root_nbr = find_root(nbr);
                if (root_nbr == root_p) continue;
                float dot = patch_normals[root_p].dot(patch_normals[root_nbr]);
                if (dot < -0.2f && patch_face_counts[root_p] >= 4) continue;
                if (info.shared_len > max_shared) {
                    max_shared = info.shared_len;
                    best_neighbor = root_nbr;
                }
            }
            if (best_neighbor < 0 && patch_face_counts[root_p] < 4) {
                for (const auto& [nbr, info] : patch_adj[root_p]) {
                    int root_nbr = find_root(nbr);
                    if (root_nbr == root_p) continue;
                    if (info.shared_len > max_shared) {
                        max_shared = info.shared_len;
                        best_neighbor = root_nbr;
                    }
                }
            }
        }

        if (best_neighbor >= 0) {
            float shared = patch_adj[root_p][best_neighbor].shared_len;
            // Merge root_p into best_neighbor with area-weighted normal
            patch_parent[root_p] = best_neighbor;
            Vec3f weighted_n = patch_normals[best_neighbor] * patch_areas[best_neighbor] +
                               patch_normals[root_p] * patch_areas[root_p];
            if (weighted_n.norm() > 1e-6f) {
                patch_normals[best_neighbor] = weighted_n.normalized();
            }
            patch_areas[best_neighbor] += patch_areas[root_p];
            patch_face_counts[best_neighbor] += patch_face_counts[root_p];
            patch_perimeters[best_neighbor] = std::max(1e-5f, patch_perimeters[best_neighbor] + patch_perimeters[root_p] - 2.0f * shared);
            remaining_patches--;

            // Contract graph edges
            for (const auto& [nbr, info] : patch_adj[root_p]) {
                int root_nbr = find_root(nbr);
                if (root_nbr == best_neighbor || root_nbr == root_p) continue;

                auto& adj_entry = patch_adj[best_neighbor][root_nbr];
                adj_entry.shared_len += info.shared_len;
                adj_entry.is_feature |= info.is_feature;

                patch_adj[root_nbr].erase(root_p);
                auto& rev_entry = patch_adj[root_nbr][best_neighbor];
                rev_entry.shared_len += info.shared_len;
                rev_entry.is_feature |= info.is_feature;
            }
            patch_adj[root_p].clear();
            patch_adj[best_neighbor].erase(root_p);

            // Re-enqueue if still small or above target budget, but avoid growing beyond target
            if (patch_areas[best_neighbor] < 1.5f * target_patch_area &&
                (patch_areas[best_neighbor] < min_area_thresh || patch_face_counts[best_neighbor] < 3 || remaining_patches > target_patches)) {
                pq.push({patch_areas[best_neighbor], best_neighbor});
            }
        }
    }

    // Merge small patches with < 4 neighbors into compatible neighbors
    for (int pass = 0; pass < 4; ++pass) {
        bool merged_any = false;
        for (int p = 0; p < initial_patches; ++p) {
            int root_p = find_root(p);
            if (root_p != p) continue;
            size_t deg = patch_adj[root_p].size();
            if (deg < 4 && remaining_patches > 20 &&
                (patch_areas[root_p] < 0.8f * target_patch_area || patch_face_counts[root_p] < 5)) {
                int best_nbr = -1;
                float best_len = -1.0f;
                for (const auto& [nbr, info] : patch_adj[root_p]) {
                    int root_nbr = find_root(nbr);
                    if (root_nbr == root_p) continue;
                    if (info.is_feature && patch_face_counts[root_p] >= 5) continue;
                    if (patch_adj[root_nbr].size() > 7 && patch_face_counts[root_p] >= 5) continue;
                    if (patch_areas[root_nbr] + patch_areas[root_p] > 2.2f * target_patch_area && patch_face_counts[root_p] >= 5) continue;
                    float dot = patch_normals[root_p].dot(patch_normals[root_nbr]);
                    if (dot < 0.25f && patch_face_counts[root_p] >= 5) continue;
                    if (info.shared_len > best_len) {
                        best_len = info.shared_len;
                        best_nbr = root_nbr;
                    }
                }
                if (best_nbr >= 0) {
                    patch_parent[root_p] = best_nbr;
                    Vec3f weighted_n = patch_normals[best_nbr] * patch_areas[best_nbr] +
                                       patch_normals[root_p] * patch_areas[root_p];
                    if (weighted_n.norm() > 1e-6f) {
                        patch_normals[best_nbr] = weighted_n.normalized();
                    }
                    patch_areas[best_nbr] += patch_areas[root_p];
                    patch_face_counts[best_nbr] += patch_face_counts[root_p];
                    float shared = patch_adj[root_p][best_nbr].shared_len;
                    patch_perimeters[best_nbr] = std::max(1e-5f, patch_perimeters[best_nbr] + patch_perimeters[root_p] - 2.0f * shared);
                    remaining_patches--;

                    for (const auto& [nbr, info] : patch_adj[root_p]) {
                        int root_nbr = find_root(nbr);
                        if (root_nbr == best_nbr || root_nbr == root_p) continue;
                        auto& adj_entry = patch_adj[best_nbr][root_nbr];
                        adj_entry.shared_len += info.shared_len;
                        adj_entry.is_feature |= info.is_feature;

                        patch_adj[root_nbr].erase(root_p);
                        auto& rev_entry = patch_adj[root_nbr][best_nbr];
                        rev_entry.shared_len += info.shared_len;
                        rev_entry.is_feature |= info.is_feature;
                    }
                    patch_adj[root_p].clear();
                    patch_adj[best_nbr].erase(root_p);
                    merged_any = true;
                }
            }
        }
        if (!merged_any) break;
    }

    // Compact patch IDs
    std::vector<int> final_remap(initial_patches, -1);
    int active_patch_count = 0;
    for (int p = 0; p < initial_patches; ++p) {
        int root = find_root(p);
        if (final_remap[root] < 0) {
            final_remap[root] = active_patch_count++;
        }
    }
    for (uint32_t f = 0; f < nf; ++f) {
        if (face_patch[f] >= 0) {
            face_patch[f] = final_remap[find_root(face_patch[f])];
        }
    }

    // Ensure connectivity: re-run flood fill on patch boundaries so each patch is truly 1 connected component
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
            if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                is_cut_edge[e] = true;
            }
        }
    }

    face_patch = flood_fill_patches(is_cut_edge);
    active_patch_count = 0;
    for (int p : face_patch) if (p >= active_patch_count) active_patch_count = p + 1;

    // Eliminate internal islands/holes inside patches so every patch is a simple topological disk
    for (int hole_pass = 0; hole_pass < 3; ++hole_pass) {
        std::vector<std::vector<uint32_t>> p_bnd_edges(active_patch_count);
        for (size_t e = 0; e < mesh.edges.size(); ++e) {
            if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
            uint32_t h0 = mesh.edges[e].halfedge;
            uint32_t h1 = mesh.halfedges[h0].twin;
            if (h1 == INVALID_INDEX) {
                uint32_t f0 = mesh.halfedges[h0].face;
                if (f0 != INVALID_INDEX && face_patch[f0] >= 0) {
                    p_bnd_edges[face_patch[f0]].push_back(static_cast<uint32_t>(e));
                }
            } else {
                uint32_t f0 = mesh.halfedges[h0].face;
                uint32_t f1 = mesh.halfedges[h1].face;
                if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                    if (face_patch[f0] >= 0) p_bnd_edges[face_patch[f0]].push_back(static_cast<uint32_t>(e));
                    if (face_patch[f1] >= 0) p_bnd_edges[face_patch[f1]].push_back(static_cast<uint32_t>(e));
                }
            }
        }

        bool absorbed_any_island = false;
        for (int q = 0; q < active_patch_count; ++q) {
            const auto& b_edges = p_bnd_edges[q];
            if (b_edges.size() < 4) continue;

            std::unordered_map<uint32_t, std::vector<uint32_t>> v_to_be;
            for (uint32_t e : b_edges) {
                uint32_t h = mesh.edges[e].halfedge;
                uint32_t u = mesh.from_vertex(h);
                uint32_t v = mesh.halfedges[h].to_vertex;
                v_to_be[u].push_back(e);
                v_to_be[v].push_back(e);
            }

            std::unordered_set<uint32_t> vis_e;
            std::vector<std::vector<uint32_t>> b_comps;
            std::vector<float> comp_perim;

            for (uint32_t start_e : b_edges) {
                if (vis_e.count(start_e)) continue;
                std::vector<uint32_t> comp;
                float perim = 0.0f;
                std::queue<uint32_t> eq;
                eq.push(start_e);
                vis_e.insert(start_e);
                while (!eq.empty()) {
                    uint32_t ce = eq.front();
                    eq.pop();
                    comp.push_back(ce);
                    uint32_t ch = mesh.edges[ce].halfedge;
                    uint32_t u = mesh.from_vertex(ch);
                    uint32_t v = mesh.halfedges[ch].to_vertex;
                    perim += (mesh.vertices[v].pos - mesh.vertices[u].pos).norm();
                    for (uint32_t ne : v_to_be[u]) {
                        if (!vis_e.count(ne)) { vis_e.insert(ne); eq.push(ne); }
                    }
                    for (uint32_t ne : v_to_be[v]) {
                        if (!vis_e.count(ne)) { vis_e.insert(ne); eq.push(ne); }
                    }
                }
                b_comps.push_back(std::move(comp));
                comp_perim.push_back(perim);
            }

            if (b_comps.size() <= 1) continue;

            size_t max_c = 0;
            for (size_t c = 1; c < b_comps.size(); ++c) {
                if (comp_perim[c] > comp_perim[max_c]) max_c = c;
            }

            // For each inner boundary component, check if it encloses a small island of faces != q
            for (size_t c = 0; c < b_comps.size(); ++c) {
                if (c == max_c) continue;

                std::vector<uint32_t> island_faces;
                std::unordered_set<uint32_t> vis_f;
                std::queue<uint32_t> fq;
                bool touches_open_bnd = false;

                for (uint32_t ce : b_comps[c]) {
                    uint32_t h0 = mesh.edges[ce].halfedge;
                    uint32_t h1 = mesh.halfedges[h0].twin;
                    if (h1 == INVALID_INDEX) { touches_open_bnd = true; break; }
                    uint32_t f0 = mesh.halfedges[h0].face;
                    uint32_t f1 = mesh.halfedges[h1].face;
                    uint32_t seed_f = (f0 != INVALID_INDEX && face_patch[f0] != q) ? f0 : f1;
                    if (seed_f != INVALID_INDEX && face_patch[seed_f] != q && !vis_f.count(seed_f)) {
                        vis_f.insert(seed_f);
                        fq.push(seed_f);
                    }
                }

                while (!fq.empty() && !touches_open_bnd && island_faces.size() <= 1500) {
                    uint32_t cf = fq.front();
                    fq.pop();
                    island_faces.push_back(cf);

                    for (uint32_t fh : mesh.get_face_halfedges(cf)) {
                        uint32_t tw = mesh.halfedges[fh].twin;
                        if (tw == INVALID_INDEX) {
                            touches_open_bnd = true;
                            break;
                        }
                        uint32_t nf_idx = mesh.halfedges[tw].face;
                        if (nf_idx != INVALID_INDEX && face_patch[nf_idx] != q && !vis_f.count(nf_idx)) {
                            vis_f.insert(nf_idx);
                            fq.push(nf_idx);
                        }
                    }
                }

                if (!touches_open_bnd && !island_faces.empty() && island_faces.size() <= 1500) {
                    for (uint32_t inf : island_faces) {
                        face_patch[inf] = q;
                    }
                    absorbed_any_island = true;
                }
            }
        }

        if (!absorbed_any_island) break;

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
                if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                    is_cut_edge[e] = true;
                }
            }
        }
        face_patch = flood_fill_patches(is_cut_edge);
        active_patch_count = 0;
        for (int p : face_patch) if (p >= active_patch_count) active_patch_count = p + 1;
    }

    // Absorb remaining 1-neighbor pockets, 2-neighbor lenses, and tiny (< 4 faces) slivers
    for (int deg_pass = 0; deg_pass < 4; ++deg_pass) {
        std::vector<int> p_fcount(active_patch_count, 0);
        std::vector<float> p_area(active_patch_count, 0.0f);
        for (uint32_t f = 0; f < nf; ++f) {
            int p = face_patch[f];
            if (p >= 0 && p < active_patch_count) {
                p_fcount[p]++;
                p_area[p] += mesh.compute_face_area(f);
            }
        }

        std::vector<std::map<int, float>> p_shared(active_patch_count);
        for (size_t e = 0; e < mesh.edges.size(); ++e) {
            if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
            uint32_t h0 = mesh.edges[e].halfedge;
            uint32_t h1 = mesh.halfedges[h0].twin;
            if (h1 == INVALID_INDEX) continue;
            uint32_t f0 = mesh.halfedges[h0].face;
            uint32_t f1 = mesh.halfedges[h1].face;
            if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                int p0 = face_patch[f0];
                int p1 = face_patch[f1];
                if (p0 >= 0 && p1 >= 0) {
                    float len = (mesh.vertices[mesh.halfedges[h0].to_vertex].pos - mesh.vertices[mesh.from_vertex(h0)].pos).norm();
                    p_shared[p0][p1] += len;
                    p_shared[p1][p0] += len;
                }
            }
        }

        bool changed = false;
        for (int p = 0; p < active_patch_count; ++p) {
            if (p_fcount[p] == 0 || p_shared[p].empty()) continue;
            if (p_fcount[p] < 4) {
                int best_q = -1;
                float best_l = -1.0f;
                for (const auto& [q, l] : p_shared[p]) {
                    if (q != p && p_fcount[q] > 0 && l > best_l) {
                        best_l = l;
                        best_q = q;
                    }
                }
                if (best_q >= 0) {
                    for (uint32_t f = 0; f < nf; ++f) {
                        if (face_patch[f] == p) face_patch[f] = best_q;
                    }
                    p_fcount[best_q] += p_fcount[p];
                    p_area[best_q] += p_area[p];
                    p_fcount[p] = 0;
                    for (const auto& [nbr, l] : p_shared[p]) {
                        if (nbr != best_q) {
                            p_shared[best_q][nbr] += l;
                            p_shared[nbr][best_q] += l;
                        }
                        p_shared[nbr].erase(p);
                    }
                    p_shared[p].clear();
                    changed = true;
                }
            }
        }
        if (!changed) break;

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
                if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                    is_cut_edge[e] = true;
                }
            }
        }
        face_patch = flood_fill_patches(is_cut_edge);
        active_patch_count = 0;
        for (int p : face_patch) if (p >= active_patch_count) active_patch_count = p + 1;
    }

    // 4. Update cut edges with final active patch IDs
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
            if (f0 != INVALID_INDEX && f1 != INVALID_INDEX && face_patch[f0] != face_patch[f1]) {
                is_cut_edge[e] = true;
            }
        }
    }

    // 5. Build Canonical Cut Graph: cut degree and neighbors
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
        std::unordered_set<int> distinct_p;
        for (uint32_t f : mesh.get_vertex_faces(static_cast<uint32_t>(v))) {
            if (f != INVALID_INDEX && mesh.is_valid_face(f)) {
                distinct_p.insert(face_patch[f]);
            }
        }
        if (cut_deg[v] != 2 || distinct_p.size() >= 3) {
            is_junction[v] = true;
        }
    }

    // Guarantee that every connected component in the cut graph contains at least one junction
    {
        std::vector<bool> comp_vis(mesh.vertices.size(), false);
        for (size_t v = 0; v < mesh.vertices.size(); ++v) {
            if (cut_deg[v] == 0 || comp_vis[v]) continue;

            std::vector<uint32_t> comp;
            std::queue<uint32_t> q;
            q.push(static_cast<uint32_t>(v));
            comp_vis[v] = true;
            bool has_junc = false;

            while (!q.empty()) {
                uint32_t cur = q.front();
                q.pop();
                comp.push_back(cur);
                if (is_junction[cur]) has_junc = true;

                for (uint32_t nbr : cut_nbrs[cur]) {
                    if (!comp_vis[nbr]) {
                        comp_vis[nbr] = true;
                        q.push(nbr);
                    }
                }
            }

            if (!has_junc && !comp.empty()) {
                uint32_t j1 = comp.front();
                is_junction[j1] = true;
                if (comp.size() >= 4) {
                    uint32_t j2 = comp[comp.size() / 2];
                    is_junction[j2] = true;
                }
            }
        }
    }

    // 6. Trace Canonical Paths strictly from junctions to junctions
    std::unordered_map<uint64_t, uint32_t> edge_lookup;
    edge_lookup.reserve(mesh.edges.size() * 2);
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h0 = mesh.edges[e].halfedge;
        if (h0 == INVALID_INDEX) continue;
        uint32_t u = mesh.from_vertex(h0);
        uint32_t v = mesh.halfedges[h0].to_vertex;
        uint64_t k = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
        edge_lookup[k] = static_cast<uint32_t>(e);
    }

    auto find_mesh_edge = [&](uint32_t u, uint32_t v) -> uint32_t {
        uint64_t k = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
        auto it = edge_lookup.find(k);
        return (it != edge_lookup.end()) ? it->second : INVALID_INDEX;
    };

    std::vector<bool> edge_visited(mesh.edges.size(), false);
    std::vector<CanonicalEdgePath> paths;

    // Directed edge (u, v) -> OrientedEdgeRef
    std::map<std::pair<uint32_t, uint32_t>, OrientedEdgeRef> directed_edge_to_path;

    auto get_edge_patches = [&](uint32_t u, uint32_t v) -> std::pair<uint32_t, uint32_t> {
        uint32_t e = find_mesh_edge(u, v);
        if (e == INVALID_INDEX) return {INVALID_INDEX, INVALID_INDEX};
        uint32_t h0 = mesh.edges[e].halfedge;
        if (h0 == INVALID_INDEX) return {INVALID_INDEX, INVALID_INDEX};
        uint32_t h1 = mesh.halfedges[h0].twin;

        uint32_t f0 = mesh.halfedges[h0].face;
        uint32_t f1 = (h1 != INVALID_INDEX) ? mesh.halfedges[h1].face : INVALID_INDEX;

        int p0 = (f0 != INVALID_INDEX && f0 < nf) ? face_patch[f0] : -1;
        int p1 = (f1 != INVALID_INDEX && f1 < nf) ? face_patch[f1] : -1;

        uint32_t from0 = mesh.from_vertex(h0);
        uint32_t left_p = (from0 == u) ? (p0 >= 0 ? static_cast<uint32_t>(p0) : INVALID_INDEX)
                                       : (p1 >= 0 ? static_cast<uint32_t>(p1) : INVALID_INDEX);
        uint32_t right_p = (from0 == u) ? (p1 >= 0 ? static_cast<uint32_t>(p1) : INVALID_INDEX)
                                        : (p0 >= 0 ? static_cast<uint32_t>(p0) : INVALID_INDEX);
        return {left_p, right_p};
    };

    std::vector<uint32_t> junctions;
    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (is_junction[v]) junctions.push_back(static_cast<uint32_t>(v));
    }
    std::sort(junctions.begin(), junctions.end());

    for (uint32_t junc : junctions) {
        auto nbrs = cut_nbrs[junc];
        std::sort(nbrs.begin(), nbrs.end());

        for (uint32_t nbr : nbrs) {
            uint32_t e = find_mesh_edge(junc, nbr);
            if (e == INVALID_INDEX || edge_visited[e]) continue;

            CanonicalEdgePath path;
            uint32_t path_id = static_cast<uint32_t>(paths.size());
            path.edge_path_id = path_id;
            path.node_start = junc;
            path.path_vertices.push_back(junc);
            path.path_vertices.push_back(nbr);
            edge_visited[e] = true;

            float cur_len = (mesh.vertices[nbr].pos - mesh.vertices[junc].pos).norm();
            size_t sharp_cnt = mesh.edges[e].is_sharp ? 1 : 0;
            size_t seam_cnt = mesh.edges[e].is_seam ? 1 : 0;
            size_t edge_cnt = 1;
            if (mesh.is_boundary_edge(e)) path.is_open_border = true;

            uint32_t prev_v = junc;
            uint32_t curr_v = nbr;
            size_t walk_guard = 0;

            while (curr_v != junc && !is_junction[curr_v] && ++walk_guard <= mesh.vertices.size()) {
                uint32_t next_v = INVALID_INDEX;
                for (uint32_t cand : cut_nbrs[curr_v]) {
                    if (cand != prev_v) {
                        uint32_t cand_e = find_mesh_edge(curr_v, cand);
                        if (cand_e != INVALID_INDEX && !edge_visited[cand_e]) {
                            next_v = cand;
                            break;
                        }
                    }
                }
                if (next_v == INVALID_INDEX) break;

                uint32_t next_e = find_mesh_edge(curr_v, next_v);
                if (next_e != INVALID_INDEX) {
                    edge_visited[next_e] = true;
                    edge_cnt++;
                    if (mesh.edges[next_e].is_sharp) sharp_cnt++;
                    if (mesh.edges[next_e].is_seam) seam_cnt++;
                    if (mesh.is_boundary_edge(next_e)) path.is_open_border = true;
                    cur_len += (mesh.vertices[next_v].pos - mesh.vertices[curr_v].pos).norm();
                }

                path.path_vertices.push_back(next_v);
                prev_v = curr_v;
                curr_v = next_v;
            }

            path.is_sharp = (sharp_cnt * 2 > edge_cnt && edge_cnt >= 2);
            path.is_seam = (seam_cnt * 2 > edge_cnt && edge_cnt >= 2);
            path.node_end = curr_v;
            path.continuous_len = std::max(cur_len, 1e-5f);
            path.integer_divisions = 1;
            uint32_t lp = INVALID_INDEX, rp = INVALID_INDEX;
            for (size_t k = 0; k + 1 < path.path_vertices.size(); ++k) {
                auto [cur_lp, cur_rp] = get_edge_patches(path.path_vertices[k], path.path_vertices[k + 1]);
                if (lp == INVALID_INDEX && cur_lp != INVALID_INDEX) lp = cur_lp;
                if (rp == INVALID_INDEX && cur_rp != INVALID_INDEX) rp = cur_rp;
                if (lp != INVALID_INDEX && rp != INVALID_INDEX) break;
            }
            path.left_patch = lp;
            path.right_patch = rp;

            for (size_t k = 0; k + 1 < path.path_vertices.size(); ++k) {
                uint32_t u_k = path.path_vertices[k];
                uint32_t v_k = path.path_vertices[k + 1];
                directed_edge_to_path[{u_k, v_k}] = { path_id, true };
                directed_edge_to_path[{v_k, u_k}] = { path_id, false };
            }

            paths.push_back(std::move(path));
        }
    }

    // Trace any remaining cut edges (e.g. isolated cycles of size 2 or 3)
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!is_cut_edge[e] || edge_visited[e]) continue;
        uint32_t eh = mesh.edges[e].halfedge;
        uint32_t junc = mesh.from_vertex(eh);
        uint32_t nbr = mesh.halfedges[eh].to_vertex;
        is_junction[junc] = true;

        CanonicalEdgePath path;
        uint32_t path_id = static_cast<uint32_t>(paths.size());
        path.edge_path_id = path_id;
        path.node_start = junc;
        path.path_vertices.push_back(junc);
        path.path_vertices.push_back(nbr);
        edge_visited[e] = true;

        float cur_len = (mesh.vertices[nbr].pos - mesh.vertices[junc].pos).norm();
        size_t edge_cnt = 1;
        size_t sharp_cnt = mesh.edges[e].is_sharp ? 1 : 0;
        size_t seam_cnt = mesh.edges[e].is_seam ? 1 : 0;
        if (mesh.is_boundary_edge(static_cast<uint32_t>(e))) path.is_open_border = true;

        uint32_t prev_v = junc;
        uint32_t curr_v = nbr;
        size_t walk_guard = 0;

        while (curr_v != junc && !is_junction[curr_v] && ++walk_guard <= mesh.vertices.size()) {
            uint32_t next_v = INVALID_INDEX;
            for (uint32_t cand : cut_nbrs[curr_v]) {
                if (cand != prev_v) {
                    uint32_t cand_e = find_mesh_edge(curr_v, cand);
                    if (cand_e != INVALID_INDEX && !edge_visited[cand_e]) {
                        next_v = cand;
                        break;
                    }
                }
            }
            if (next_v == INVALID_INDEX) break;

            uint32_t next_e = find_mesh_edge(curr_v, next_v);
            if (next_e != INVALID_INDEX) {
                edge_visited[next_e] = true;
                edge_cnt++;
                if (mesh.edges[next_e].is_sharp) sharp_cnt++;
                if (mesh.edges[next_e].is_seam) seam_cnt++;
                if (mesh.is_boundary_edge(next_e)) path.is_open_border = true;
                cur_len += (mesh.vertices[next_v].pos - mesh.vertices[curr_v].pos).norm();
            }

            path.path_vertices.push_back(next_v);
            prev_v = curr_v;
            curr_v = next_v;
        }

        path.is_sharp = (sharp_cnt * 2 > edge_cnt);
        path.is_seam = (seam_cnt * 2 > edge_cnt);

        path.node_end = curr_v;
        path.continuous_len = std::max(cur_len, 1e-5f);
        path.integer_divisions = 1;
        uint32_t lp = INVALID_INDEX, rp = INVALID_INDEX;
        for (size_t k = 0; k + 1 < path.path_vertices.size(); ++k) {
            auto [cur_lp, cur_rp] = get_edge_patches(path.path_vertices[k], path.path_vertices[k + 1]);
            if (lp == INVALID_INDEX && cur_lp != INVALID_INDEX) lp = cur_lp;
            if (rp == INVALID_INDEX && cur_rp != INVALID_INDEX) rp = cur_rp;
            if (lp != INVALID_INDEX && rp != INVALID_INDEX) break;
        }
        path.left_patch = lp;
        path.right_patch = rp;

        for (size_t k = 0; k + 1 < path.path_vertices.size(); ++k) {
            uint32_t u_k = path.path_vertices[k];
            uint32_t v_k = path.path_vertices[k + 1];
            directed_edge_to_path[{u_k, v_k}] = { path_id, true };
            directed_edge_to_path[{v_k, u_k}] = { path_id, false };
        }

        paths.push_back(std::move(path));
    }

    // 7. Assemble QuasiQuadPatch boundary loops directly from canonical paths
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

    std::vector<std::vector<OrientedEdgeRef>> patch_bnd_paths(active_patch_count);
    for (size_t ep_id = 0; ep_id < paths.size(); ++ep_id) {
        const auto& ep = paths[ep_id];
        if (ep.left_patch == ep.right_patch) continue; // Skip internal edges

        if (ep.left_patch < static_cast<uint32_t>(active_patch_count)) {
            patch_bnd_paths[ep.left_patch].push_back({static_cast<uint32_t>(ep_id), true});
        }
        if (ep.right_patch < static_cast<uint32_t>(active_patch_count)) {
            patch_bnd_paths[ep.right_patch].push_back({static_cast<uint32_t>(ep_id), false});
        }
    }

    auto find_simple_cycles = [&](const std::vector<OrientedEdgeRef>& bnd_edges) -> std::vector<std::vector<OrientedEdgeRef>> {
        if (bnd_edges.empty()) return {};

        std::unordered_map<uint32_t, std::vector<size_t>> adj;
        for (size_t i = 0; i < bnd_edges.size(); ++i) {
            const auto& ep = paths[bnd_edges[i].edge_path_id];
            uint32_t u = bnd_edges[i].forward ? ep.node_start : ep.node_end;
            adj[u].push_back(i);
        }

        std::vector<bool> used(bnd_edges.size(), false);
        std::vector<std::vector<OrientedEdgeRef>> cycles;

        for (size_t start_idx = 0; start_idx < bnd_edges.size(); ++start_idx) {
            if (used[start_idx]) continue;

            std::vector<size_t> walk;
            std::vector<uint32_t> node_history;
            std::unordered_map<uint32_t, size_t> node_to_walk_pos;

            size_t cur_e = start_idx;
            while (true) {
                used[cur_e] = true;
                walk.push_back(cur_e);

                const auto& ep = paths[bnd_edges[cur_e].edge_path_id];
                uint32_t u = bnd_edges[cur_e].forward ? ep.node_start : ep.node_end;
                uint32_t v = bnd_edges[cur_e].forward ? ep.node_end : ep.node_start;

                node_history.push_back(u);
                node_to_walk_pos[u] = walk.size() - 1;

                auto it = node_to_walk_pos.find(v);
                if (it != node_to_walk_pos.end()) {
                    // A simple cycle is closed from it->second to walk.size() - 1
                    size_t c_start = it->second;
                    std::vector<OrientedEdgeRef> cyc;
                    for (size_t k = c_start; k < walk.size(); ++k) {
                        cyc.push_back(bnd_edges[walk[k]]);
                    }
                    cycles.push_back(std::move(cyc));

                    for (size_t k = c_start; k < walk.size(); ++k) {
                        node_to_walk_pos.erase(node_history[k]);
                    }
                    walk.resize(c_start);
                    node_history.resize(c_start);
                    if (walk.empty()) break;
                    const auto& last_ep = paths[bnd_edges[walk.back()].edge_path_id];
                    v = bnd_edges[walk.back()].forward ? last_ep.node_end : last_ep.node_start;
                }

                size_t next_e = INVALID_INDEX;
                auto adj_it = adj.find(v);
                if (adj_it != adj.end()) {
                    for (size_t cand_e : adj_it->second) {
                        if (!used[cand_e]) {
                            next_e = cand_e;
                            break;
                        }
                    }
                }

                if (next_e == INVALID_INDEX) {
                    break;
                }
                cur_e = next_e;
            }
        }
        return cycles;
    };

    size_t loops_missing = 0, unclosed_count = 0;

    for (int p = 0; p < active_patch_count; ++p) {
        if (patches[p].member_faces.empty() || patch_bnd_paths[p].empty()) {
            loops_missing++;
            continue;
        }

        auto cycles = find_simple_cycles(patch_bnd_paths[p]);
        if (cycles.empty()) {
            loops_missing++;
            continue;
        }

        // Pick cycle with largest perimeter
        size_t best_c = 0;
        float best_len = -1.0f;
        for (size_t c = 0; c < cycles.size(); ++c) {
            float len = 0.0f;
            for (const auto& eref : cycles[c]) {
                len += paths[eref.edge_path_id].continuous_len;
            }
            if (len > best_len) {
                best_len = len;
                best_c = c;
            }
        }

        patches[p].boundary_loop = std::move(cycles[best_c]);
    }

    // Ensure every patch boundary loop with >= 4 mesh vertices has at least 4 canonical paths
    // by splitting the longest path(s) in-place across both adjacent patches
    auto replace_edge_in_loop = [](std::vector<OrientedEdgeRef>& loop, uint32_t old_id, uint32_t new_id) {
        std::vector<OrientedEdgeRef> updated;
        updated.reserve(loop.size() + 1);
        for (const auto& eref : loop) {
            if (eref.edge_path_id == old_id) {
                if (eref.forward) {
                    updated.push_back({old_id, true});
                    updated.push_back({new_id, true});
                } else {
                    updated.push_back({new_id, false});
                    updated.push_back({old_id, false});
                }
            } else {
                updated.push_back(eref);
            }
        }
        loop = std::move(updated);
    };

    auto split_canonical_path = [&](uint32_t ep1_id) -> bool {
        if (ep1_id >= paths.size() || paths[ep1_id].path_vertices.size() < 3) return false;
        uint32_t ep2_id = static_cast<uint32_t>(paths.size());
        CanonicalEdgePath ep1 = paths[ep1_id];
        CanonicalEdgePath ep2 = ep1;

        size_t mid_idx = ep1.path_vertices.size() / 2;
        uint32_t mid_v = ep1.path_vertices[mid_idx];
        is_junction[mid_v] = true;
        junctions.push_back(mid_v);

        ep1.path_vertices.assign(paths[ep1_id].path_vertices.begin(), paths[ep1_id].path_vertices.begin() + mid_idx + 1);
        ep1.node_end = mid_v;
        float len1 = 0.0f;
        for (size_t i = 0; i + 1 < ep1.path_vertices.size(); ++i) {
            len1 += (mesh.vertices[ep1.path_vertices[i + 1]].pos - mesh.vertices[ep1.path_vertices[i]].pos).norm();
        }
        ep1.continuous_len = std::max(len1, 1e-5f);

        ep2.edge_path_id = ep2_id;
        ep2.path_vertices.assign(paths[ep1_id].path_vertices.begin() + mid_idx, paths[ep1_id].path_vertices.end());
        ep2.node_start = mid_v;
        float len2 = 0.0f;
        for (size_t i = 0; i + 1 < ep2.path_vertices.size(); ++i) {
            len2 += (mesh.vertices[ep2.path_vertices[i + 1]].pos - mesh.vertices[ep2.path_vertices[i]].pos).norm();
        }
        ep2.continuous_len = std::max(len2, 1e-5f);

        uint32_t lp = ep1.left_patch;
        uint32_t rp = ep1.right_patch;
        paths[ep1_id] = std::move(ep1);
        paths.push_back(std::move(ep2));

        if (lp < static_cast<uint32_t>(active_patch_count)) {
            replace_edge_in_loop(patches[lp].boundary_loop, ep1_id, ep2_id);
        }
        if (rp < static_cast<uint32_t>(active_patch_count) && rp != lp) {
            replace_edge_in_loop(patches[rp].boundary_loop, ep1_id, ep2_id);
        }
        return true;
    };

    // Count how many patch boundary_loops use each canonical path
    std::vector<int> path_loop_uses(paths.size(), 0);
    for (int p = 0; p < active_patch_count; ++p) {
        for (const auto& eref : patches[p].boundary_loop) {
            if (eref.edge_path_id < path_loop_uses.size()) {
                path_loop_uses[eref.edge_path_id]++;
            }
        }
    }

    {
        // Diagnostic: a canonical path should normally appear in TWO patch loops (one per
        // side). Paths used once are fine only when they lie on the model's open border;
        // any other "used once" path means the neighbouring patch is missing it in its
        // loop - the source of unstitched borders (holes).
        size_t used_once = 0, used_once_not_open = 0, not_open_examples = 0;
        for (size_t i = 0; i < paths.size() && i < path_loop_uses.size(); ++i) {
            if (path_loop_uses[i] <= 1) {
                used_once++;
                if (!paths[i].is_open_border) {
                    used_once_not_open++;
                    if (not_open_examples < 5) {
                        std::cout << "    [stage4/unshared] path " << i
                                  << " len=" << paths[i].continuous_len
                                  << " loop_uses=" << path_loop_uses[i]
                                  << " verts=" << paths[i].path_vertices.size() << std::endl;
                        not_open_examples++;
                    }
                }
            }
        }
        std::cout << "[stage4] paths_used_once=" << used_once
                  << " not_open_border=" << used_once_not_open
                  << " (of " << paths.size() << ")" << std::endl;
    }

    for (int p = 0; p < active_patch_count; ++p) {
        if (patches[p].boundary_loop.empty()) continue;
        // NOTE: loops whose canonical paths are used by a single patch used to be cleared
        // here. Those are mostly patches sitting on the model's OPEN boundary, where a
        // path legitimately belongs to exactly one loop - clearing punched holes along
        // open borders (boundary edges + stray components). Short loops are emitted as a
        // fan by extract_patch_quads instead.

        while (patches[p].boundary_loop.size() >= 2 && patches[p].boundary_loop.size() < 4) {
            size_t best_k = INVALID_INDEX;
            size_t max_verts = 2;
            float max_len = -1.0f;

            for (size_t k = 0; k < patches[p].boundary_loop.size(); ++k) {
                uint32_t ep_id = patches[p].boundary_loop[k].edge_path_id;
                const auto& ep = paths[ep_id];
                bool is_shared = (ep_id >= path_loop_uses.size() || path_loop_uses[ep_id] >= 2);
                if (ep.path_vertices.size() >= 3 && is_shared) {
                    if (ep.continuous_len > max_len || (ep.continuous_len == max_len && ep.path_vertices.size() > max_verts)) {
                        max_len = ep.continuous_len;
                        max_verts = ep.path_vertices.size();
                        best_k = k;
                    }
                }
            }

            if (best_k == INVALID_INDEX) {
                break;
            }
            uint32_t old_eid = patches[p].boundary_loop[best_k].edge_path_id;
            if (!split_canonical_path(old_eid)) {
                break;
            }
            path_loop_uses.push_back(old_eid < path_loop_uses.size() ? path_loop_uses[old_eid] : 2);
        }
    }

    std::map<size_t, size_t> loop_size_hist;
    for (int p = 0; p < active_patch_count; ++p) {
        loop_size_hist[patches[p].boundary_loop.size()]++;
    }
    std::cout << "[stage4] loop sizes:";
    for (const auto& [sz, cnt] : loop_size_hist) {
        std::cout << " [" << sz << "]:" << cnt;
    }
    std::cout << std::endl;

    std::cout << "[stage4] junctions=" << junctions.size()
              << " canonical_paths=" << paths.size()
              << " patches=" << active_patch_count
              << " patches_without_loop=" << loops_missing
              << " unclosed_loops=" << unclosed_count
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