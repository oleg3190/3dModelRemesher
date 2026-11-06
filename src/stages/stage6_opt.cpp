#include "stages/stage6_opt.h"
#include <cmath>
#include <cstdlib>
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

    std::vector<bool> poly_deleted(qmesh.polygons.size(), false);

    // Identify open-boundary vertices so we NEVER delete them as doublets
    std::vector<bool> is_boundary_v(nv, false);
    for (const auto& [key, inc] : edge_to_polys) {
        if (inc.size() == 1) {
            uint32_t u = static_cast<uint32_t>(key >> 32);
            uint32_t v = static_cast<uint32_t>(key & 0xFFFFFFFF);
            if (u < nv) is_boundary_v[u] = true;
            if (v < nv) is_boundary_v[v] = true;
        }
    }

    size_t nonmanifold_deleted = 0;
    // Deduplicate faces sharing all vertices (both quads and triangles) and filter triangles overlapping quad halves
    auto compute_poly_hash = [](const std::vector<uint32_t>& poly) -> uint64_t {
        std::vector<uint32_t> v = poly;
        std::sort(v.begin(), v.end());
        uint64_t h = 1469598103934665603ULL;
        for (uint32_t idx : v) {
            h ^= static_cast<uint64_t>(idx) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    };

    auto compute_tri_hash = [](uint32_t a, uint32_t b, uint32_t c) -> uint64_t {
        uint32_t v[3] = {a, b, c};
        if (v[0] > v[1]) std::swap(v[0], v[1]);
        if (v[1] > v[2]) std::swap(v[1], v[2]);
        if (v[0] > v[1]) std::swap(v[0], v[1]);
        uint64_t h = 1469598103934665603ULL;
        for (int i = 0; i < 3; ++i) {
            h ^= static_cast<uint64_t>(v[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    };

    std::unordered_set<uint64_t> seen_poly_keys;
    std::unordered_set<uint64_t> seen_tri_keys;

    // Pass 1: register triangles first
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        const auto& poly = qmesh.polygons[p];
        if (poly.size() == 3) {
            uint64_t poly_hash = compute_poly_hash(poly);
            uint64_t th = compute_tri_hash(poly[0], poly[1], poly[2]);
            if (seen_poly_keys.count(poly_hash) || seen_tri_keys.count(th)) {
                poly_deleted[p] = true;
                nonmanifold_deleted++;
            } else {
                seen_poly_keys.insert(poly_hash);
                seen_tri_keys.insert(th);
            }
        }
    }
    // Pass 2: register quads; if fan triangulation (0,1,2)+(0,2,3) collides with an existing triangle,
    // rotate the quad by 1 vertex to switch its fan diagonal to (1,3) before deleting
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        auto& poly = qmesh.polygons[p];
        if (poly.size() == 4) {
            uint64_t poly_hash = compute_poly_hash(poly);
            if (seen_poly_keys.count(poly_hash)) {
                poly_deleted[p] = true;
                nonmanifold_deleted++;
                continue;
            }
            uint64_t th0 = compute_tri_hash(poly[0], poly[1], poly[2]);
            uint64_t th1 = compute_tri_hash(poly[0], poly[2], poly[3]);
            if (seen_tri_keys.count(th0) || seen_tri_keys.count(th1)) {
                uint64_t th0_rot = compute_tri_hash(poly[1], poly[2], poly[3]);
                uint64_t th1_rot = compute_tri_hash(poly[1], poly[3], poly[0]);
                if (!seen_tri_keys.count(th0_rot) && !seen_tri_keys.count(th1_rot)) {
                    std::rotate(poly.begin(), poly.begin() + 1, poly.end());
                    th0 = th0_rot;
                    th1 = th1_rot;
                } else {
                    poly_deleted[p] = true;
                    nonmanifold_deleted++;
                    continue;
                }
            }
            seen_poly_keys.insert(poly_hash);
            seen_tri_keys.insert(th0);
            seen_tri_keys.insert(th1);
        } else if (poly.size() != 3) {
            if (poly.size() > 4) {
                uint64_t poly_hash = compute_poly_hash(poly);
                if (seen_poly_keys.count(poly_hash)) {
                    poly_deleted[p] = true;
                    nonmanifold_deleted++;
                } else {
                    seen_poly_keys.insert(poly_hash);
                }
            } else {
                poly_deleted[p] = true;
            }
        }
    }
    std::cout << "[stage6] duplicate_faces_deleted=" << nonmanifold_deleted << std::endl;

    // Valence count
    std::vector<int> valence(nv, 0);
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        if (poly_deleted[p]) continue;
        for (uint32_t v : qmesh.polygons[p]) {
            if (v < nv) valence[v]++;
        }
    }

    size_t doublet_deleted = 0;
    // Doublet elimination: strictly interior vertex of valence 2 shared by 2 quads
    for (uint32_t v = 0; v < nv; ++v) {
        if (valence[v] == 2 && !is_boundary_v[v]) {
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

                int idx0 = -1;
                for (int i = 0; i < 4; ++i) {
                    if (poly0[i] == v) { idx0 = i; break; }
                }
                uint32_t a0 = poly0[(idx0 + 1) % 4];
                uint32_t b0 = poly0[(idx0 + 2) % 4];
                uint32_t c0 = poly0[(idx0 + 3) % 4];

                int idx1 = -1;
                for (int i = 0; i < 4; ++i) {
                    if (poly1[i] == v) { idx1 = i; break; }
                }

                uint32_t a1 = poly1[(idx1 + 1) % 4];
                uint32_t d1 = poly1[(idx1 + 2) % 4];
                uint32_t c1 = poly1[(idx1 + 3) % 4];

                if (a1 == c0 && c1 == a0 && b0 != d1) {
                    std::vector<uint32_t> merged_q = { a0, b0, c0, d1 };
                    uint64_t mh = compute_poly_hash(merged_q);
                    if (!seen_poly_keys.count(mh)) {
                        seen_poly_keys.insert(mh);
                        qmesh.polygons[p0] = std::move(merged_q);
                        poly_deleted[p1] = true;
                        doublet_deleted++;
                    }
                }
            }
        }
    }
    std::cout << "[stage6] doublet_deleted=" << doublet_deleted << std::endl;

    // Filter deleted polygons
    std::vector<std::vector<uint32_t>> clean_polys;
    std::vector<uint16_t> clean_mats;
    for (size_t p = 0; p < qmesh.polygons.size(); ++p) {
        if (!poly_deleted[p]) {
            clean_polys.push_back(std::move(qmesh.polygons[p]));
            clean_mats.push_back(p < qmesh.material_ids.size() ? qmesh.material_ids[p] : 0);
        }
    }

    // Pair adjacent triangles sharing an interior edge into quads when geometrically valid
    std::unordered_set<uint64_t> sharp_or_seam;
    for (const auto& e : qmesh.sharp_edges) {
        uint32_t u = std::min(e.first, e.second), v = std::max(e.first, e.second);
        sharp_or_seam.insert((static_cast<uint64_t>(u) << 32) | v);
    }
    for (const auto& e : qmesh.seam_edges) {
        uint32_t u = std::min(e.first, e.second), v = std::max(e.first, e.second);
        sharp_or_seam.insert((static_cast<uint64_t>(u) << 32) | v);
    }

    std::unordered_map<uint64_t, std::vector<size_t>> tri_edge_map;
    std::unordered_map<uint64_t, int> all_edge_cnt;
    for (size_t p = 0; p < clean_polys.size(); ++p) {
        const auto& poly = clean_polys[p];
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i], v = poly[(i + 1) % n];
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            all_edge_cnt[key]++;
            if (n == 3) {
                tri_edge_map[key].push_back(p);
            }
        }
    }

    struct TriMergeCand {
        float sj;
        size_t t0, t1;
        std::array<uint32_t, 4> q;
    };
    std::vector<TriMergeCand> merge_cands;
    for (const auto& [key, tlist] : tri_edge_map) {
        if (tlist.size() != 2 || all_edge_cnt[key] != 2 || sharp_or_seam.count(key)) continue;
        size_t t0 = tlist[0], t1 = tlist[1];
        if (clean_mats[t0] != clean_mats[t1]) continue;
        const auto& p0 = clean_polys[t0];
        const auto& p1 = clean_polys[t1];

        int i0 = -1;
        for (int i = 0; i < 3; ++i) {
            uint32_t u = p0[i], v = p0[(i + 1) % 3];
            uint64_t k = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            if (k == key) { i0 = i; break; }
        }
        int i1 = -1;
        for (int i = 0; i < 3; ++i) {
            uint32_t u = p1[i], v = p1[(i + 1) % 3];
            uint64_t k = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            if (k == key) { i1 = i; break; }
        }
        if (i0 < 0 || i1 < 0) continue;

        uint32_t a = p0[i0];
        uint32_t b = p0[(i0 + 1) % 3];
        uint32_t c = p0[(i0 + 2) % 3];
        // Opposite orientation across shared edge (a, b) means p1 has directed edge (b, a)
        if (p1[i1] != b || p1[(i1 + 1) % 3] != a) continue;
        uint32_t d = p1[(i1 + 2) % 3];
        if (c == d || c == a || c == b || d == a || d == b) continue;

        float sj = compute_scaled_jacobian(
            qmesh.positions[d], qmesh.positions[b], qmesh.positions[c], qmesh.positions[a]);
        float warp = compute_warp_angle_deg(
            qmesh.positions[d], qmesh.positions[b], qmesh.positions[c], qmesh.positions[a]);
        uint64_t e_cd = (static_cast<uint64_t>(std::min(c, d)) << 32) | std::max(c, d);
        if (sj > -0.20f && warp < 75.0f && all_edge_cnt.find(e_cd) == all_edge_cnt.end()) {
            merge_cands.push_back({sj, t0, t1, {d, b, c, a}});
        }
    }

    std::sort(merge_cands.begin(), merge_cands.end(), [](const TriMergeCand& x, const TriMergeCand& y) {
        return x.sj > y.sj;
    });

    std::vector<bool> tri_merged(clean_polys.size(), false);
    for (const auto& mc : merge_cands) {
        if (tri_merged[mc.t0] || tri_merged[mc.t1]) continue;
        std::vector<uint32_t> qvec = {mc.q[0], mc.q[1], mc.q[2], mc.q[3]};
        uint64_t qh = compute_poly_hash(qvec);
        if (seen_poly_keys.count(qh)) continue;
        seen_poly_keys.insert(qh);
        tri_merged[mc.t0] = true;
        tri_merged[mc.t1] = true;
        clean_polys[mc.t0] = std::move(qvec);
    }

    std::vector<std::vector<uint32_t>> final_polys;
    std::vector<uint16_t> final_mats;
    final_polys.reserve(clean_polys.size());
    final_mats.reserve(clean_mats.size());
    for (size_t p = 0; p < clean_polys.size(); ++p) {
        if (tri_merged[p] && clean_polys[p].size() == 3) continue;
        final_polys.push_back(std::move(clean_polys[p]));
        final_mats.push_back(clean_mats[p]);
    }

    qmesh.polygons = std::move(final_polys);
    qmesh.material_ids = std::move(final_mats);
}

