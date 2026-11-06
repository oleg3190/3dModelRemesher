#include "stages/stage0_prep.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <cmath>
#include <iostream>

namespace qremesh {

struct GridCoordHash {
    size_t operator()(const std::array<int64_t, 3>& c) const {
        return (static_cast<size_t>(c[0]) * 73856093) ^
               (static_cast<size_t>(c[1]) * 19349663) ^
               (static_cast<size_t>(c[2]) * 83492791);
    }
};

bool Stage0Prep::weld_vertices(
    const float* in_pos, uint32_t in_nv,
    const uint32_t* in_tris, uint32_t in_nt,
    const uint16_t* in_mats,
    float weld_eps,
    std::vector<Vec3f>& out_pos,
    std::vector<uint32_t>& out_tris,
    std::vector<uint16_t>& out_mats,
    std::vector<uint32_t>& old_to_new_v
) {
    if (in_nv == 0 || in_nt == 0) return false;

    old_to_new_v.assign(in_nv, INVALID_INDEX);
    out_pos.clear();
    out_tris.clear();
    out_mats.clear();

    float inv_cell = (weld_eps > 1e-8f) ? (1.0f / weld_eps) : 1e6f;
    std::unordered_map<std::array<int64_t, 3>, std::vector<uint32_t>, GridCoordHash> grid;

    for (uint32_t i = 0; i < in_nv; ++i) {
        Vec3f p(in_pos[3 * i + 0], in_pos[3 * i + 1], in_pos[3 * i + 2]);
        if (!std::isfinite(p.x()) || !std::isfinite(p.y()) || !std::isfinite(p.z())) {
            continue;
        }

        int64_t gx = static_cast<int64_t>(std::floor(p.x() * inv_cell));
        int64_t gy = static_cast<int64_t>(std::floor(p.y() * inv_cell));
        int64_t gz = static_cast<int64_t>(std::floor(p.z() * inv_cell));

        uint32_t matched_idx = INVALID_INDEX;
        for (int64_t dx = -1; dx <= 1 && matched_idx == INVALID_INDEX; ++dx) {
            for (int64_t dy = -1; dy <= 1 && matched_idx == INVALID_INDEX; ++dy) {
                for (int64_t dz = -1; dz <= 1 && matched_idx == INVALID_INDEX; ++dz) {
                    auto it = grid.find({gx + dx, gy + dy, gz + dz});
                    if (it != grid.end()) {
                        for (uint32_t candidate : it->second) {
                            if ((out_pos[candidate] - p).squaredNorm() <= weld_eps * weld_eps) {
                                matched_idx = candidate;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (matched_idx == INVALID_INDEX) {
            matched_idx = static_cast<uint32_t>(out_pos.size());
            out_pos.push_back(p);
            grid[{gx, gy, gz}].push_back(matched_idx);
        }
        old_to_new_v[i] = matched_idx;
    }

    // Filter triangles and remove degenerates
    for (uint32_t f = 0; f < in_nt; ++f) {
        uint32_t v0 = in_tris[3 * f + 0];
        uint32_t v1 = in_tris[3 * f + 1];
        uint32_t v2 = in_tris[3 * f + 2];

        if (v0 >= in_nv || v1 >= in_nv || v2 >= in_nv) continue;
        uint32_t nv0 = old_to_new_v[v0];
        uint32_t nv1 = old_to_new_v[v1];
        uint32_t nv2 = old_to_new_v[v2];

        if (nv0 == INVALID_INDEX || nv1 == INVALID_INDEX || nv2 == INVALID_INDEX) continue;
        if (nv0 == nv1 || nv1 == nv2 || nv2 == nv0) continue;

        float area = triangle_area(out_pos[nv0], out_pos[nv1], out_pos[nv2]);
        if (area <= 1e-12f) continue;

        out_tris.push_back(nv0);
        out_tris.push_back(nv1);
        out_tris.push_back(nv2);
        if (in_mats) {
            out_mats.push_back(in_mats[f]);
        }
    }

    return !out_tris.empty();
}

bool Stage0Prep::fix_manifold_and_orient(
    std::vector<Vec3f>& positions,
    std::vector<uint32_t>& triangles,
    std::vector<uint16_t>& materials
) {
    uint32_t nt = static_cast<uint32_t>(triangles.size() / 3);
    if (nt == 0) return false;

    // 1. Discard duplicate triangles with identical vertex sets
    std::vector<bool> face_valid(nt, true);
    auto make_tri_key = [](uint32_t a, uint32_t b, uint32_t c) -> uint64_t {
        if (a > b) std::swap(a, b);
        if (b > c) std::swap(b, c);
        if (a > b) std::swap(a, b);
        return (static_cast<uint64_t>(a) * 73856093) ^
               (static_cast<uint64_t>(b) * 19349663) ^
               (static_cast<uint64_t>(c) * 83492791);
    };
    std::unordered_map<uint64_t, std::vector<uint32_t>> tri_buckets;
    for (uint32_t f = 0; f < nt; ++f) {
        uint32_t v0 = triangles[3 * f + 0];
        uint32_t v1 = triangles[3 * f + 1];
        uint32_t v2 = triangles[3 * f + 2];
        tri_buckets[make_tri_key(v0, v1, v2)].push_back(f);
    }
    for (const auto& [k, fl] : tri_buckets) {
        if (fl.size() > 1) {
            for (size_t i = 1; i < fl.size(); ++i) {
                face_valid[fl[i]] = false;
            }
        }
    }

    // 2. Count edge incidences for valid faces
    std::unordered_map<uint64_t, std::vector<uint32_t>> edge_to_faces;
    for (uint32_t f = 0; f < nt; ++f) {
        if (!face_valid[f]) continue;
        uint32_t v[3] = { triangles[3 * f + 0], triangles[3 * f + 1], triangles[3 * f + 2] };
        for (int i = 0; i < 3; ++i) {
            uint32_t u0 = std::min(v[i], v[(i + 1) % 3]);
            uint32_t u1 = std::max(v[i], v[(i + 1) % 3]);
            uint64_t key = (static_cast<uint64_t>(u0) << 32) | u1;
            edge_to_faces[key].push_back(f);
        }
    }

    // Discard non-manifold faces (where edge has > 2 faces)
    // keep-first-2 is ARBITRARY: on a "book" edge (4 faces) it may keep two pages that are
    // a twisted sliver pair, which downstream becomes a degenerate patch (S33/T3: a cleaned
    // input lifted median SJ 0.626 -> 0.707 even though qremesh already reached nm=0). The
    // fix is to keep the two faces whose normals AGREE most (the true manifold surface) and
    // drop the rest. Env QREMESH_NM_KEEP_BEST selects the smarter rule (default off -> keeps
    // prior behaviour bit-identical).
    const bool nm_keep_best = std::getenv("QREMESH_NM_KEEP_BEST") != nullptr;
    auto tri_normal = [&](uint32_t f) -> Vec3f {
        uint32_t a = triangles[3 * f], b = triangles[3 * f + 1], c = triangles[3 * f + 2];
        Vec3f n = (positions[b] - positions[a]).cross(positions[c] - positions[a]);
        float l = n.norm();
        return (l > 1e-12f) ? n * (1.0f / l) : Vec3f(0, 0, 0);
    };
    if (!nm_keep_best) {
        for (const auto& pair : edge_to_faces) {
            if (pair.second.size() <= 2) continue;
            for (size_t k = 2; k < pair.second.size(); ++k)
                face_valid[pair.second[k]] = false;
        }
    } else {
        // Two-phase, NO mutation during selection so the choice is order-independent and
        // coherent. Phase 1: for every non-manifold edge pick the best-2 (most coplanar) face
        // PAIR and record, per face, how many of its book edges REJECT it. Phase 2: drop a
        // face only if it was rejected by any non-manifold edge it belongs to (intersection of
        // the per-edge keep sets). This removes twisted sliver pages without orphaning faces
        // that two edges disagree about (which the naive single-pass keep-best-2 caused ->
        // disconnected islands: 1 -> 4 components).
        std::unordered_map<uint32_t, int> face_reject;   // faceId -> #edges excluding it
        for (const auto& pair : edge_to_faces) {
            if (pair.second.size() <= 2) continue;
            const auto& fl = pair.second;
            int best_i = -1, best_j = -1; float best_dot = -2.0f;
            for (size_t a = 0; a < fl.size(); ++a) {
                if (!face_valid[fl[a]]) continue;
                Vec3f na = tri_normal(fl[a]);
                for (size_t b = a + 1; b < fl.size(); ++b) {
                    if (!face_valid[fl[b]]) continue;
                    float d = na.dot(tri_normal(fl[b]));
                    if (d > best_dot) { best_dot = d; best_i = (int)fl[a]; best_j = (int)fl[b]; }
                }
            }
            if (best_i < 0) continue;   // <2 valid faces, nothing to exclude
            for (size_t f : fl)
                if ((int)f != best_i && (int)f != best_j) face_reject[(uint32_t)f]++;
        }
        for (const auto& kv : face_reject) face_valid[kv.first] = false;
    }

    // 3. BFS Normal orientation with conflict pruning
    std::vector<bool> visited(nt, false);
    for (uint32_t start_f = 0; start_f < nt; ++start_f) {
        if (!face_valid[start_f] || visited[start_f]) continue;

        std::queue<uint32_t> q;
        q.push(start_f);
        visited[start_f] = true;

        while (!q.empty()) {
            uint32_t cur = q.front();
            q.pop();
            if (!face_valid[cur]) continue;

            uint32_t cv[3] = { triangles[3 * cur + 0], triangles[3 * cur + 1], triangles[3 * cur + 2] };
            for (int i = 0; i < 3; ++i) {
                uint32_t ca = cv[i];
                uint32_t cb = cv[(i + 1) % 3];
                uint32_t u0 = std::min(ca, cb);
                uint32_t u1 = std::max(ca, cb);
                uint64_t key = (static_cast<uint64_t>(u0) << 32) | u1;

                auto it = edge_to_faces.find(key);
                if (it != edge_to_faces.end()) {
                    for (uint32_t nbr : it->second) {
                        if (nbr != cur && face_valid[nbr]) {
                            uint32_t nv[3] = { triangles[3 * nbr + 0], triangles[3 * nbr + 1], triangles[3 * nbr + 2] };
                            bool same_direction = false;
                            for (int j = 0; j < 3; ++j) {
                                if (nv[j] == ca && nv[(j + 1) % 3] == cb) {
                                    same_direction = true;
                                    break;
                                }
                            }
                            if (!visited[nbr]) {
                                if (same_direction) {
                                    std::swap(triangles[3 * nbr + 1], triangles[3 * nbr + 2]);
                                }
                                visited[nbr] = true;
                                q.push(nbr);
                            } else {
                                if (same_direction) {
                                    // Non-orientable / Möbius seam conflict: drop current face
                                    face_valid[cur] = false;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Pack valid triangles
    std::vector<uint32_t> clean_tris;
    std::vector<uint16_t> clean_mats;
    clean_tris.reserve(triangles.size());
    if (!materials.empty()) clean_mats.reserve(materials.size());

    for (uint32_t f = 0; f < nt; ++f) {
        if (face_valid[f]) {
            clean_tris.push_back(triangles[3 * f + 0]);
            clean_tris.push_back(triangles[3 * f + 1]);
            clean_tris.push_back(triangles[3 * f + 2]);
            if (f < materials.size()) {
                clean_mats.push_back(materials[f]);
            }
        }
    }

    // 4. Split non-manifold vertices (where vertex link has multiple connected components)
    {
        uint32_t clean_nt = static_cast<uint32_t>(clean_tris.size() / 3);
        std::vector<std::vector<uint32_t>> v_to_faces(positions.size());
        for (uint32_t f = 0; f < clean_nt; ++f) {
            for (int k = 0; k < 3; ++k) {
                uint32_t v = clean_tris[3 * f + k];
                if (v < v_to_faces.size()) {
                    v_to_faces[v].push_back(f);
                }
            }
        }

        size_t original_v_count = positions.size();
        size_t split_count = 0;
        for (size_t v = 0; v < original_v_count; ++v) {
            const auto& inc_faces = v_to_faces[v];
            if (inc_faces.size() <= 1) continue;

            // Map each neighbor vertex to the faces containing the edge (v, neighbor)
            std::unordered_map<uint32_t, std::vector<uint32_t>> edge_arms;
            for (uint32_t f : inc_faces) {
                uint32_t fv[3] = { clean_tris[3 * f + 0], clean_tris[3 * f + 1], clean_tris[3 * f + 2] };
                for (int k = 0; k < 3; ++k) {
                    if (fv[k] != v) {
                        edge_arms[fv[k]].push_back(f);
                    }
                }
            }

            // Build adjacency between faces in inc_faces
            std::unordered_map<uint32_t, std::vector<uint32_t>> face_adj;
            for (const auto& [nbr, flist] : edge_arms) {
                if (flist.size() == 2) {
                    face_adj[flist[0]].push_back(flist[1]);
                    face_adj[flist[1]].push_back(flist[0]);
                }
            }

            // Find connected components among inc_faces
            std::unordered_set<uint32_t> visited_faces;
            std::vector<std::vector<uint32_t>> components;

            for (uint32_t f : inc_faces) {
                if (visited_faces.count(f)) continue;
                std::vector<uint32_t> comp;
                std::queue<uint32_t> q;
                q.push(f);
                visited_faces.insert(f);

                while (!q.empty()) {
                    uint32_t cur = q.front();
                    q.pop();
                    comp.push_back(cur);

                    for (uint32_t nxt : face_adj[cur]) {
                        if (!visited_faces.count(nxt)) {
                            visited_faces.insert(nxt);
                            q.push(nxt);
                        }
                    }
                }
                components.push_back(std::move(comp));
            }

            if (components.size() > 1) {
                split_count += (components.size() - 1);
                for (size_t c = 1; c < components.size(); ++c) {
                    uint32_t new_v = static_cast<uint32_t>(positions.size());
                    positions.push_back(positions[v]);

                    for (uint32_t f : components[c]) {
                        for (int k = 0; k < 3; ++k) {
                            if (clean_tris[3 * f + k] == v) {
                                clean_tris[3 * f + k] = new_v;
                            }
                        }
                    }
                }
            }
        }
        std::cout << "[stage0] non-manifold vertices split: " << split_count 
                  << " (verts: " << original_v_count << " -> " << positions.size() << ")" << std::endl;

        // Remove tiny disconnected floating triangle burrs (< 25 faces) detached by pinch splitting
        if (clean_nt > 100) {
            std::unordered_map<uint64_t, std::vector<uint32_t>> edge_to_f;
            edge_to_f.reserve(clean_nt * 2);
            for (uint32_t f = 0; f < clean_nt; ++f) {
                uint32_t fv[3] = { clean_tris[3 * f + 0], clean_tris[3 * f + 1], clean_tris[3 * f + 2] };
                for (int k = 0; k < 3; ++k) {
                    uint32_t u = std::min(fv[k], fv[(k + 1) % 3]);
                    uint32_t v = std::max(fv[k], fv[(k + 1) % 3]);
                    edge_to_f[(static_cast<uint64_t>(u) << 32) | v].push_back(f);
                }
            }

            std::vector<int> f_comp(clean_nt, -1);
            std::vector<size_t> comp_sizes;
            size_t max_comp_sz = 0;

            for (uint32_t f = 0; f < clean_nt; ++f) {
                if (f_comp[f] >= 0) continue;
                int cid = static_cast<int>(comp_sizes.size());
                size_t sz = 0;
                std::queue<uint32_t> fq;
                fq.push(f);
                f_comp[f] = cid;
                while (!fq.empty()) {
                    uint32_t cf = fq.front();
                    fq.pop();
                    sz++;
                    uint32_t fv[3] = { clean_tris[3 * cf + 0], clean_tris[3 * cf + 1], clean_tris[3 * cf + 2] };
                    for (int k = 0; k < 3; ++k) {
                        uint32_t u = std::min(fv[k], fv[(k + 1) % 3]);
                        uint32_t v = std::max(fv[k], fv[(k + 1) % 3]);
                        for (uint32_t nf_idx : edge_to_f[(static_cast<uint64_t>(u) << 32) | v]) {
                            if (f_comp[nf_idx] < 0) {
                                f_comp[nf_idx] = cid;
                                fq.push(nf_idx);
                            }
                        }
                    }
                }
                comp_sizes.push_back(sz);
                if (sz > max_comp_sz) max_comp_sz = sz;
            }

            if (comp_sizes.size() > 1 && max_comp_sz > 100) {
                std::vector<uint32_t> filtered_tris;
                std::vector<uint16_t> filtered_mats;
                filtered_tris.reserve(clean_tris.size());
                if (!clean_mats.empty()) filtered_mats.reserve(clean_mats.size());
                for (uint32_t f = 0; f < clean_nt; ++f) {
                    if (comp_sizes[f_comp[f]] >= 25) {
                        filtered_tris.push_back(clean_tris[3 * f + 0]);
                        filtered_tris.push_back(clean_tris[3 * f + 1]);
                        filtered_tris.push_back(clean_tris[3 * f + 2]);
                        if (f < clean_mats.size()) filtered_mats.push_back(clean_mats[f]);
                    }
                }
                clean_tris = std::move(filtered_tris);
                clean_mats = std::move(filtered_mats);
            }
        }
    }

    triangles = std::move(clean_tris);
    materials = std::move(clean_mats);
    return !triangles.empty();
}

void Stage0Prep::fill_small_holes(HalfEdgeMesh& mesh, float max_hole_perimeter) {
    // Find boundary loops
    std::vector<bool> h_visited(mesh.halfedges.size(), false);

    for (size_t h_idx = 0; h_idx < mesh.halfedges.size(); ++h_idx) {
        if (!mesh.is_valid_halfedge(static_cast<uint32_t>(h_idx))) continue;
        if (h_visited[h_idx]) continue;
        if (mesh.halfedges[h_idx].twin != INVALID_INDEX) continue; // Not boundary

        // Walk boundary loop
        std::vector<uint32_t> loop_v;
        uint32_t curr = static_cast<uint32_t>(h_idx);
        float perimeter = 0.0f;
        bool closed = false;

        for (size_t step = 0; step < 1000; ++step) {
            h_visited[curr] = true;
            uint32_t u = mesh.from_vertex(curr);
            uint32_t v = mesh.halfedges[curr].to_vertex;
            loop_v.push_back(u);
            perimeter += (mesh.vertices[u].pos - mesh.vertices[v].pos).norm();

            // Find next boundary halfedge from v
            uint32_t next_bnd = INVALID_INDEX;
            for (uint32_t out_h : mesh.get_vertex_outgoing_halfedges(v)) {
                if (mesh.halfedges[out_h].twin == INVALID_INDEX) {
                    next_bnd = out_h;
                    break;
                }
            }

            if (next_bnd == INVALID_INDEX || next_bnd == h_idx) {
                if (next_bnd == h_idx) closed = true;
                break;
            }
            curr = next_bnd;
        }

        if (closed && loop_v.size() >= 3 && perimeter <= max_hole_perimeter) {
            // Fan triangulation of small hole
            uint32_t v0 = loop_v[0];
            for (size_t k = 1; k + 1 < loop_v.size(); ++k) {
                mesh.add_face({v0, loop_v[k + 1], loop_v[k]});
            }
        }
    }
}

struct QuadricMatrix {
    Mat3d A = Mat3d::Zero();
    Vec3d b = Vec3d::Zero();
    double c = 0.0;

    void add_plane(const Vec3f& normal, const Vec3f& point, double weight = 1.0) {
        Vec3d n = normal.cast<double>().normalized();
        double d = -n.dot(point.cast<double>());
        A += weight * (n * n.transpose());
        b += weight * (d * n);
        c += weight * (d * d);
    }

    double evaluate(const Vec3d& p) const {
        return p.dot(A * p) + 2.0 * b.dot(p) + c;
    }
};

void Stage0Prep::simplify_qem(HalfEdgeMesh& mesh, uint32_t target_face_count) {
    if (mesh.faces.size() <= target_face_count) return;

    // Compute vertex quadrics
    std::vector<QuadricMatrix> v_quadrics(mesh.vertices.size());
    for (size_t f = 0; f < mesh.faces.size(); ++f) {
        if (!mesh.is_valid_face(static_cast<uint32_t>(f))) continue;
        Vec3f fn = mesh.compute_face_normal(static_cast<uint32_t>(f));
        auto f_v = mesh.get_face_vertices(static_cast<uint32_t>(f));
        if (f_v.size() >= 3) {
            float area = mesh.compute_face_area(static_cast<uint32_t>(f));
            for (uint32_t v : f_v) {
                v_quadrics[v].add_plane(fn, mesh.vertices[f_v[0]].pos, area);
            }
        }
    }

    // Heavy penalty for sharp edges and boundary edges
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.edges[e].is_sharp || mesh.edges[e].is_seam || mesh.is_boundary_edge(static_cast<uint32_t>(e))) {
            uint32_t h = mesh.edges[e].halfedge;
            uint32_t u = mesh.from_vertex(h);
            uint32_t v = mesh.halfedges[h].to_vertex;
            Vec3f edge_dir = (mesh.vertices[v].pos - mesh.vertices[u].pos).normalized();
            Vec3f n = mesh.vertices[u].pos.cross(edge_dir).normalized();
            v_quadrics[u].add_plane(n, mesh.vertices[u].pos, 1e6);
            v_quadrics[v].add_plane(n, mesh.vertices[v].pos, 1e6);
        }
    }

    struct CollapseCandidate {
        uint32_t edge_idx;
        double cost;
        Vec3f best_pt;
        uint32_t version;

        bool operator>(const CollapseCandidate& o) const {
            return cost > o.cost;
        }
    };

    std::vector<uint32_t> edge_version(mesh.edges.size(), 0);
    std::priority_queue<CollapseCandidate, std::vector<CollapseCandidate>, std::greater<CollapseCandidate>> pq;

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        if (mesh.edges[e].is_sharp || mesh.edges[e].is_seam) continue;

        uint32_t h = mesh.edges[e].halfedge;
        uint32_t u = mesh.from_vertex(h);
        uint32_t v = mesh.halfedges[h].to_vertex;

        QuadricMatrix q_sum = v_quadrics[u];
        q_sum.A += v_quadrics[v].A;
        q_sum.b += v_quadrics[v].b;
        q_sum.c += v_quadrics[v].c;

        Vec3f mid = (mesh.vertices[u].pos + mesh.vertices[v].pos) * 0.5f;
        double cost = q_sum.evaluate(mid.cast<double>());
        pq.push({ static_cast<uint32_t>(e), cost, mid, edge_version[e] });
    }

    uint32_t cur_faces = static_cast<uint32_t>(mesh.faces.size());
    while (!pq.empty() && cur_faces > target_face_count) {
        auto cand = pq.top();
        pq.pop();

        if (!mesh.is_valid_edge(cand.edge_idx)) continue;
        if (cand.version != edge_version[cand.edge_idx]) continue;
        if (mesh.edges[cand.edge_idx].is_sharp || mesh.edges[cand.edge_idx].is_seam) continue;

        uint32_t h = mesh.edges[cand.edge_idx].halfedge;
        if (!mesh.is_valid_halfedge(h)) continue;
        uint32_t u = mesh.from_vertex(h);
        uint32_t v = mesh.halfedges[h].to_vertex;

        if (mesh.collapse_edge(cand.edge_idx, cand.best_pt)) {
            // cur_faces drift guard: a collapse removes 0..2 faces depending on
            // boundary/non-manifold context; subtracting a fixed 2 undercounts and lets
            // the loop run past the live face count, then dereference a deleted halfedge
            // (segfault on dense inputs). Re-count live faces periodically.
            static int rc_ctr = 0;
            if ((++rc_ctr & 0xFF) == 0) {
                uint32_t live = 0;
                for (uint32_t f = 0; f < (uint32_t)mesh.faces.size(); ++f)
                    if (mesh.is_valid_face(f)) ++live;
                cur_faces = live;
            } else if (cur_faces >= 2) {
                cur_faces -= 2;
            }

            // Merge quadric to surviving vertex v
            v_quadrics[v].A += v_quadrics[u].A;
            v_quadrics[v].b += v_quadrics[u].b;
            v_quadrics[v].c += v_quadrics[u].c;

            // Push updated costs for incident edges around v
            for (uint32_t out_h : mesh.get_vertex_outgoing_halfedges(v)) {
                uint32_t adj_e = mesh.halfedges[out_h].edge;
                if (!mesh.is_valid_edge(adj_e) || mesh.edges[adj_e].is_sharp || mesh.edges[adj_e].is_seam) continue;
                uint32_t w = mesh.halfedges[out_h].to_vertex;

                QuadricMatrix q_adj = v_quadrics[v];
                q_adj.A += v_quadrics[w].A;
                q_adj.b += v_quadrics[w].b;
                q_adj.c += v_quadrics[w].c;

                Vec3f adj_mid = (mesh.vertices[v].pos + mesh.vertices[w].pos) * 0.5f;
                double adj_cost = q_adj.evaluate(adj_mid.cast<double>());
                edge_version[adj_e]++;
                pq.push({ adj_e, adj_cost, adj_mid, edge_version[adj_e] });
            }
        }
    }

    mesh.compact();
}

int Stage0Prep::detect_symmetry_axis(const HalfEdgeMesh& mesh, const BVHTree& bvh, float tolerance) {
    if (mesh.vertices.empty()) return -1;

    // Sample up to 256 vertices deterministically across the mesh
    size_t nv = mesh.vertices.size();
    size_t step = std::max<size_t>(1, nv / 256);

    int best_axis = -1;
    float best_rms = std::numeric_limits<float>::infinity();

    for (int axis = 0; axis < 3; ++axis) {
        double sum_sq = 0.0;
        size_t count = 0;
        bool has_pos = false;
        bool has_neg = false;

        for (size_t i = 0; i < nv; i += step) {
            if (mesh.vertices[i].is_deleted) continue;
            Vec3f p = mesh.vertices[i].pos;
            if (p[axis] > tolerance) has_pos = true;
            if (p[axis] < -tolerance) has_neg = true;

            p[axis] = -p[axis];
            Vec3f cp, n;
            float d2 = 0.0f;
            if (bvh.closest_point(p, cp, n, d2)) {
                sum_sq += d2;
                count++;
            }
        }

        if (count > 0 && has_pos && has_neg) {
            float rms = static_cast<float>(std::sqrt(sum_sq / static_cast<double>(count)));
            if (rms <= tolerance * 2.5f && rms < best_rms) {
                best_rms = rms;
                best_axis = axis;
            }
        }
    }

    return best_axis;
}

void Stage0Prep::slice_symmetry(HalfEdgeMesh& mesh, int axis, float tolerance) {
    if (axis < 0 || axis > 2) return;

    // First, split any edge that crosses the symmetry plane
    size_t initial_edges = mesh.edges.size();
    for (size_t e = 0; e < initial_edges; ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h = mesh.edges[e].halfedge;
        uint32_t u = mesh.from_vertex(h);
        uint32_t v = mesh.halfedges[h].to_vertex;

        float xu = mesh.vertices[u].pos[axis];
        float xv = mesh.vertices[v].pos[axis];

        // Edge crosses from positive to negative
        if ((xu > tolerance && xv < -tolerance) || (xu < -tolerance && xv > tolerance)) {
            float t = xu / (xu - xv);
            t = clampf(t, 0.001f, 0.999f);
            Vec3f cut_p = mesh.vertices[u].pos + t * (mesh.vertices[v].pos - mesh.vertices[u].pos);
            cut_p[axis] = 0.0f;

            uint32_t new_v = mesh.split_edge(static_cast<uint32_t>(e), cut_p);
            if (new_v != INVALID_INDEX) {
                mesh.vertices[new_v].pos[axis] = 0.0f;
                mesh.vertices[new_v].is_on_symmetry_plane = true;
                mesh.vertices[new_v].is_sharp = true;
            }
        }
    }

    // Mark vertices on or near plane
    for (auto& v : mesh.vertices) {
        if (v.is_deleted) continue;
        if (std::abs(v.pos[axis]) <= tolerance) {
            v.pos[axis] = 0.0f;
            v.is_on_symmetry_plane = true;
            v.is_sharp = true;
        } else if (v.pos[axis] < 0.0f) {
            v.is_deleted = true; // Drop negative side
        }
    }

    // Mark faces containing deleted vertices as deleted
    for (size_t f = 0; f < mesh.faces.size(); ++f) {
        if (!mesh.is_valid_face(static_cast<uint32_t>(f))) continue;
        auto f_v = mesh.get_face_vertices(static_cast<uint32_t>(f));
        for (uint32_t v : f_v) {
            if (mesh.vertices[v].is_deleted) {
                mesh.faces[f].is_deleted = true;
                break;
            }
        }
    }

    mesh.compact();

    // Mark all edges on symmetry plane as symmetry and sharp
    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h = mesh.edges[e].halfedge;
        uint32_t u = mesh.from_vertex(h);
        uint32_t v = mesh.halfedges[h].to_vertex;
        if (mesh.vertices[u].is_on_symmetry_plane && mesh.vertices[v].is_on_symmetry_plane) {
            mesh.edges[e].is_symmetry = true;
            mesh.edges[e].is_sharp = true;
            mesh.edges[e].crease = 1.0f;
        }
    }
}

QrmStatus Stage0Prep::execute(RemeshContext& ctx) {
    if (!ctx.notify_progress(0.02f, "Stage 0: Prep & Manifold Repair")) {
        return QRM_ERR_CANCELLED;
    }

    const QrmMeshIn* in = ctx.in_mesh;
    if (!in || in->nv < 4 || in->nt < 4 || !in->positions || !in->tri_indices) {
        return QRM_ERR_INVALID_INPUT;
    }

    // Compute bounding box
    ctx.bbox = BoundingBox();
    for (uint32_t i = 0; i < in->nv; ++i) {
        Vec3f p(in->positions[3 * i + 0], in->positions[3 * i + 1], in->positions[3 * i + 2]);
        if (!std::isfinite(p.x()) || !std::isfinite(p.y()) || !std::isfinite(p.z())) {
            return QRM_ERR_INVALID_INPUT;
        }
        ctx.bbox.expand(p);
    }
    if (ctx.bbox.diagonal() <= 1e-7f) {
        return QRM_ERR_INVALID_INPUT;
    }
    ctx.bbox_diag = std::max(ctx.bbox.diagonal(), 1e-4f);

    // Compute base target length
    if (ctx.params->target_quad_count > 0) {
        float surf_area = 0.0f;
        for (uint32_t f = 0; f < in->nt; ++f) {
            uint32_t i0 = in->tri_indices[3 * f + 0];
            uint32_t i1 = in->tri_indices[3 * f + 1];
            uint32_t i2 = in->tri_indices[3 * f + 2];
            if (i0 >= in->nv || i1 >= in->nv || i2 >= in->nv) continue;
            Vec3f p0(in->positions[3 * i0 + 0], in->positions[3 * i0 + 1], in->positions[3 * i0 + 2]);
            Vec3f p1(in->positions[3 * i1 + 0], in->positions[3 * i1 + 1], in->positions[3 * i1 + 2]);
            Vec3f p2(in->positions[3 * i2 + 0], in->positions[3 * i2 + 1], in->positions[3 * i2 + 2]);
            surf_area += triangle_area(p0, p1, p2);
        }
        ctx.base_target_length = std::sqrt(std::max(surf_area, 1e-6f) / static_cast<float>(ctx.params->target_quad_count));
    } else if (ctx.params->target_edge_length > 0.0f) {
        ctx.base_target_length = ctx.params->target_edge_length;
    } else {
        ctx.base_target_length = 0.02f * ctx.bbox_diag;
    }

    // Step 1: Weld vertices
    std::vector<Vec3f> welded_pos;
    std::vector<uint32_t> welded_tris;
    std::vector<uint16_t> mat_ids;
    std::vector<uint32_t> old_to_new_v;
    float weld_eps = 1e-6f * ctx.bbox_diag;
    // T3b (S35): relax weld tolerance via env to collapse near-coincident book vertices so
    // keep-best-2 does not need slivers to stay one component. Default 1 = current behaviour.
    if (const char* e = std::getenv("QREMESH_WELD_SCALE")) {
        float s = std::atof(e);
        if (s > 1.0f) weld_eps *= s;
    }

    if (!weld_vertices(in->positions, in->nv, in->tri_indices, in->nt, in->tri_material_ids, weld_eps, welded_pos, welded_tris, mat_ids, old_to_new_v)) {
        return QRM_ERR_INVALID_INPUT;
    }

    // Step 2: Manifold repair & orient
    if (!fix_manifold_and_orient(welded_pos, welded_tris, mat_ids) || welded_tris.size() < 12) {
        return QRM_ERR_TOPOLOGY_UNFIXABLE;
    }

    // [stage0/manifold] diagnostic: count edges still shared by >2 faces after the internal
    // repair. S33/T3 showed a non-manifold INPUT (2132 edges) costs median SJ ~0.08 and the
    // SJ<0.5 tail ~10pp, so knowing whether stage0 already resolves them decides whether an
    // intrinsic pre-repair (matching pymeshlab) is the missing lever.
    {
        std::unordered_map<uint64_t, int> ecount;
        for (size_t f = 0; f + 2 < welded_tris.size(); f += 3) {
            uint32_t a = welded_tris[f], b = welded_tris[f + 1], c = welded_tris[f + 2];
            uint32_t tri[3][2] = {{a,b},{b,c},{c,a}};
            for (auto& e : tri) {
                uint32_t lo = std::min(e[0], e[1]), hi = std::max(e[0], e[1]);
                ecount[(uint64_t(lo) << 32) | hi]++;
            }
        }
        int nm = 0;
        for (const auto& kv : ecount) if (kv.second > 2) nm++;
        std::cout << "[stage0/manifold] faces_after_repair=" << welded_tris.size() / 3
                  << " nonmanifold_edges=" << nm << std::endl;
    }

    // Step 3: Build reference BVH on original cleaned geometry
    std::vector<float> flat_pos;
    flat_pos.reserve(welded_pos.size() * 3);
    for (const auto& p : welded_pos) {
        flat_pos.push_back(p.x());
        flat_pos.push_back(p.y());
        flat_pos.push_back(p.z());
    }

    if (!ctx.original_bvh.build(flat_pos.data(), static_cast<uint32_t>(welded_pos.size()),
                                welded_tris.data(), static_cast<uint32_t>(welded_tris.size() / 3))) {
        return QRM_ERR_INTERNAL;
    }

    // Build working HalfEdgeMesh
    if (!ctx.working_mesh.build_from_triangles(
            flat_pos.data(), static_cast<uint32_t>(welded_pos.size()),
            welded_tris.data(), static_cast<uint32_t>(welded_tris.size() / 3),
            mat_ids.empty() ? nullptr : mat_ids.data()
        )) {
        return QRM_ERR_INTERNAL;
    }

    // Copy user vertex density weights
    if (in->vertex_density_weights) {
        for (uint32_t i = 0; i < in->nv; ++i) {
            uint32_t new_v = old_to_new_v[i];
            if (new_v != INVALID_INDEX && new_v < ctx.working_mesh.vertices.size()) {
                ctx.working_mesh.vertices[new_v].density_weight = clampf(in->vertex_density_weights[i], 0.25f, 4.0f);
            }
        }
    }

    // Mark sharp edges and seam edges from input
    if (ctx.params->preserve_sharp_edges && in->sharp_edge_pairs && in->num_sharp_edges > 0) {
        for (uint32_t i = 0; i < in->num_sharp_edges; ++i) {
            uint32_t raw0 = in->sharp_edge_pairs[2 * i + 0];
            uint32_t raw1 = in->sharp_edge_pairs[2 * i + 1];
            if (raw0 >= in->nv || raw1 >= in->nv) continue;
            uint32_t v0 = old_to_new_v[raw0];
            uint32_t v1 = old_to_new_v[raw1];
            float crease = in->sharp_edge_creases ? in->sharp_edge_creases[i] : 1.0f;
            if (v0 != INVALID_INDEX && v1 != INVALID_INDEX && v0 != v1) {
                ctx.working_mesh.set_sharp_edge(v0, v1, crease);
            }
        }
    }

    if (ctx.params->preserve_uv_seams && in->seam_edge_pairs && in->num_seam_edges > 0) {
        for (uint32_t i = 0; i < in->num_seam_edges; ++i) {
            uint32_t raw0 = in->seam_edge_pairs[2 * i + 0];
            uint32_t raw1 = in->seam_edge_pairs[2 * i + 1];
            if (raw0 >= in->nv || raw1 >= in->nv) continue;
            uint32_t v0 = old_to_new_v[raw0];
            uint32_t v1 = old_to_new_v[raw1];
            if (v0 != INVALID_INDEX && v1 != INVALID_INDEX && v0 != v1) {
                ctx.working_mesh.set_seam_edge(v0, v1);
            }
        }
    }

    // Step 4: Fill technical small holes if preserve_open_borders is false
    if (!ctx.params->preserve_open_borders) {
        fill_small_holes(ctx.working_mesh, 2.0f * ctx.base_target_length);
    }

    // Step 5: QEM proxy if triangle count > threshold
    // proxy_tri_threshold is overridable via QREMESH_PROXY_THRESHOLD for testing dense
    // inputs (and to skip the decimation entirely by setting it very high).
    long proxy_thr = static_cast<long>(ctx.params->proxy_tri_threshold);
    if (const char* e = std::getenv("QREMESH_PROXY_THRESHOLD")) {
        long v = std::atol(e);
        if (v > 0) proxy_thr = v;
    }
    if (static_cast<long>(ctx.working_mesh.faces.size()) > proxy_thr) {
        if (!ctx.notify_progress(0.08f, "Stage 0: QEM Decimation Proxy")) {
            return QRM_ERR_CANCELLED;
        }
        simplify_qem(ctx.working_mesh, static_cast<uint32_t>(proxy_thr));
    }

    // Step 6: Symmetry slice if requested (0=X, 1=Y, 2=Z, 3=Auto-detect)
    float sym_tol = ctx.params->symmetry_tolerance * 0.01f * ctx.bbox_diag;
    ctx.active_symmetry_axis = -1;
    if (ctx.params->symmetry_axis >= 0 && ctx.params->symmetry_axis <= 2) {
        ctx.active_symmetry_axis = ctx.params->symmetry_axis;
    } else if (ctx.params->symmetry_axis == 3) {
        ctx.active_symmetry_axis = detect_symmetry_axis(ctx.working_mesh, ctx.original_bvh, sym_tol);
    }

    if (ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
        slice_symmetry(ctx.working_mesh, ctx.active_symmetry_axis, sym_tol);
    }

    ctx.dump_stage_obj("stage0_prep", ctx.working_mesh);
    return QRM_OK;
}

} // namespace qremesh