void Stage6Optimize::fill_small_boundary_holes(
    RemeshContext& ctx,
    QuadMeshResult& qmesh,
    size_t max_hole_verts
) {
    if (qmesh.positions.empty() || qmesh.polygons.empty()) return;

    // Map undirected edge -> list of face indices
    std::unordered_map<uint64_t, std::vector<uint32_t>> edge_to_faces;
    for (size_t fi = 0; fi < qmesh.polygons.size(); ++fi) {
        const auto& p = qmesh.polygons[fi];
        size_t n = p.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = p[i];
            uint32_t v = p[(i + 1) % n];
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            edge_to_faces[key].push_back(static_cast<uint32_t>(fi));
        }
    }

    std::vector<std::pair<uint32_t, uint32_t>> bnd_edges;
    for (const auto& [key, flist] : edge_to_faces) {
        if (flist.size() == 1) {
            uint32_t u = static_cast<uint32_t>(key >> 32);
            uint32_t v = static_cast<uint32_t>(key & 0xFFFFFFFF);
            bnd_edges.emplace_back(u, v);
        }
    }

    // Find and trace simple boundary loops
    std::unordered_map<uint32_t, std::vector<uint32_t>> bnd_adj;
    for (const auto& [u, v] : bnd_edges) {
        bnd_adj[u].push_back(v);
        bnd_adj[v].push_back(u);
    }

    std::unordered_set<uint32_t> visited_v;
    size_t holes_filled = 0;

    for (const auto& [start_v, nbrs] : bnd_adj) {
        if (visited_v.count(start_v) || nbrs.size() != 2) continue;

        std::vector<uint32_t> loop;
        uint32_t curr = start_v;
        uint32_t prev = INVALID_INDEX;
        bool is_simple_loop = true;

        while (true) {
            visited_v.insert(curr);
            loop.push_back(curr);
            const auto& cur_nbrs = bnd_adj[curr];
            if (cur_nbrs.size() != 2) {
                is_simple_loop = false;
                break;
            }
            uint32_t nxt = (cur_nbrs[0] == prev) ? cur_nbrs[1] : cur_nbrs[0];
            if (nxt == start_v) {
                break; // Closed loop found!
            }
            if (visited_v.count(nxt)) {
                is_simple_loop = false;
                break;
            }
            prev = curr;
            curr = nxt;
            if (loop.size() > max_hole_verts) {
                is_simple_loop = false;
                break;
            }
        }

        if (is_simple_loop && loop.size() >= 3 && loop.size() <= max_hole_verts) {
            // Compute average neighbor normal to ensure correct winding
            Vec3f avg_normal = Vec3f::Zero();
            for (uint32_t lv : loop) {
                uint32_t nbr_v = bnd_adj[lv][0];
                uint64_t k = (static_cast<uint64_t>(std::min(lv, nbr_v)) << 32) | std::max(lv, nbr_v);
                auto it = edge_to_faces.find(k);
                if (it != edge_to_faces.end()) {
                    for (uint32_t fi : it->second) {
                        const auto& poly = qmesh.polygons[fi];
                        if (poly.size() >= 3) {
                            Vec3f fn = (qmesh.positions[poly[1]] - qmesh.positions[poly[0]]).cross(qmesh.positions[poly[2]] - qmesh.positions[poly[0]]);
                            if (fn.norm() > 1e-6f) avg_normal += fn.normalized();
                        }
                    }
                }
            }
            if (avg_normal.norm() < 1e-6f) avg_normal = Vec3f(0.0f, 0.0f, 1.0f);
            else avg_normal.normalize();

            auto add_hole_face = [&](std::vector<uint32_t> f_verts) {
                if (f_verts.size() < 3) return;
                Vec3f fn = (qmesh.positions[f_verts[1]] - qmesh.positions[f_verts[0]]).cross(qmesh.positions[f_verts[2]] - qmesh.positions[f_verts[0]]);
                if (fn.dot(avg_normal) < 0.0f) {
                    std::reverse(f_verts.begin(), f_verts.end());
                }
                qmesh.polygons.push_back(std::move(f_verts));
                qmesh.material_ids.push_back(0);
                holes_filled++;
            };

            if (loop.size() == 3) {
                add_hole_face({loop[0], loop[1], loop[2]});
            } else if (loop.size() == 4) {
                add_hole_face({loop[0], loop[1], loop[2], loop[3]});
            } else {
                // Centroid fan
                Vec3f c = Vec3f::Zero();
                for (uint32_t lv : loop) c += qmesh.positions[lv];
                c /= static_cast<float>(loop.size());
                Vec3f cp, cn;
                float d2;
                if (ctx.original_bvh.closest_point(c, cp, cn, d2)) c = cp;
                uint32_t c_idx = static_cast<uint32_t>(qmesh.positions.size());
                qmesh.positions.push_back(c);
                size_t L = loop.size();
                for (size_t k = 0; k + 1 < L; k += 2) {
                    add_hole_face({loop[k], loop[k + 1], loop[(k + 2) % L], c_idx});
                }
                if (L % 2 == 1) {
                    add_hole_face({loop[L - 1], loop[0], c_idx});
                }
            }
        }
    }
    std::cout << "[stage6] small boundary holes filled: " << holes_filled << std::endl;
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

    for (uint32_t v = 0; v < nv; ++v) {
        std::sort(sharp_nbrs[v].begin(), sharp_nbrs[v].end());
    }

    float max_dev = ctx.params->max_surface_deviation * 0.01f * ctx.bbox_diag;
    int effective_iters = std::max(iters + 3, 8);

    // Forward Laplacian relaxation with an ANCHOR cap: every vertex keeps its position
    // from the start of the relaxation as an anchor and may travel at most 60% of its
    // shortest incident edge. Without the cap the smoother slides vertices off small
    // features (rails, brackets, rims) while staying on the source surface - erodes detail.
    const std::vector<Vec3f> anchor = qmesh.positions;
    std::vector<float> slide_cap(nv, 0.0f);
    for (const auto& poly : qmesh.polygons) {
        size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            uint32_t u = poly[i];
            uint32_t w = poly[(i + 1) % n];
            if (u >= nv || w >= nv) continue;
            float len = (anchor[u] - anchor[w]).norm();
            if (len > 1e-9f) {
                float c = 0.60f * len;
                if (slide_cap[u] <= 0.0f || c < slide_cap[u]) slide_cap[u] = c;
                if (slide_cap[w] <= 0.0f || c < slide_cap[w]) slide_cap[w] = c;
            }
        }
    }

    for (int iter = 0; iter < effective_iters; ++iter) {
        // Forward (lambda) step only: the mu counter-step was tried and it amplifies
        // micro-irregularities -> the surface crinkles (area +20%) without moving off
        // the source. The anchor cap below already protects against shrinkage/erosion.
        const float step_factor = 0.40f;
        std::vector<Vec3f> new_positions = qmesh.positions;

        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, nv), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t v = r.begin(); v != r.end(); ++v) {
                if (sharp_deg[v] >= 3) continue;

                if (sharp_deg[v] == 2) {
                    uint32_t na = sharp_nbrs[v][0];
                    uint32_t nb = sharp_nbrs[v][1];
                    Vec3f curve_mid = (qmesh.positions[na] + qmesh.positions[nb]) * 0.5f;
                    Vec3f shift = curve_mid - qmesh.positions[v];
                    Vec3f curve_dir = (qmesh.positions[nb] - qmesh.positions[na]).normalized();
                    Vec3f tangent_shift = shift.dot(curve_dir) * curve_dir;
                    new_positions[v] += step_factor * tangent_shift;
                } else if (!adj[v].empty()) {
                    Vec3f centroid = Vec3f::Zero();
                    for (uint32_t nbr : adj[v]) centroid += qmesh.positions[nbr];
                    centroid /= static_cast<float>(adj[v].size());

                    Vec3f shift = centroid - qmesh.positions[v];

                    Vec3f normal, closest_orig;
                    float d2;
                    if (ctx.original_bvh.closest_point(qmesh.positions[v], closest_orig, normal, d2)) {
                        Vec3f tan_shift = shift - shift.dot(normal) * normal;
                        new_positions[v] += step_factor * tan_shift;
                    }
                }
            }
        });

        // Project to original surface + anchor cap
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, nv), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t v = r.begin(); v != r.end(); ++v) {
                if (sharp_deg[v] >= 3) continue;
                Vec3f cp, norm;
                float dist_sq;
                if (ctx.original_bvh.closest_point(new_positions[v], cp, norm, dist_sq)) {
                    float dist = (new_positions[v] - cp).norm();
                    if (dist > max_dev && dist > 1e-6f) {
                        new_positions[v] = cp + (new_positions[v] - cp) * (max_dev / dist);
                    } else {
                        new_positions[v] = cp;
                    }
                }
                float cap = (v < slide_cap.size()) ? slide_cap[v] : 0.0f;
                if (cap > 0.0f) {
                    Vec3f delta = new_positions[v] - anchor[v];
                    float dl = delta.norm();
                    if (dl > cap) new_positions[v] = anchor[v] + delta * (cap / dl);
                }
            }
        });

        qmesh.positions = std::move(new_positions);
    }
}

void Stage6Optimize::untangle_low_jacobians(
    RemeshContext& ctx,
    QuadMeshResult& qmesh
) {
    constexpr int MAX_PASSES = 16;

    // Anti-wrinkle guard: the untangler may only move a vertex a fraction of its shortest
    // incident edge away from where it started. Without it, per-quad quality greedy moves
    // let vertices migrate along the surface and the mesh inflates by ~19% (wrinkles)
    // while every vertex still lies exactly on the source.
    const std::vector<Vec3f> anchor = qmesh.positions;
    std::vector<float> slide_cap(qmesh.positions.size(), 0.0f);
    for (const auto& poly : qmesh.polygons) {
        size_t pn = poly.size();
        for (size_t i = 0; i < pn; ++i) {
            uint32_t u = poly[i];
            uint32_t w = poly[(i + 1) % pn];
            if (u >= slide_cap.size() || w >= slide_cap.size()) continue;
            float len = (anchor[u] - anchor[w]).norm();
            if (len > 1e-9f) {
                float c = 1.0f * len;  // untangle needs room for corrective slides
                if (slide_cap[u] <= 0.0f || c < slide_cap[u]) slide_cap[u] = c;
                if (slide_cap[w] <= 0.0f || c < slide_cap[w]) slide_cap[w] = c;
            }
        }
    }

    std::vector<std::vector<size_t>> v_to_quads(qmesh.positions.size());
    for (size_t qi = 0; qi < qmesh.polygons.size(); ++qi) {
        if (qmesh.polygons[qi].size() == 4) {
            for (uint32_t v : qmesh.polygons[qi]) {
                if (v < v_to_quads.size()) v_to_quads[v].push_back(qi);
            }
        }
    }

    auto compute_quad_aspect = [](const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3) -> float {
        float l0 = (p1 - p0).norm();
        float l1 = (p2 - p1).norm();
        float l2 = (p3 - p2).norm();
        float l3 = (p0 - p3).norm();
        float lmax = std::max({l0, l1, l2, l3});
        float lmin = std::min({l0, l1, l2, l3});
        return (lmin > 1e-8f) ? (lmax / lmin) : 1000.0f;
    };

    auto compute_quad_combined_quality = [&](const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3) -> float {
        float sj = compute_scaled_jacobian(p0, p1, p2, p3);
        float ar = compute_quad_aspect(p0, p1, p2, p3);
        if (sj <= 0.0f) {
            return sj - 0.005f * std::min(ar, 100.0f);
        }
        return sj / (1.0f + 0.45f * std::max(0.0f, ar - 1.15f));
    };

    auto eval_incident_quality = [&](uint32_t vid, float& out_min_sj, float& out_min_q, float& out_avg_q) -> float {
        out_min_q = 1.0f;
        out_min_sj = 1.0f;
        out_avg_q = 1.0f;
        if (vid >= v_to_quads.size() || v_to_quads[vid].empty()) return 1.0f;
        float sum_q = 0.0f;
        for (size_t qi : v_to_quads[vid]) {
            const auto& q = qmesh.polygons[qi];
            float sj = compute_scaled_jacobian(
                qmesh.positions[q[0]], qmesh.positions[q[1]],
                qmesh.positions[q[2]], qmesh.positions[q[3]]
            );
            float qval = compute_quad_combined_quality(
                qmesh.positions[q[0]], qmesh.positions[q[1]],
                qmesh.positions[q[2]], qmesh.positions[q[3]]
            );
            out_min_sj = std::min(out_min_sj, sj);
            out_min_q = std::min(out_min_q, qval);
            sum_q += qval;
        }
        out_avg_q = sum_q / static_cast<float>(v_to_quads[vid].size());
        return 0.45f * out_min_q + 0.55f * out_avg_q;
    };

    auto incident_area = [&](uint32_t vid) -> float {
        float a = 0.0f;
        if (vid >= v_to_quads.size()) return 0.0f;
        for (size_t qi : v_to_quads[vid]) {
            const auto& q = qmesh.polygons[qi];
            if (q.size() < 3 || q[0] >= qmesh.positions.size()) continue;
            const Vec3f& a0 = qmesh.positions[q[0]];
            for (size_t k = 1; k + 1 < q.size(); ++k) {
                if (q[k] >= qmesh.positions.size() || q[k + 1] >= qmesh.positions.size()) continue;
                Vec3f e1 = qmesh.positions[q[k]] - a0;
                Vec3f e2 = qmesh.positions[q[k + 1]] - a0;
                a += 0.5f * e1.cross(e2).norm();
            }
        }
        return a;
    };

    // Global surface-area band: quality moves may neither inflate nor collapse the mesh
    // relative to what stage 5 produced (per-move *incident* area guard was one-sided and
    // starved the untangler, while no guard at all inflated the mesh by ~19% = wrinkles).
    double global_area = 0.0;
    for (const auto& poly : qmesh.polygons) {
        if (poly.size() < 3 || poly[0] >= qmesh.positions.size()) continue;
        const Vec3f& a0 = qmesh.positions[poly[0]];
        for (size_t k = 1; k + 1 < poly.size(); ++k) {
            if (poly[k] >= qmesh.positions.size() || poly[k + 1] >= qmesh.positions.size()) continue;
            Vec3f e1 = qmesh.positions[poly[k]] - a0;
            Vec3f e2 = qmesh.positions[poly[k + 1]] - a0;
            global_area += 0.5 * static_cast<double>(e1.cross(e2).norm());
        }
    }
    const double area_lo = 0.94 * global_area;
    const double area_hi = 1.05 * global_area;

    for (int pass = 0; pass < MAX_PASSES; ++pass) {
        bool fixed_any = false;

        for (auto& poly : qmesh.polygons) {
            if (poly.size() != 4) continue;

            const Vec3f& p0 = qmesh.positions[poly[0]];
            const Vec3f& p1 = qmesh.positions[poly[1]];
            const Vec3f& p2 = qmesh.positions[poly[2]];
            const Vec3f& p3 = qmesh.positions[poly[3]];

            float j = compute_scaled_jacobian(p0, p1, p2, p3);
            float ar = compute_quad_aspect(p0, p1, p2, p3);

            // Genuinely defective quads trigger repair; a per-move area guard below stops
            // the greedy quality moves from inflating the mesh.
            if (j < 0.78f || ar > 1.35f) {
                Vec3f c = (p0 + p1 + p2 + p3) * 0.25f;

                Vec3f normal = Vec3f(0.0f, 0.0f, 1.0f);
                Vec3f cp;
                float d2;
                if (ctx.original_bvh.closest_point(c, cp, normal, d2)) {
                    c = cp;
                }
                Vec3f q_norm = (p2 - p0).cross(p3 - p1);
                if (q_norm.squaredNorm() > 1e-12f) {
                    Vec3f qn = q_norm.normalized();
                    if (qn.dot(normal) < 0.0f) normal = -normal;
                    normal = (normal + qn).normalized();
                }

                // 1) Procrustes orthogonal frame aligned with quad edges
                Vec3f u_vec = (p1 - p0) + (p2 - p3);
                Vec3f v_vec = (p3 - p0) + (p2 - p1);
                u_vec -= u_vec.dot(normal) * normal;
                v_vec -= v_vec.dot(normal) * normal;

                Vec3f u_sym = u_vec + v_vec.cross(normal);
                if (u_sym.squaredNorm() < 1e-10f) {
                    Vec3f d1_raw = (p2 - p0) - (p2 - p0).dot(normal) * normal;
                    if (d1_raw.squaredNorm() < 1e-10f) continue;
                    u_sym = d1_raw.normalized();
                } else {
                    u_sym.normalize();
                }
                Vec3f v_sym = normal.cross(u_sym).normalized();

                float l0 = (p1 - p0).norm();
                float l1 = (p2 - p1).norm();
                float l2 = (p3 - p2).norm();
                float l3 = (p0 - p3).norm();
                float h = 0.125f * (l0 + l1 + l2 + l3);

                Vec3f proc_pts[4] = {
                    c - h * u_sym - h * v_sym,
                    c + h * u_sym - h * v_sym,
                    c + h * u_sym + h * v_sym,
                    c - h * u_sym + h * v_sym
                };

                // 2) Diagonal-aligned untangling square (robust for folded / negative-SJ quads)
                Vec3f d1_raw = (p2 - p0) - (p2 - p0).dot(normal) * normal;
                Vec3f diag1 = (d1_raw.squaredNorm() > 1e-10f) ? d1_raw.normalized() : (u_sym + v_sym).normalized();
                Vec3f diag2 = normal.cross(diag1).normalized();
                if ((p3 - p1).dot(diag2) < 0.0f) diag2 = -diag2;
                float r1 = 0.5f * (p2 - p0).norm();
                float r2 = 0.5f * (p3 - p1).norm();
                float avg_r = 0.5f * (r1 + r2);
                Vec3f diag_pts[4] = {
                    c - avg_r * diag1,
                    c - avg_r * diag2,
                    c + avg_r * diag1,
                    c + avg_r * diag2
                };

                for (int i = 0; i < 4; ++i) {
                    uint32_t vid = poly[i];
                    float before_sj = 1.0f, before_min_q = 1.0f, before_avg_q = 1.0f;
                    float before_score = eval_incident_quality(vid, before_sj, before_min_q, before_avg_q);
                    float before_area = incident_area(vid);
                    Vec3f orig_p = qmesh.positions[vid];

                    bool accepted = false;
                    const Vec3f targets[2] = { proc_pts[i], diag_pts[i] };
                    int num_targets = (j < 0.35f || before_sj < 0.25f) ? 2 : 1;

                    for (int ti = 0; ti < num_targets && !accepted; ++ti) {
                        for (float alpha : {0.38f, 0.20f, 0.10f}) {
                            Vec3f cand_p = (1.0f - alpha) * orig_p + alpha * targets[ti];
                            Vec3f proj_p, n;
                            float d2_proj;
                            if (ctx.original_bvh.closest_point(cand_p, proj_p, n, d2_proj)) {
                                cand_p = proj_p;
                            }
                            if (vid < slide_cap.size() && slide_cap[vid] > 0.0f &&
                                (cand_p - anchor[vid]).norm() > slide_cap[vid]) {
                                continue; // would migrate the vertex too far -> skip
                            }

                            qmesh.positions[vid] = cand_p;
                            float after_sj = 1.0f, after_min_q = 1.0f, after_avg_q = 1.0f;
                            float after_score = eval_incident_quality(vid, after_sj, after_min_q, after_avg_q);
                            // Global area band guard: the move must keep the TOTAL surface
                            // inside [96%, 102%] of what stage 5 produced.
                            float after_area = incident_area(vid);
                            double projected_global = global_area - before_area + after_area;
                            bool area_ok = (projected_global >= area_lo && projected_global <= area_hi);

                            bool move_ok = false;
                            if (before_sj < 0.22f) {
                                // Strict untangling mode: prioritize raising the minimum Scaled Jacobian
                                move_ok = (after_sj > before_sj + 1e-4f) ||
                                          (after_sj >= before_sj - 1e-4f && after_min_q > before_min_q + 1e-4f);
                            } else {
                                bool sj_ok = (after_sj >= 0.22f);
                                bool min_q_ok = (after_min_q >= std::min(before_min_q - 0.015f, 0.25f));
                                move_ok = (after_score > before_score + 1e-4f && sj_ok && min_q_ok);
                            }

                            if (move_ok && area_ok) {
                                fixed_any = true;
                                accepted = true;
                                global_area = projected_global;
                                break;
                            } else {
                                qmesh.positions[vid] = orig_p;
                            }
                        }
                    }
                }
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

    // Hausdorff & RMS error computed with dense two-way sampling against original mesh
    // 1. out -> src (samples on output mesh to ctx.original_bvh)
    float max_out2src_sq = 0.0f;
    double sum_out2src_sq = 0.0;
    size_t out_samples_count = 0;

    for (const auto& p : qmesh.positions) {
        Vec3f cp, n;
        float d2;
        if (ctx.original_bvh.closest_point(p, cp, n, d2)) {
            max_out2src_sq = std::max(max_out2src_sq, d2);
            sum_out2src_sq += d2;
            out_samples_count++;
        }
    }

    for (const auto& poly : qmesh.polygons) {
        if (poly.size() >= 3) {
            Vec3f centroid = Vec3f::Zero();
            for (uint32_t idx : poly) centroid += qmesh.positions[idx];
            centroid /= static_cast<float>(poly.size());

            Vec3f cp, n;
            float d2;
            if (ctx.original_bvh.closest_point(centroid, cp, n, d2)) {
                max_out2src_sq = std::max(max_out2src_sq, d2);
                sum_out2src_sq += d2;
                out_samples_count++;
            }
        }
    }

    // 2. src -> out (samples on original mesh to out_bvh)
    BVHTree out_bvh;
    std::vector<float> flat_out_pos;
    flat_out_pos.reserve(qmesh.positions.size() * 3);
    for (const auto& p : qmesh.positions) {
        flat_out_pos.push_back(p.x());
        flat_out_pos.push_back(p.y());
        flat_out_pos.push_back(p.z());
    }

    std::vector<uint32_t> out_tri_indices;
    for (const auto& poly : qmesh.polygons) {
        if (poly.size() == 4) {
            out_tri_indices.push_back(poly[0]);
            out_tri_indices.push_back(poly[1]);
            out_tri_indices.push_back(poly[2]);

            out_tri_indices.push_back(poly[0]);
            out_tri_indices.push_back(poly[2]);
            out_tri_indices.push_back(poly[3]);
        } else if (poly.size() >= 3) {
            for (size_t k = 1; k + 1 < poly.size(); ++k) {
                out_tri_indices.push_back(poly[0]);
                out_tri_indices.push_back(poly[k]);
                out_tri_indices.push_back(poly[k + 1]);
            }
        }
    }

    bool has_out_bvh = false;
    if (!out_tri_indices.empty()) {
        has_out_bvh = out_bvh.build(
            flat_out_pos.data(),
            static_cast<uint32_t>(qmesh.positions.size()),
            out_tri_indices.data(),
            static_cast<uint32_t>(out_tri_indices.size() / 3)
        );
    }

    float max_src2out_sq = 0.0f;
    double sum_src2out_sq = 0.0;
    size_t src_samples_count = 0;

    if (has_out_bvh) {
        for (const auto& v : ctx.working_mesh.vertices) {
            if (!v.is_deleted) {
                Vec3f cp, n;
                float d2;
                if (out_bvh.closest_point(v.pos, cp, n, d2)) {
                    max_src2out_sq = std::max(max_src2out_sq, d2);
                    sum_src2out_sq += d2;
                    src_samples_count++;
                }
            }
        }
    }

    float max_dev_sq = std::max(max_out2src_sq, max_src2out_sq);
    double rms_out2src = std::sqrt(sum_out2src_sq / std::max<size_t>(1, out_samples_count));
    double rms_src2out = std::sqrt(sum_src2out_sq / std::max<size_t>(1, src_samples_count));
    double chamfer_rms = 0.5 * (rms_out2src + rms_src2out);

    float diag = std::max(ctx.bbox_diag, 1e-4f);
    r.hausdorff_max_rel = (std::sqrt(max_dev_sq) / diag) * 100.0f;
    r.rms_deviation_rel = static_cast<float>((chamfer_rms / diag) * 100.0);

    auto end_time = std::chrono::high_resolution_clock::now();
    r.total_time_sec = std::chrono::duration<float>(end_time - ctx.start_time).count();
}

QrmStatus Stage6Optimize::execute(RemeshContext& ctx, QuadMeshResult& qmesh) {
    if (!ctx.notify_progress(0.85f, "Stage 6: Topological Cleanup & Tangential Relaxation")) {
        return QRM_ERR_CANCELLED;
    }

    // Debug overrides for A/B testing of the post-process stages:
    //   QREMESH_RELAX_ITERS=<n>  override relaxation iterations (0 disables it)
    //   QREMESH_NO_HOLEFILL=1    skip hole filling
    //   QREMESH_NO_UNTANGLE=1    skip the untangle pass
    int relax_iters = ctx.params->vertex_relax_iters;
    bool skip_holefill = false;
    bool skip_untangle = false;
    if (const char* env_r = std::getenv("QREMESH_RELAX_ITERS")) {
        relax_iters = std::max(0, std::atoi(env_r));
    }
    if (const char* env_h = std::getenv("QREMESH_NO_HOLEFILL")) {
        skip_holefill = (std::atoi(env_h) != 0);
    }
    if (const char* env_u = std::getenv("QREMESH_NO_UNTANGLE")) {
        skip_untangle = (std::atoi(env_u) != 0);
    }

    // Surface-area timeline: shows which sub-stage inflates the mesh (wrinkles = excess area)
    auto poly_area = [&qmesh]() {
        double a = 0.0;
        for (const auto& poly : qmesh.polygons) {
            if (poly.size() < 3) continue;
            const Vec3f& p0 = qmesh.positions[poly[0]];
            for (size_t i = 1; i + 1 < poly.size(); ++i) {
                Vec3f e1 = qmesh.positions[poly[i]] - p0;
                Vec3f e2 = qmesh.positions[poly[i + 1]] - p0;
                a += 0.5 * static_cast<double>(e1.cross(e2).norm());
            }
        }
        return a;
    };
    auto log_area = [&](const char* tag) {
        std::cout << "[stage6/area] " << tag << "=" << poly_area()
                  << " polys=" << qmesh.polygons.size() << std::endl;
    };
    log_area("entry");

    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 300); log_area("after_fill"); }
    cleanup_topology(qmesh);
    log_area("after_cleanup");
    tangential_taubin_smoothing(ctx, qmesh, relax_iters);
    log_area("after_relax");
    cleanup_topology(qmesh);
    log_area("after_cleanup2");
    if (!skip_untangle) { untangle_low_jacobians(ctx, qmesh); log_area("after_untangle"); }
    // Second fill pass: cleanup/untangle can open small holes after the first pass
    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 300); log_area("after_fill2"); }

    // Final exact-position weld & manifold/degenerate face sanitizer
    {
        struct PosKey {
            int64_t x, y, z;
            bool operator==(const PosKey& o) const { return x == o.x && y == o.y && z == o.z; }
        };
        struct PosKeyHash {
            size_t operator()(const PosKey& k) const {
                size_t h = static_cast<size_t>(k.x) * 73856093ULL;
                h ^= static_cast<size_t>(k.y) * 19349663ULL;
                h ^= static_cast<size_t>(k.z) * 83492791ULL;
                return h;
            }
        };
        std::unordered_map<PosKey, uint32_t, PosKeyHash> pos_to_vid;
        std::vector<uint32_t> v_remap(qmesh.positions.size());
        for (size_t i = 0; i < qmesh.positions.size(); ++i) {
            const Vec3f& p = qmesh.positions[i];
            PosKey k{
                static_cast<int64_t>(std::llround(static_cast<double>(p.x()) * 1e7)),
                static_cast<int64_t>(std::llround(static_cast<double>(p.y()) * 1e7)),
                static_cast<int64_t>(std::llround(static_cast<double>(p.z()) * 1e7))
            };
            auto it = pos_to_vid.find(k);
            if (it == pos_to_vid.end()) {
                pos_to_vid[k] = static_cast<uint32_t>(i);
                v_remap[i] = static_cast<uint32_t>(i);
            } else {
                v_remap[i] = it->second;
            }
        }

        auto tri_area = [&](uint32_t a, uint32_t b, uint32_t c) -> double {
            Eigen::Vector3d pa = qmesh.positions[a].cast<double>();
            Eigen::Vector3d pb = qmesh.positions[b].cast<double>();
            Eigen::Vector3d pc = qmesh.positions[c].cast<double>();
            return 0.5 * (pb - pa).cross(pc - pa).norm();
        };

        auto compute_tri_hash = [](uint32_t a, uint32_t b, uint32_t c) -> uint64_t {
            uint32_t v[3] = {a, b, c};
            if (v[0] > v[1]) std::swap(v[0], v[1]);
            if (v[1] > v[2]) std::swap(v[1], v[2]);
            if (v[0] > v[1]) std::swap(v[0], v[1]);
            uint64_t h = 1469598103934665603ULL;
            for (int i = 0; i < 3; ++i) {
                h ^= static_cast<uint64_t>(v[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        };

        std::vector<std::vector<uint32_t>> valid_polys;
        std::vector<uint16_t> valid_mats;
        valid_polys.reserve(qmesh.polygons.size());
        valid_mats.reserve(qmesh.material_ids.size());

        for (size_t pi = 0; pi < qmesh.polygons.size(); ++pi) {
            std::vector<uint32_t> p;
            for (uint32_t v : qmesh.polygons[pi]) {
                uint32_t rv = v_remap[v];
                if (p.empty() || p.back() != rv) p.push_back(rv);
            }
            while (p.size() > 1 && p.front() == p.back()) p.pop_back();
            if (p.size() == 4) {
                if (p[0] == p[2] || p[1] == p[3]) continue;
                if (tri_area(p[0], p[1], p[2]) < 1e-13 || tri_area(p[0], p[2], p[3]) < 1e-13) {
                    if (tri_area(p[1], p[2], p[3]) >= 1e-13 && tri_area(p[1], p[3], p[0]) >= 1e-13) {
                        std::rotate(p.begin(), p.begin() + 1, p.end());
                    } else {
                        continue;
                    }
                }
            } else if (p.size() == 3) {
                if (tri_area(p[0], p[1], p[2]) < 1e-13) continue;
            } else {
                continue;
            }
            valid_polys.push_back(std::move(p));
            valid_mats.push_back(pi < qmesh.material_ids.size() ? qmesh.material_ids[pi] : 0);
        }

        // Remove non-manifold faces (edge count > 2) and fan-triangulation duplicates
        std::vector<bool> drop(valid_polys.size(), false);
        std::unordered_set<uint64_t> seen_tris;
        for (size_t pi = 0; pi < valid_polys.size(); ++pi) {
            if (valid_polys[pi].size() == 3) {
                uint64_t th = compute_tri_hash(valid_polys[pi][0], valid_polys[pi][1], valid_polys[pi][2]);
                if (seen_tris.count(th)) drop[pi] = true;
                else seen_tris.insert(th);
            }
        }
        for (size_t pi = 0; pi < valid_polys.size(); ++pi) {
            auto& p = valid_polys[pi];
            if (p.size() == 4) {
                uint64_t th0 = compute_tri_hash(p[0], p[1], p[2]);
                uint64_t th1 = compute_tri_hash(p[0], p[2], p[3]);
                if (seen_tris.count(th0) || seen_tris.count(th1)) {
                    uint64_t th0_r = compute_tri_hash(p[1], p[2], p[3]);
                    uint64_t th1_r = compute_tri_hash(p[1], p[3], p[0]);
                    if (!seen_tris.count(th0_r) && !seen_tris.count(th1_r) &&
                        tri_area(p[1], p[2], p[3]) >= 1e-13 && tri_area(p[1], p[3], p[0]) >= 1e-13) {
                        std::rotate(p.begin(), p.begin() + 1, p.end());
                        th0 = th0_r;
                        th1 = th1_r;
                    } else {
                        drop[pi] = true;
                        continue;
                    }
                }
                seen_tris.insert(th0);
                seen_tris.insert(th1);
            }
        }

        std::unordered_map<uint64_t, std::vector<size_t>> edge_faces;
        for (size_t pi = 0; pi < valid_polys.size(); ++pi) {
            if (drop[pi]) continue;
            const auto& p = valid_polys[pi];
            for (size_t i = 0; i < p.size(); ++i) {
                uint32_t u = p[i], v = p[(i + 1) % p.size()];
                uint64_t k = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
                edge_faces[k].push_back(pi);
            }
        }
        for (const auto& [k, flist] : edge_faces) {
            if (flist.size() > 2) {
                std::vector<size_t> active;
                for (size_t fi : flist) if (!drop[fi]) active.push_back(fi);
                while (active.size() > 2) {
                    // Prefer dropping triangles or smallest-area face
                    size_t worst_idx = 0;
                    for (size_t i = 1; i < active.size(); ++i) {
                        if (valid_polys[active[i]].size() < valid_polys[active[worst_idx]].size()) {
                            worst_idx = i;
                        }
                    }
                    drop[active[worst_idx]] = true;
                    active.erase(active.begin() + worst_idx);
                }
            }
        }

        qmesh.polygons.clear();
        qmesh.material_ids.clear();
        for (size_t pi = 0; pi < valid_polys.size(); ++pi) {
            if (!drop[pi]) {
                qmesh.polygons.push_back(std::move(valid_polys[pi]));
                qmesh.material_ids.push_back(valid_mats[pi]);
            }
        }
    }

    if (ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
        float tol = ctx.params->symmetry_tolerance * 0.01f * ctx.bbox_diag;
        mirror_symmetry(ctx, qmesh, ctx.active_symmetry_axis, tol);
    }

    // Final fill: the exact-position weld / degenerate-face sanitizer above can open
    // holes again after the previous passes
    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 300); log_area("after_fill3"); }

    // Drop microscopic disconnected islands (closed shells below 0.5% of the largest
    // shell's area): leftovers of tiny sliver patches that show up as stray components
    // while carrying no visible geometry.
    {
        size_t np = qmesh.polygons.size();
        if (np > 1) {
            std::vector<uint32_t> parent(np);
            for (size_t i = 0; i < np; ++i) parent[i] = static_cast<uint32_t>(i);
            auto find = [&parent](uint32_t x) {
                uint32_t r = x;
                while (parent[r] != r) r = parent[r];
                while (parent[x] != x) { uint32_t nx = parent[x]; parent[x] = r; x = nx; }
                return r;
            };
            std::unordered_map<uint64_t, uint32_t> owner;
            for (size_t pi = 0; pi < np; ++pi) {
                const auto& p = qmesh.polygons[pi];
                for (size_t k = 0; k < p.size(); ++k) {
                    uint32_t a = p[k], b = p[(k + 1) % p.size()];
                    if (a == b) continue;
                    uint64_t key = (static_cast<uint64_t>(std::min(a, b)) << 32) | std::max(a, b);
                    auto it = owner.find(key);
                    if (it == owner.end()) {
                        owner.emplace(key, static_cast<uint32_t>(pi));
                    } else {
                        uint32_t ra = find(it->second);
                        uint32_t rb = find(static_cast<uint32_t>(pi));
                        if (ra != rb) parent[rb] = ra;
                    }
                }
            }
            std::vector<double> comp_area(np, 0.0);
            for (size_t pi = 0; pi < np; ++pi) {
                const auto& p = qmesh.polygons[pi];
                if (p.size() < 3) continue;
                const Vec3f& a0 = qmesh.positions[p[0]];
                for (size_t k = 1; k + 1 < p.size(); ++k) {
                    Vec3f e1 = qmesh.positions[p[k]] - a0;
                    Vec3f e2 = qmesh.positions[p[k + 1]] - a0;
                    comp_area[find(static_cast<uint32_t>(pi))] += 0.5 * static_cast<double>(e1.cross(e2).norm());
                }
            }
            double max_area = 0.0;
            for (size_t pi = 0; pi < np; ++pi) {
                double a = comp_area[find(static_cast<uint32_t>(pi))];
                if (a > max_area) max_area = a;
            }
            std::vector<std::vector<uint32_t>> polys;
            std::vector<uint16_t> mats;
            polys.reserve(np);
            mats.reserve(np);
            for (size_t pi = 0; pi < np; ++pi) {
                if (comp_area[find(static_cast<uint32_t>(pi))] >= 0.005 * max_area) {
                    polys.push_back(qmesh.polygons[pi]);
                    mats.push_back(pi < qmesh.material_ids.size() ? qmesh.material_ids[pi] : 0);
                }
            }
            size_t dropped = np - polys.size();
            if (dropped > 0 && dropped < np) {
                qmesh.polygons = std::move(polys);
                qmesh.material_ids = std::move(mats);
                std::cout << "[stage6] micro_islands_dropped=" << dropped << std::endl;
            }
        }
    }

    compute_final_quality_report(ctx, qmesh);

    ctx.dump_quads_obj("stage6_optimized", qmesh);

    ctx.notify_progress(1.0f, "Completed");
    return QRM_OK;
}

} // namespace qremesh
