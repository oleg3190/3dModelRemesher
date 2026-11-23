#include "stages/stage6_opt.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <unordered_map>
#include <iterator>
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

    // ---- is_open_border: the model's LEGITIMATE open silhouette -----------------------
    // A boundary loop that lies on the original mesh's open border is NOT a hole - it is
    // the real edge of the surface and must be preserved. Quantize every original boundary
    // vertex (working_mesh.is_boundary_vertex) into a grid of cell = base_target_length and
    // remember which cells are occupied. A candidate loop is "on the open border" when most
    // of its output vertices fall in a border-occupied cell (checked with the 26 neighbours
    // to absorb welding jitter). Border loops are filled only when MICRO (<= 6 verts): a
    // tiny nick in the silhouette is safe to close, a real border is not.
    const float oc = std::max(ctx.base_target_length, 1e-5f);
    auto ocell = [&](const Vec3f& p) -> std::array<int64_t, 3> {
        return { (int64_t)std::llround(p.x() / oc),
                 (int64_t)std::llround(p.y() / oc),
                 (int64_t)std::llround(p.z() / oc) };
    };
    auto okey = [](const std::array<int64_t, 3>& c) -> uint64_t {
        uint64_t h = (uint64_t)(c[0] * 73856093) ^ (uint64_t)(c[1] * 19349663) ^ (uint64_t)(c[2] * 83492791);
        return h;
    };
    std::unordered_set<uint64_t> open_border_cells;
    {
        // ORIGINAL input silhouette = edges that occur in exactly one original triangle.
        // Built from ctx.original_bvh.triangles (positions, pre-weld, pre-QEM) so cracks
        // introduced by working_mesh simplification cannot masquerade as an open border.
        // For a watertight model this set is ~empty -> the guard is inert and real holes
        // (from assembly misses) get filled; for a model with a genuine open edge the
        // silhouette is populated and those loops are preserved.
        std::unordered_map<uint64_t, uint32_t> edge_count;
        std::vector<std::pair<uint64_t,uint64_t>> edge_cells; // (hash_a, hash_b) per unique edge
        std::unordered_map<uint64_t, size_t> edge_slot;        // combined key -> index in edge_cells
        auto cellhash = [&](const Vec3f& a) -> uint64_t { return okey(ocell(a)); };
        auto add_tri = [&](const Vec3f& p0, const Vec3f& p1, const Vec3f& p2) {
            uint64_t hs[3] = { cellhash(p0), cellhash(p1), cellhash(p2) };
            uint32_t vs[3][2] = {{0,1},{1,2},{2,0}};
            for (int k = 0; k < 3; ++k) {
                uint64_t ha = hs[vs[k][0]], hb = hs[vs[k][1]];
                uint64_t lo = std::min(ha,hb), hi = std::max(ha,hb);
                uint64_t comb = lo ^ (hi + 0x9e3779b97f4a7c15ull + (lo<<6) + (lo>>2));
                auto it = edge_slot.find(comb);
                if (it == edge_slot.end()) {
                    edge_slot[comb] = edge_cells.size();
                    edge_cells.push_back({lo,hi});
                    edge_count[comb] = 1;
                } else {
                    edge_count[comb]++;
                }
            }
        };
        // original_bvh is built AFTER stage0 non-manifold repair. Its new single-face edges
        // are repair-induced boundaries, not the input's legitimate open silhouette. Use
        // raw input topology by default; set QREMESH_HOLE_ORIGINPUT=0 for legacy behavior.
        bool raw_silhouette = true;
        if (const char* e = std::getenv("QREMESH_HOLE_ORIGINPUT")) raw_silhouette = (std::atoi(e) != 0);
        if (raw_silhouette && ctx.in_mesh) {
            const QrmMeshIn* in = ctx.in_mesh;
            auto P = [&](uint32_t vi) -> Vec3f {
                return Vec3f(in->positions[3u * vi], in->positions[3u * vi + 1],
                             in->positions[3u * vi + 2]);
            };
            for (uint32_t t = 0; t < in->nt; ++t) {
                const uint32_t* ti = in->tri_indices + 3u * t;
                add_tri(P(ti[0]), P(ti[1]), P(ti[2]));
            }
        } else {
            for (const auto& t : ctx.original_bvh.triangles_ref()) add_tri(t.v0, t.v1, t.v2);
        }
        // insert the endpoint cells of every single-face (silhouette) edge
        for (const auto& sl : edge_slot) {
            if (edge_count[sl.first] == 1) {
                const auto& ec = edge_cells[sl.second];
                open_border_cells.insert(ec.first);
                open_border_cells.insert(ec.second);
            }
        }
    }
    auto vertex_on_open_border = [&](uint32_t v) -> bool {
        auto c = ocell(qmesh.positions[v]);
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dz = -1; dz <= 1; ++dz)
                    if (open_border_cells.count(okey({c[0] + dx, c[1] + dy, c[2] + dz})))
                        return true;
        return false;
    };

    // Find and trace simple boundary loops
    std::unordered_map<uint32_t, std::vector<uint32_t>> bnd_adj;
    for (const auto& [u, v] : bnd_edges) {
        bnd_adj[u].push_back(v);
        bnd_adj[v].push_back(u);
    }

    size_t holes_filled = 0;
    size_t border_preserved = 0;
    size_t rejected_too_big = 0;
    size_t rejected_non_simple = 0;

    // The directed boundary walk handles loops that touch at pinch vertices. Keep a
    // conservative 64-vertex cap; both settings remain overridable for A/B testing.
    // QREMESH_HOLE_LIMIT=<n> changes the cap; QREMESH_HOLE_PINCH=0 restores legacy tracing.
    size_t hole_limit = max_hole_verts;
    if (const char* e = std::getenv("QREMESH_HOLE_LIMIT")) {
        const int v = std::atoi(e);
        if (v >= 4) hole_limit = static_cast<size_t>(v);
    }
    bool pinch_mode = true;
    if (const char* e = std::getenv("QREMESH_HOLE_PINCH")) pinch_mode = (std::atoi(e) != 0);

    // Fill ONE traced loop: legit-open-border guard, then a quad fan around a source-snapped
    // centroid. Factored out of the naive walk so the directed (pinch) walker reuses it.
    auto fill_loop = [&](const std::vector<uint32_t>& loop) {
        if (loop.size() < 3 || loop.size() > hole_limit) return;
        // Preserve raw-input openings only when the caller requests open borders.
        // For AI assets that must become closed solids, setting this option false lets
        // Stage 6 repair source holes as well (Stage 0 uses the same option).
        if (ctx.params->preserve_open_borders) {
            size_t on_border = 0;
            for (uint32_t lv : loop) if (vertex_on_open_border(lv)) on_border++;
            const size_t MIN_BORDER_LOOP = 12;
            if (on_border == loop.size() && loop.size() >= MIN_BORDER_LOOP) {
                border_preserved++;
                return;
            }
        }
        // Use the actual ordered loop edges. At a pinch vertex bnd_adj[lv][0] may belong
        // to a different sub-loop, producing a wrong cap winding for figure-eight boundaries.
        Vec3f avg_normal = Vec3f::Zero();
        for (size_t i = 0; i < loop.size(); ++i) {
            const uint32_t lv = loop[i];
            const uint32_t nbr_v = loop[(i + 1) % loop.size()];
            const uint64_t k = (static_cast<uint64_t>(std::min(lv, nbr_v)) << 32) | std::max(lv, nbr_v);
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
            float d2 = 0.0f;
            // Avoid snapping the cap center onto a different nearby sheet (thin walls and
            // self-touching AI meshes). Snap only when the source is close and normals agree.
            const float snap_limit_sq = 0.25f * ctx.base_target_length * ctx.base_target_length;
            if (ctx.original_bvh.closest_point(c, cp, cn, d2) &&
                d2 <= snap_limit_sq && cn.dot(avg_normal) > 0.5f) {
                c = cp;
            }
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
    };

    std::unordered_set<uint32_t> visited_v;
    if (!pinch_mode) {
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
                rejected_non_simple++;
                break;
            }
            uint32_t nxt = (cur_nbrs[0] == prev) ? cur_nbrs[1] : cur_nbrs[0];
            if (nxt == start_v) {
                break; // Closed loop found!
            }
            if (visited_v.count(nxt)) {
                is_simple_loop = false;
                rejected_non_simple++;
                break;
            }
            prev = curr;
            curr = nxt;
            if (loop.size() > hole_limit) {
                is_simple_loop = false;
                rejected_too_big++;
                break;
            }
        }

        if (is_simple_loop && loop.size() >= 3 && loop.size() <= hole_limit) {
            fill_loop(loop);
        }
    }
    } // if (!pinch_mode)

    if (pinch_mode) {
        // Directed boundary walk: every boundary edge is oriented along its single face's
        // winding, and next(a,b) rotates around b through the incident faces until a
        // boundary edge leaves b. Two loops touching at a degree-4 vertex get separate
        // walks here instead of being rejected as one non-simple blob by the naive walk.
        struct DE { uint32_t a, b, f; };
        auto pack = [](uint32_t a, uint32_t b) -> uint64_t {
            return (static_cast<uint64_t>(a) << 32) | b;
        };
        std::vector<DE> des;
        std::unordered_map<uint64_t, size_t> de_idx;
        des.reserve(bnd_edges.size());
        for (const auto& [u, v] : bnd_edges) {
            const uint64_t ek = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            auto it = edge_to_faces.find(ek);
            if (it == edge_to_faces.end() || it->second.size() != 1) continue;
            const uint32_t f = it->second[0];
            const auto& poly = qmesh.polygons[f];
            const size_t n = poly.size();
            uint32_t a = u, b = v;
            bool wound = false;
            for (size_t i = 0; i < n; ++i)
                if (poly[i] == u && poly[(i + 1) % n] == v) { wound = true; break; }
            if (!wound) { a = v; b = u; }
            de_idx[pack(a, b)] = des.size();
            des.push_back(DE{a, b, f});
        }
        auto succ = [&](uint32_t f, uint32_t x, uint32_t y) -> uint32_t {
            const auto& poly = qmesh.polygons[f];
            const size_t n = poly.size();
            for (size_t i = 0; i < n; ++i)
                if (poly[i] == x && poly[(i + 1) % n] == y) return poly[(i + 2) % n];
            return INVALID_INDEX;
        };
        auto next_dir = [&](const DE& e, uint32_t& na, uint32_t& nb) -> bool {
            uint32_t f = e.f;
            uint32_t c = succ(f, e.a, e.b);
            if (c == INVALID_INDEX) return false;
            for (int step = 0; step < 32; ++step) {
                const uint64_t ek = (static_cast<uint64_t>(std::min(e.b, c)) << 32) | std::max(e.b, c);
                auto it = edge_to_faces.find(ek);
                if (it != edge_to_faces.end() && it->second.size() == 1) {
                    na = e.b; nb = c;   // f winds a->b->c, so the directed edge is b->c
                    return true;
                }
                uint32_t g = INVALID_INDEX;
                if (it != edge_to_faces.end())
                    for (uint32_t x : it->second) if (x != f) { g = x; break; }
                if (g == INVALID_INDEX) return false;
                const uint32_t d = succ(g, c, e.b);   // g winds c->b->d
                if (d == INVALID_INDEX) return false;
                f = g;
                c = d;
            }
            return false;
        };
        std::vector<uint8_t> used(des.size(), 0);
        for (size_t i = 0; i < des.size(); ++i) {
            if (used[i]) continue;
            std::vector<uint32_t> loop;
            bool ok = true;
            size_t cur = i;
            while (true) {
                if (used[cur]) { ok = false; break; }
                used[cur] = 1;
                loop.push_back(des[cur].a);
                uint32_t na = 0, nb = 0;
                if (!next_dir(des[cur], na, nb)) { ok = false; break; }
                auto nit = de_idx.find(pack(na, nb));
                if (nit == de_idx.end()) { ok = false; break; }
                if (nit->second == i) break;          // closed back to the start
                cur = nit->second;
            }
            if (!ok) { rejected_non_simple++; continue; }
            // A closed walk can pass a pinch vertex twice (figure-eight): split it into
            // simple sub-loops before filling - a fan over a REPEATED vertex puts 4 faces
            // on one radial edge (seen on the S57 model: nm=1 after the first pinch run).
            std::vector<uint32_t> stack;
            std::unordered_map<uint32_t, size_t> pos;
            auto emit = [&](const std::vector<uint32_t>& sub) {
                if (sub.size() < 3) return;
                if (sub.size() > hole_limit) { rejected_too_big++; return; }
                fill_loop(sub);
            };
            for (uint32_t v : loop) {
                auto pit = pos.find(v);
                if (pit != pos.end()) {
                    std::vector<uint32_t> sub(stack.begin() + pit->second, stack.end());
                    for (size_t k = pit->second + 1; k < stack.size(); ++k) pos.erase(stack[k]);
                    stack.resize(pit->second + 1);   // back at v; v stays on the stack
                    emit(sub);
                } else {
                    pos[v] = stack.size();
                    stack.push_back(v);
                }
            }
            emit(stack);
        }
    }

    std::cout << "[stage6] small boundary holes filled: " << holes_filled
              << " border_loops_preserved: " << border_preserved
              << " rejected(too_big=" << rejected_too_big
              << " non_simple=" << rejected_non_simple << ")"
              << " limit=" << hole_limit
              << " pinch=" << (pinch_mode ? 1 : 0) << std::endl;
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
    // Conservative defaults retain local relief. Environment values are validated and
    // bounded so accidental values cannot turn this pass into an unrestricted smoother.
    float step_factor = 0.20f;
    if (const char* e = std::getenv("QREMESH_RELAX_STEP")) {
        const float v = static_cast<float>(std::atof(e));
        if (v > 0.0f && v <= 0.4f) step_factor = v;
    }
    float slide_cap_ratio = 0.35f;
    if (const char* e = std::getenv("QREMESH_RELAX_CAP")) {
        const float v = static_cast<float>(std::atof(e));
        if (v > 0.0f && v <= 1.0f) slide_cap_ratio = v;
    }

    // Forward Laplacian relaxation with an ANCHOR cap: every vertex keeps its position
    // from the start of the relaxation as an anchor and may travel at most slide_cap_ratio
    // of its shortest incident edge. The cap prevents migration along and erosion of small
    // features (rails, brackets, rims) while staying on the source surface.
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
                float c = slide_cap_ratio * len;
                if (slide_cap[u] <= 0.0f || c < slide_cap[u]) slide_cap[u] = c;
                if (slide_cap[w] <= 0.0f || c < slide_cap[w]) slide_cap[w] = c;
            }
        }
    }

    for (int iter = 0; iter < effective_iters; ++iter) {
        // Forward (lambda) step only: the mu counter-step was tried and it amplifies
        // micro-irregularities -> the surface crinkles (area +20%) without moving off
        // the source. The anchor cap below already protects against shrinkage/erosion.
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

// ---------------------------------------------------------------------------
// T7 (BUG-01): localized smooth-crease repair.
//
// The detector is ANALYTIC 3D, no screenshot: an output face is a repair target only when
//   (a) it is creased  - max dihedral over its edges > OUT_DIH (default 120 deg), AND
//   (b) the ORIGINAL source under it is locally smooth - max source-triangle dihedral within
//       R = REL * bbox_diag (default 1%) < SRC_GATE (default 30 deg).
// Gate (b) is the whole point: on a noisy AI mesh 87% of creased output faces sit on REAL
// relief (1707 of 1896 in the ADR measurement); smoothing them would erase detail. Only the
// remaining gouges on genuinely smooth source (the BUG-01 arrows) are touched.
//
// The repair moves ONLY grid-interior vertices of target faces (valence 4, not on an open
// border, not on a sharp/feature edge = Dirichlet borders and features stay pinned), slides
// them out of the fold onto the local smooth plane, snaps them back on-surface with
// original_bvh, and every move is gated by an anchor slide-cap + a global area band + a
// no-SJ-regression test (invariant #7). Deterministic: serial, index-ordered, hash maps are
// only point-looked-up, never iterated.
// ---------------------------------------------------------------------------
void Stage6Optimize::localized_smooth_crease_repair(RemeshContext& ctx, QuadMeshResult& qmesh) {
    const size_t nv = qmesh.positions.size();
    const size_t nf = qmesh.polygons.size();
    if (nv < 4 || nf < 2) return;

    // --- tunables (env; the ON/OFF gate lives in execute(), default OFF = baseline) ------
    float out_dih_deg = 120.0f;
    float src_gate_deg = 30.0f;
    float rel = 0.01f;
    int iters = 8;
    float step_beta = 1.0f;      // fraction of the out-of-plane offset removed per attempt
    if (const char* e = std::getenv("QREMESH_T7_OUT_DIH"))  { float v = std::atof(e); if (v > 0) out_dih_deg = v; }
    if (const char* e = std::getenv("QREMESH_T7_SRC_GATE")) { float v = std::atof(e); if (v > 0) src_gate_deg = v; }
    if (const char* e = std::getenv("QREMESH_T7_REL"))      { float v = std::atof(e); if (v > 0) rel = v; }
    if (const char* e = std::getenv("QREMESH_T7_ITERS"))    { int v = std::atoi(e); if (v >= 0) iters = v; }
    if (const char* e = std::getenv("QREMESH_T7_STEP"))     { float v = std::atof(e); if (v > 0 && v <= 1) step_beta = v; }

    auto deg_of = [](float c) { return std::acos(std::max(-1.0f, std::min(1.0f, c))) * 180.0f / PI_F; };

    // =========================================================================
    // 1) OUTPUT adjacency + face normals + per-face max dihedral
    // =========================================================================
    auto ekey = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };

    // dir0/dir1: +1 if the face traverses the edge min->max, -1 if max->min. Two faces on a
    // consistently oriented manifold must disagree; agreement = a WINDING CONFLICT, which would
    // make a signed-normal dihedral of ~180 deg meaningless (coplanar faces would read as folds).
    struct OutEdge { uint32_t f0 = INVALID_INDEX; uint32_t f1 = INVALID_INDEX;
                     int8_t dir0 = 0, dir1 = 0; };
    std::unordered_map<uint64_t, OutEdge> out_edges;
    out_edges.reserve(nf * 2);
    std::unordered_set<uint64_t> nm_edges;   // >2 incident faces: ambiguous, never a crease cue

    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& p = qmesh.polygons[fi];
        const size_t pn = p.size();
        if (pn < 3) continue;
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = p[i], b = p[(i + 1) % pn];
            if (a == b || a >= nv || b >= nv) continue;
            uint64_t k = ekey(a, b);
            const int8_t dir = (a < b) ? 1 : -1;
            auto it = out_edges.find(k);
            if (it == out_edges.end()) {
                OutEdge oe; oe.f0 = static_cast<uint32_t>(fi); oe.dir0 = dir;
                out_edges.emplace(k, oe);
            } else if (it->second.f1 == INVALID_INDEX && nm_edges.count(k) == 0) {
                if (it->second.f0 != static_cast<uint32_t>(fi)) {
                    it->second.f1 = static_cast<uint32_t>(fi);
                    it->second.dir1 = dir;
                }
            } else {
                nm_edges.insert(k);
            }
        }
    }

    // Newell normal per polygon (robust on non-planar quads)
    auto newell = [&](size_t fi) -> Vec3f {
        const auto& p = qmesh.polygons[fi];
        Vec3d n = Vec3d::Zero();
        const size_t pn = p.size();
        for (size_t i = 0; i < pn; ++i) {
            Vec3d a = qmesh.positions[p[i]].cast<double>();
            Vec3d b = qmesh.positions[p[(i + 1) % pn]].cast<double>();
            n += a.cross(b);
        }
        double ln = n.norm();
        if (ln < 1e-14) return Vec3f(0.0f, 0.0f, 0.0f);
        return (n / ln).cast<float>();
    };

    std::vector<Vec3f> fnorm(nf);
    for (size_t fi = 0; fi < nf; ++fi) fnorm[fi] = newell(fi);

    // vertex -> incident faces / incident edges / ring neighbours
    // NOTE: v_edges holds the 64-bit packed edge keys (uint32_t would truncate -> lookups
    // miss -> every vertex looks flat; that bug silently stalled the whole pass once).
    std::vector<std::vector<uint32_t>> v_faces(nv), v_ring(nv);
    std::vector<std::vector<uint64_t>> v_edges(nv);
    std::vector<uint8_t> on_border(nv, 0);
    for (const auto& [k, oe] : out_edges) {
        uint32_t a = static_cast<uint32_t>(k >> 32);
        uint32_t b = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        v_edges[a].push_back(k);
        v_edges[b].push_back(k);
        v_ring[a].push_back(b);
        v_ring[b].push_back(a);
        if (oe.f1 == INVALID_INDEX) { on_border[a] = 1; on_border[b] = 1; }
        // vertex -> incident faces (a faces list per VERTEX; building it the other way round
        // makes every vertex->face lookup read vertex ids as face indices -> wild memory)
        if (oe.f0 != INVALID_INDEX && oe.f0 < nf) { v_faces[a].push_back(oe.f0); v_faces[b].push_back(oe.f0); }
        if (oe.f1 != INVALID_INDEX && oe.f1 < nf) { v_faces[a].push_back(oe.f1); v_faces[b].push_back(oe.f1); }
    }
    for (size_t v = 0; v < nv; ++v) {
        std::sort(v_faces[v].begin(), v_faces[v].end());
        v_faces[v].erase(std::unique(v_faces[v].begin(), v_faces[v].end()), v_faces[v].end());
        std::sort(v_ring[v].begin(), v_ring[v].end());
        v_ring[v].erase(std::unique(v_ring[v].begin(), v_ring[v].end()), v_ring[v].end());
        std::sort(v_edges[v].begin(), v_edges[v].end());
        v_edges[v].erase(std::unique(v_edges[v].begin(), v_edges[v].end()), v_edges[v].end());
    }

    auto edge_dih = [&](uint64_t k) -> float {
        auto it = out_edges.find(k);
        if (it == out_edges.end()) return 0.0f;
        if (it->second.f0 == INVALID_INDEX || it->second.f1 == INVALID_INDEX) return 0.0f;
        return deg_of(fnorm[it->second.f0].dot(fnorm[it->second.f1]));
    };
    auto face_max_dih = [&](uint32_t fi) -> float {
        const auto& p = qmesh.polygons[fi];
        const size_t pn = p.size();
        float m = 0.0f;
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = p[i], b = p[(i + 1) % pn];
            if (a == b) continue;
            float d = edge_dih(ekey(a, b));
            if (d > m) m = d;
        }
        return m;
    };
    auto vert_max_dih = [&](uint32_t v) -> float {
        float m = 0.0f;
        for (uint64_t k : v_edges[v]) {
            float d = edge_dih(k);
            if (d > m) m = d;
        }
        return m;
    };

    // Winding audit of the detector's assumption (measured here, not assumed): dihedral is a
    // SIGNED dot of face normals, so it only means "fold" while the output stays consistently
    // oriented. 0 conflicts => a ~180 deg reading is a real pleat, not flipped winding.
    size_t winding_conflicts = 0;
    for (const auto& [k, oe] : out_edges) {
        if (oe.f0 == INVALID_INDEX || oe.f1 == INVALID_INDEX) continue;
        if (oe.dir0 != 0 && oe.dir1 != 0 && oe.dir0 == oe.dir1) winding_conflicts++;
    }

    // =========================================================================
    // 2) SOURCE local roughness (analytic, from original_bvh triangles)
    // =========================================================================
    struct PKey { int64_t x, y, z; bool operator==(const PKey& o) const { return x==o.x && y==o.y && z==o.z; } };
    struct PKeyHash {
        size_t operator()(const PKey& k) const {
            size_t h = static_cast<size_t>(k.x) * 73856093ULL;
            h ^= static_cast<size_t>(k.y) * 19349663ULL;
            h ^= static_cast<size_t>(k.z) * 83492791ULL;
            return h;
        }
    };
    const float Q = 1e6f;
    auto pq = [&](const Vec3f& p) -> PKey {
        return PKey{ std::llround(static_cast<double>(p.x()) * Q),
                     std::llround(static_cast<double>(p.y()) * Q),
                     std::llround(static_cast<double>(p.z()) * Q) };
    };

    const auto& src_tris = ctx.original_bvh.triangles_ref();
    const size_t nt = src_tris.size();
    std::vector<uint32_t> tri_vid(nt * 3, INVALID_INDEX);
    std::vector<Vec3f> src_v;
    src_v.reserve(nt * 3);
    std::unordered_map<PKey, uint32_t, PKeyHash> src_vmap;
    if (nt)
        src_vmap.reserve(nt * 3);

    struct SrcEdge { uint32_t t0 = INVALID_INDEX; uint32_t t1 = INVALID_INDEX; };
    std::unordered_map<uint64_t, SrcEdge> src_edges;
    if (nt)
        src_edges.reserve(nt * 2);
    std::unordered_set<uint64_t> src_nm;

    auto get_vid = [&](const Vec3f& p) -> uint32_t {
        PKey k = pq(p);
        auto it = src_vmap.find(k);
        if (it != src_vmap.end()) return it->second;
        uint32_t id = static_cast<uint32_t>(src_v.size());
        src_v.push_back(p);
        src_vmap.emplace(k, id);
        return id;
    };

    for (size_t ti = 0; ti < nt; ++ti) {
        const BVHTriangle& t = src_tris[ti];
        const Vec3f* cp = &t.v0;
        for (int i = 0; i < 3; ++i) tri_vid[ti * 3 + i] = get_vid(cp[i]);
        for (int i = 0; i < 3; ++i) {
            uint32_t a = tri_vid[ti * 3 + i], b = tri_vid[ti * 3 + (i + 1) % 3];
            if (a == b) continue;
            uint64_t k = ekey(a, b);
            auto it = src_edges.find(k);
            if (it == src_edges.end()) {
                SrcEdge se; se.t0 = static_cast<uint32_t>(ti);
                src_edges.emplace(k, se);
            } else if (it->second.t1 == INVALID_INDEX && src_nm.count(k) == 0) {
                if (it->second.t0 != static_cast<uint32_t>(ti)) it->second.t1 = static_cast<uint32_t>(ti);
            } else {
                src_nm.insert(k);
            }
        }
    }

    // per-source-triangle max dihedral with its edge neighbours (winding as-is, same
    // convention as the reference python detector test_out/smooth_creases.py)
    std::vector<float> tri_dih(nt, 0.0f);
    for (const auto& [k, se] : src_edges) {
        if (se.t0 == INVALID_INDEX || se.t1 == INVALID_INDEX) continue;
        float d = deg_of(src_tris[se.t0].normal.dot(src_tris[se.t1].normal));
        if (d > tri_dih[se.t0]) tri_dih[se.t0] = d;
        if (d > tri_dih[se.t1]) tri_dih[se.t1] = d;
    }

    const float R = std::max(1e-5f, rel * ctx.bbox_diag);
    auto cellkey = [&](int64_t x, int64_t y, int64_t z) -> uint64_t {
        // spatial hash of grid cells (lookups only; never iterated)
        auto mix = [](int64_t v) { return static_cast<uint64_t>(v + 0x9e3779b97f4a7c15ULL); };
        return mix(x) ^ (mix(y) * 0x9e3779b97f4a7c15ULL) ^ (mix(z) * 0xc2b2ae3d27d4eb4fULL);
    };
    const double invR = 1.0 / static_cast<double>(R);
    std::unordered_map<uint64_t, std::vector<uint32_t>> src_grid;
    std::vector<Vec3f> tri_c(nt);
    for (size_t ti = 0; ti < nt; ++ti) {
        const BVHTriangle& t = src_tris[ti];
        Vec3f c = (t.v0 + t.v1 + t.v2) * (1.0f / 3.0f);
        tri_c[ti] = c;
        int64_t gx = static_cast<int64_t>(std::floor(c.x() * invR));
        int64_t gy = static_cast<int64_t>(std::floor(c.y() * invR));
        int64_t gz = static_cast<int64_t>(std::floor(c.z() * invR));
        src_grid[cellkey(gx, gy, gz)].push_back(static_cast<uint32_t>(ti));
    }

    auto src_rough_at = [&](const Vec3f& p) -> float {
        int64_t gx = static_cast<int64_t>(std::floor(p.x() * invR));
        int64_t gy = static_cast<int64_t>(std::floor(p.y() * invR));
        int64_t gz = static_cast<int64_t>(std::floor(p.z() * invR));
        float m = 0.0f;
        const double r2 = static_cast<double>(R) * R;
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                for (int dz = -1; dz <= 1; ++dz) {
                    auto it = src_grid.find(cellkey(gx + dx, gy + dy, gz + dz));
                    if (it == src_grid.end()) continue;
                    for (uint32_t ti : it->second) {
                        Vec3f d = tri_c[ti] - p;
                        if (d.squaredNorm() <= r2 && tri_dih[ti] > m) m = tri_dih[ti];
                    }
                }
        return m;
    };

    // =========================================================================
    // 3) Target mask: creased output face x locally smooth source
    // =========================================================================
    std::vector<uint8_t> is_target(nf, 0);
    size_t creased = 0, targets = 0;
    for (size_t fi = 0; fi < nf; ++fi) {
        if (qmesh.polygons[fi].size() != 4) continue;
        if (face_max_dih(static_cast<uint32_t>(fi)) <= out_dih_deg) continue;
        creased++;
        const auto& p = qmesh.polygons[fi];
        Vec3f c = Vec3f::Zero();
        for (uint32_t v : p) c += qmesh.positions[v];
        c *= 0.25f;
        if (src_rough_at(c) < src_gate_deg) {
            is_target[fi] = 1;
            targets++;
        }
    }

    // Movable set: grid-interior nodes of target faces only.
    // Pinned: open-border vertices (Dirichlet), sharp/feature-edge vertices, valence != 4.
    std::vector<uint8_t> movable(nv, 0);
    std::unordered_set<uint64_t> sharp_set;
    sharp_set.reserve(qmesh.sharp_edges.size() * 2);
    for (const auto& e : qmesh.sharp_edges) {
        if (e.first < nv && e.second < nv && e.first != e.second)
            sharp_set.insert(ekey(e.first, e.second));
    }
    std::vector<uint8_t> on_sharp(nv, 0);
    for (uint64_t k : sharp_set) {
        uint32_t a = static_cast<uint32_t>(k >> 32), b = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        on_sharp[a] = 1; on_sharp[b] = 1;
    }
    // Pin reasons (telemetry): a target-face corner may not move if it is on the open
    // border (Dirichlet), on a sharp/feature edge, or not a regular grid node (valence != 4).
    auto pin_reason = [&](uint32_t v) -> int {
        if (v >= nv) return 4;
        if (on_border[v]) return 1;
        if (on_sharp[v]) return 2;
        if (v_faces[v].size() != 4 || v_ring[v].size() != 4) return 3;
        return 0;
    };
    for (size_t fi = 0; fi < nf; ++fi) {
        if (!is_target[fi]) continue;
        for (uint32_t v : qmesh.polygons[fi]) {
            if (v < nv && pin_reason(v) == 0) movable[v] = 1;
        }
    }
    size_t n_movable = 0;
    for (size_t v = 0; v < nv; ++v) n_movable += movable[v] ? 1 : 0;
    // Crease-edge endpoints: what actually pins the fold? (diagnosis of a stalled pass)
    size_t pin_border = 0, pin_sharp = 0, pin_valence = 0, crease_ep_movable = 0, crease_edges = 0;
    for (const auto& [k, oe] : out_edges) {
        if (oe.f0 == INVALID_INDEX || oe.f1 == INVALID_INDEX) continue;
        if (edge_dih(k) <= out_dih_deg) continue;
        if (!is_target[oe.f0] && !is_target[oe.f1]) continue;
        crease_edges++;
        uint32_t a = static_cast<uint32_t>(k >> 32), b = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        for (uint32_t v : {a, b}) {
            switch (pin_reason(v)) {
                case 0: crease_ep_movable++; break;
                case 1: pin_border++; break;
                case 2: pin_sharp++; break;
                default: pin_valence++; break;
            }
        }
    }

    if (targets == 0 || n_movable == 0) {
        std::cout << "[stage6/t7] creased=" << creased << " target_faces=" << targets
                  << " movable_verts=" << n_movable << " src_tris=" << nt
                  << " winding_conflicts=" << winding_conflicts
                  << " gate(out>" << out_dih_deg << ",src<" << src_gate_deg
                  << ",R=" << rel << ") moved=0 (nothing to repair)" << std::endl;
        std::cout << "[stage6/t7/pins] crease_edges=" << crease_edges
                  << " endpoint_movable=" << crease_ep_movable
                  << " pinned_border=" << pin_border << " pinned_sharp=" << pin_sharp
                  << " pinned_valence=" << pin_valence << std::endl;
        return;
    }

    // =========================================================================
    // 4) Localized anti-fold / planarize pass
    // =========================================================================
    const std::vector<Vec3f> anchor = qmesh.positions;
    std::vector<float> slide_cap(nv, 0.0f);   // 1.0 x shortest incident edge (as untangle)
    for (const auto& [k, oe] : out_edges) {
        uint32_t a = static_cast<uint32_t>(k >> 32);
        uint32_t b = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        float len = (anchor[a] - anchor[b]).norm();
        if (len <= 1e-9f) continue;
        if (slide_cap[a] <= 0.0f || len < slide_cap[a]) slide_cap[a] = len;
        if (slide_cap[b] <= 0.0f || len < slide_cap[b]) slide_cap[b] = len;
    }

    auto poly_area_of = [&](uint32_t v) -> double {
        double a = 0.0;
        for (uint32_t fi : v_faces[v]) {
            const auto& p = qmesh.polygons[fi];
            if (p.size() < 3) continue;
            const Vec3f& a0 = qmesh.positions[p[0]];
            for (size_t k = 1; k + 1 < p.size(); ++k) {
                Vec3f e1 = qmesh.positions[p[k]] - a0;
                Vec3f e2 = qmesh.positions[p[k + 1]] - a0;
                a += 0.5 * static_cast<double>(e1.cross(e2).norm());
            }
        }
        return a;
    };
    double global_area = 0.0;
    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& p = qmesh.polygons[fi];
        if (p.size() < 3) continue;
        const Vec3f& a0 = qmesh.positions[p[0]];
        for (size_t k = 1; k + 1 < p.size(); ++k) {
            Vec3f e1 = qmesh.positions[p[k]] - a0;
            Vec3f e2 = qmesh.positions[p[k + 1]] - a0;
            global_area += 0.5 * static_cast<double>(e1.cross(e2).norm());
        }
    }
    const double area_lo = 0.94 * global_area;
    const double area_hi = 1.05 * global_area;

    auto min_incident_sj = [&](uint32_t v) -> float {
        float m = 1.0f;
        for (uint32_t fi : v_faces[v]) {
            const auto& p = qmesh.polygons[fi];
            if (p.size() != 4) continue;
            m = std::min(m, compute_scaled_jacobian(
                qmesh.positions[p[0]], qmesh.positions[p[1]],
                qmesh.positions[p[2]], qmesh.positions[p[3]]));
        }
        return m;
    };
    auto refresh_incident = [&](uint32_t v) {
        for (uint32_t fi : v_faces[v]) fnorm[fi] = newell(fi);
    };

    auto face_area = [&](uint32_t fi) -> double {
        const auto& p = qmesh.polygons[fi];
        if (p.size() < 3) return 0.0;
        const Vec3f& a0 = qmesh.positions[p[0]];
        double a = 0.0;
        for (size_t k = 1; k + 1 < p.size(); ++k) {
            Vec3f e1 = qmesh.positions[p[k]] - a0;
            Vec3f e2 = qmesh.positions[p[k + 1]] - a0;
            a += 0.5 * static_cast<double>(e1.cross(e2).norm());
        }
        return a;
    };

    // Pleat unfold (family 2): the BUG-01 gouges are NOT out-of-plane bumps - measured, the
    // fold pair lies on the SAME side of its shared edge (one quad folded back inside the
    // other, dihedral ~180 deg). Unfolding = rotate the smaller flap RIGIDLY about the crease
    // edge axis by the fold angle (the "open the book flat" motion). A rigid rotation keeps
    // the flap's winding, unlike a mirror reflection through the edge line, which flips the
    // flap inside out (measured: reflect dropped the fold 179.9->21 deg but SJ 0.22 -> -0.33).
    // Afterwards the vertices are snapped back onto the source (locally planar by the gate).
    struct MoveSet {
        std::vector<uint32_t> verts, faces;
        std::vector<uint64_t> edges;
    };
    auto collect_move_set = [&](std::initializer_list<uint32_t> vs) -> MoveSet {
        MoveSet ms;
        for (uint32_t v : vs) {
            ms.verts.push_back(v);
            for (uint32_t fi : v_faces[v]) ms.faces.push_back(fi);
            for (uint64_t ek : v_edges[v]) ms.edges.push_back(ek);
        }
        std::sort(ms.faces.begin(), ms.faces.end());
        ms.faces.erase(std::unique(ms.faces.begin(), ms.faces.end()), ms.faces.end());
        std::sort(ms.edges.begin(), ms.edges.end());
        ms.edges.erase(std::unique(ms.edges.begin(), ms.edges.end()), ms.edges.end());
        return ms;
    };
    auto set_crease_max = [&](const MoveSet& ms) -> float {
        float m = 0.0f;
        for (uint64_t e : ms.edges) m = std::max(m, edge_dih(e));
        return m;
    };
    auto set_min_sj = [&](const MoveSet& ms) -> float {
        float m = 1.0f;
        for (uint32_t f : ms.faces) {
            const auto& p = qmesh.polygons[f];
            if (p.size() != 4) continue;
            m = std::min(m, compute_scaled_jacobian(
                qmesh.positions[p[0]], qmesh.positions[p[1]],
                qmesh.positions[p[2]], qmesh.positions[p[3]]));
        }
        return m;
    };
    auto set_area = [&](const MoveSet& ms) -> double {
        double a = 0.0;
        for (uint32_t f : ms.faces) a += face_area(f);
        return a;
    };

    size_t d_unfold_try = 0, d_unfold_ok = 0, d_rej_pin = 0;
    size_t d_unfold_rej_fold = 0, d_unfold_rej_sj = 0, d_unfold_rej_area = 0;
    const float cap_scale = []() {
        float c = 1.6f;
        if (const char* e = std::getenv("QREMESH_T7_CAP")) { float v = std::atof(e); if (v > 0) c = v; }
        return c;
    }();
    const int family_mask = []() {
        int m = 3;   // bit0 = planarize family, bit1 = pleat-reflect family
        if (const char* e = std::getenv("QREMESH_T7_MODE")) m = std::atoi(e);
        return m;
    }();
    size_t moves = 0;
    // Rejection-reason counters (QREMESH_T7_DEBUG=1) - where the repair actually stalls.
    const bool dbg = (std::getenv("QREMESH_T7_DEBUG") != nullptr);
    const bool loose = (std::getenv("QREMESH_T7_LOOSE") != nullptr);
    size_t d_target_active = 0, d_corner = 0, d_no_fold = 0, d_tiny_h = 0;
    size_t d_rej_cap = 0, d_rej_dih = 0, d_rej_sj = 0, d_rej_area = 0, d_no_bvh = 0, d_sample = 0;
    for (int it = 0; it < iters; ++it) {
        bool any = false;
        for (size_t fi = 0; fi < nf; ++fi) {
            if (!is_target[fi]) continue;
            if (face_max_dih(static_cast<uint32_t>(fi)) <= out_dih_deg) continue;  // healed
            const auto& poly = qmesh.polygons[fi];
            if (poly.size() != 4) continue;

            // local smooth plane: face centroid snapped onto the source, source normal
            Vec3f c = Vec3f::Zero();
            for (uint32_t v : poly) c += qmesh.positions[v];
            c *= 0.25f;
            Vec3f sn, cp;
            float d2;
            if (!ctx.original_bvh.closest_point(c, cp, sn, d2)) { d_no_bvh++; continue; }
            if (sn.squaredNorm() < 1e-12f) continue;
            sn.normalize();
            if (sn.dot(fnorm[fi]) < 0.0f) sn = -sn;

            if (family_mask & 1) {
                for (int k = 0; k < 4; ++k) {
                    uint32_t v = poly[k];
                    if (v >= nv || !movable[v]) continue;
                    d_corner++;

                    const float d_before = vert_max_dih(v);
                    if (d_before <= out_dih_deg) { d_no_fold++; continue; }   // not in a fold
                    const float sj_before = min_incident_sj(v);
                    const double a_before = poly_area_of(v);
                    const Vec3f orig_p = qmesh.positions[v];

                    // ring centroid = the flat neighbourhood the fold punched through
                    Vec3f g = Vec3f::Zero();
                    for (uint32_t nb : v_ring[v]) g += qmesh.positions[nb];
                    g /= static_cast<float>(v_ring[v].size());
                    const float h = (orig_p - g).dot(sn);   // out-of-plane offset of the apex
                    if (std::fabs(h) < 1e-7f) { d_tiny_h++; continue; }
                    if (dbg) d_target_active++;

                    for (float beta : { step_beta, 0.5f * step_beta }) {
                        Vec3f cand = orig_p - (beta * h) * sn;
                        Vec3f snap;
                        float sd2;
                        Vec3f sn2;
                        if (ctx.original_bvh.closest_point(cand, snap, sn2, sd2)) cand = snap;
                        if ((cand - anchor[v]).norm() > slide_cap[v]) { d_rej_cap++; continue; }   // anti-migration

                        qmesh.positions[v] = cand;
                        refresh_incident(v);
                        const float d_after = vert_max_dih(v);
                        const float sj_after = min_incident_sj(v);
                        if (dbg && d_sample < 12) {
                            d_sample++;
                            std::cout << "  [t7 sample] v=" << v << " h=" << h
                                      << " cap=" << slide_cap[v]
                                      << " dih " << d_before << "->" << d_after
                                      << " sj " << sj_before << "->" << sj_after << std::endl;
                        }
                        const double a_after = poly_area_of(v);
                        const double projected = global_area - a_before + a_after;
                        const bool area_ok = (projected >= area_lo && projected <= area_hi);
                        if (dbg && d_after >= d_before - 0.5f) d_rej_dih++;
                        // must relax the fold, must not create/ deepen an inverted quad, stay in band
                        if (d_after < d_before - 0.5f && sj_after >= sj_before - 0.01f && area_ok) {
                            global_area = projected;
                            moves++;
                            any = true;
                            break;
                        }
                        if (dbg && !area_ok) d_rej_area++;
                        else if (dbg) d_rej_sj++;
                        qmesh.positions[v] = orig_p;
                        refresh_incident(v);
                    }
                }
            }

            // ---- family 2: pleat unfold over the crease edges of this target face ----
            if (family_mask & 2) {
                for (int i = 0; i < 4; ++i) {
                    uint32_t a = poly[i], b = poly[(i + 1) % 4];
                    const uint64_t ek = ekey(a, b);
                    if (edge_dih(ek) <= out_dih_deg) continue;
                    auto it = out_edges.find(ek);
                    if (it == out_edges.end()) continue;
                    const uint32_t fa = it->second.f0, fb = it->second.f1;
                    if (fa == INVALID_INDEX || fb == INVALID_INDEX) continue;

                    const uint32_t flap = (face_area(fa) <= face_area(fb)) ? fa : fb;
                    uint32_t fv[2]; int fcnt = 0;
                    for (uint32_t v : qmesh.polygons[flap]) {
                        if (v != a && v != b && fcnt < 2) fv[fcnt++] = v;
                    }
                    if (fcnt < 2) continue;
                    // QREMESH_T7_LOOSE=1 lets the unfold also move irregular grid nodes
                    // (valence 3..6); border and feature-edge vertices stay pinned always.
                    if (loose) {
                        auto ok_loose = [&](uint32_t v) {
                            return v < nv && !on_border[v] && !on_sharp[v] &&
                                   v_faces[v].size() >= 3 && v_faces[v].size() <= 6;
                        };
                        if (!ok_loose(fv[0]) || !ok_loose(fv[1])) { d_rej_pin++; continue; }
                    } else if (pin_reason(fv[0]) != 0 || pin_reason(fv[1]) != 0) {
                        d_rej_pin++;
                        continue;
                    }

                    const MoveSet ms = collect_move_set({fv[0], fv[1]});
                    const float crease_before = set_crease_max(ms);
                    const float sj_before = set_min_sj(ms);
                    const double area_before = set_area(ms);
                    if (crease_before <= out_dih_deg) continue;
                    d_unfold_try++;

                    const Vec3f pa0 = qmesh.positions[a], pb0 = qmesh.positions[b];
                    Vec3f axis = pb0 - pa0;
                    if (axis.squaredNorm() < 1e-14f) continue;
                    axis.normalize();
                    const float fold = edge_dih(ek);   // degrees to remove
                    bool unfolded = false;

                    for (float sgn : { 1.0f, -1.0f }) {
                      if (unfolded) break;
                      for (float alpha : { 1.0f, 0.6f }) {
                        Vec3f np[2];
                        bool cap_ok = true;
                        const Eigen::AngleAxisf rot(sgn * alpha * fold * PI_F / 180.0f, axis);
                        for (int t = 0; t < 2; ++t) {
                            Vec3f w = qmesh.positions[fv[t]];
                            Vec3f unf = pa0 + rot * (w - pa0);
                            Vec3f snap, sn3;
                            float sd2;
                            if (ctx.original_bvh.closest_point(unf, snap, sn3, sd2)) unf = snap;
                            if ((unf - anchor[fv[t]]).norm() > cap_scale * slide_cap[fv[t]]) cap_ok = false;
                            np[t] = unf;
                        }
                        if (!cap_ok) { d_rej_cap++; continue; }

                        const Vec3f save0 = qmesh.positions[fv[0]], save1 = qmesh.positions[fv[1]];
                        qmesh.positions[fv[0]] = np[0];
                        qmesh.positions[fv[1]] = np[1];
                        for (uint32_t f : ms.faces) fnorm[f] = newell(f);
                        const float crease_after = set_crease_max(ms);
                        const float sj_after = set_min_sj(ms);
                        const double area_after = set_area(ms);
                        const double projected = global_area - area_before + area_after;
                        const bool area_ok = (projected >= area_lo && projected <= area_hi);
                        if (crease_after < crease_before - 2.0f && sj_after >= sj_before - 0.01f && area_ok) {
                            global_area = projected;
                            moves++;
                            d_unfold_ok++;
                            any = true;
                            unfolded = true;
                            break;
                        }
                        if (dbg) {
                            if (!area_ok) d_unfold_rej_area++;
                            else if (sj_after < sj_before - 0.01f) d_unfold_rej_sj++;
                            else d_unfold_rej_fold++;
                        }
                        if (dbg && d_sample < 12) {
                            d_sample++;
                            std::cout << "  [t7 unfold] flap=" << flap << " edge(" << a << "," << b << ")"
                                      << " fold " << crease_before << "->" << crease_after
                                      << " sj " << sj_before << "->" << sj_after
                                      << " area_ok=" << (area_ok ? 1 : 0) << std::endl;
                        }
                        qmesh.positions[fv[0]] = save0;
                        qmesh.positions[fv[1]] = save1;
                        for (uint32_t f : ms.faces) fnorm[f] = newell(f);
                      }
                    }
                }
            }
        }
        if (!any) break;
    }

    size_t targets_after = 0;
    for (size_t fi = 0; fi < nf; ++fi)
        if (is_target[fi] && face_max_dih(static_cast<uint32_t>(fi)) > out_dih_deg) targets_after++;

    std::cout << "[stage6/t7] gate(out>" << out_dih_deg << ",src<" << src_gate_deg
              << ",R=" << rel << ") src_tris=" << nt
              << " creased=" << creased << " target_faces=" << targets
              << " movable_verts=" << n_movable << " iters=" << iters
              << " moved=" << moves << " targets_left=" << targets_after
              << " winding_conflicts=" << winding_conflicts
              << (winding_conflicts ? " (DIHEDRAL GATE UNRELIABLE: fix output orientation first)"
                                    : " (signed dihedral trustworthy)")
              << " area=" << global_area << std::endl;
    std::cout << "[stage6/t7/pins] crease_edges=" << crease_edges
              << " endpoint_movable=" << crease_ep_movable
              << " pinned_border=" << pin_border << " pinned_sharp=" << pin_sharp
              << " pinned_valence=" << pin_valence << std::endl;
    if (dbg) {
    std::cout << "[stage6/t7/dbg] corners=" << d_corner
                  << " active=" << d_target_active << " no_fold=" << d_no_fold
                  << " tiny_h=" << d_tiny_h << " no_bvh=" << d_no_bvh
                  << " rej_cap=" << d_rej_cap << " rej_dih=" << d_rej_dih
                  << " rej_sj=" << d_rej_sj << " rej_area=" << d_rej_area
                  << " refl_try=" << d_unfold_try << " refl_ok=" << d_unfold_ok
                  << " rej_pin=" << d_rej_pin << " unfold_rej_fold=" << d_unfold_rej_fold
                  << " unfold_rej_sj=" << d_unfold_rej_sj << " unfold_rej_area=" << d_unfold_rej_area << std::endl;
    }
}

// ---------------------------------------------------------------------------
// T8: collapse degenerate short edges (sliver/spike quads).
//
// Why a TOPOLOGICAL op and not more smoothing: measured, the folded faces are degenerate by
// SHAPE, not by placement. On a clean watertight icosphere (perfect source, 98.7% valence-4)
// the extraction still folds 1.5% of its faces, and those faces are spikes: area 0.00001 against
// a 0.00025 median (25x smaller), three of four corners within 0.004 of each other, SJ ~0.002.
// On the AI model the BUG-01 zones carry 23% slivers (minEdge < 0.35 x median) against 12% for
// the whole mesh and 15% "tiny area" against 6% - a 2x enrichment. No vertex move can repair a
// quad whose two edges are ~zero long (that is exactly why the S42 unfold got blocked 527 times
// by the SJ guard); the short edge has to go.
//
// Safety: only INTERIOR edges (exactly 2 faces) whose endpoints pass the link condition
// (|N(u) n N(v)| == 2 - the standard manifold-preserving test), never a border or feature/sharp
// vertex, chosen as a disjoint set ordered by length, merged position snapped onto the source
// (invariant #7), and the WHOLE batch is reverted if the rebuilt mesh gains a boundary edge or
// any edge ends up with >2 faces.
// ---------------------------------------------------------------------------
void Stage6Optimize::collapse_degenerate_edges(
    RemeshContext& ctx,
    QuadMeshResult& qmesh,
    float ratio
) {
    const size_t nv = qmesh.positions.size();
    const size_t nf = qmesh.polygons.size();
    if (nv < 4 || nf < 2 || ratio <= 0.0f) return;

    auto ekey = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };

    struct EInfo { uint32_t f0 = INVALID_INDEX; uint32_t f1 = INVALID_INDEX; };
    std::unordered_map<uint64_t, EInfo> edges;
    std::unordered_set<uint64_t> over2;
    edges.reserve(nf * 2);
    std::vector<std::vector<uint32_t>> nbr(nv);
    std::vector<uint8_t> is_border(nv, 0);

    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& p = qmesh.polygons[fi];
        const size_t pn = p.size();
        if (pn < 3) continue;
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = p[i], b = p[(i + 1) % pn];
            if (a >= nv || b >= nv || a == b) continue;
            uint64_t k = ekey(a, b);
            auto it = edges.find(k);
            if (it == edges.end()) {
                EInfo e; e.f0 = static_cast<uint32_t>(fi);
                edges.emplace(k, e);
            } else if (it->second.f1 == INVALID_INDEX && !over2.count(k)) {
                if (it->second.f0 != static_cast<uint32_t>(fi)) it->second.f1 = static_cast<uint32_t>(fi);
            } else {
                over2.insert(k);
            }
            nbr[a].push_back(b);
            nbr[b].push_back(a);
        }
    }
    for (auto& [k, e] : edges) {
        if (e.f1 == INVALID_INDEX && !over2.count(k)) {
            is_border[static_cast<uint32_t>(k >> 32)] = 1;
            is_border[static_cast<uint32_t>(k & 0xFFFFFFFFu)] = 1;
        }
    }
    std::vector<uint8_t> is_sharp(nv, 0);
    for (const auto& se : qmesh.sharp_edges) {
        if (se.first < nv && se.second < nv) { is_sharp[se.first] = 1; is_sharp[se.second] = 1; }
    }
    for (size_t v = 0; v < nv; ++v) {
        std::sort(nbr[v].begin(), nbr[v].end());
        nbr[v].erase(std::unique(nbr[v].begin(), nbr[v].end()), nbr[v].end());
    }

    // median edge length (the scale the guard is expressed in)
    std::vector<float> lens;
    std::vector<uint64_t> keys;
    lens.reserve(edges.size());
    keys.reserve(edges.size());
    for (const auto& [k, e] : edges) {
        uint32_t a = static_cast<uint32_t>(k >> 32), b = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        lens.push_back((qmesh.positions[a] - qmesh.positions[b]).norm());
        keys.push_back(k);
    }
    std::vector<float> lens_sorted = lens;
    std::nth_element(lens_sorted.begin(), lens_sorted.begin() + lens_sorted.size() / 2, lens_sorted.end());
    const float med = lens_sorted[lens_sorted.size() / 2];
    if (!(med > 1e-9f)) return;
    const float thresh = ratio * med;

    size_t boundary_before = 0;
    for (const auto& [k, e] : edges)
        if (e.f1 == INVALID_INDEX && !over2.count(k)) boundary_before++;

    // candidate interior short edges, deterministic order: shortest first, then by key
    std::vector<std::pair<float, uint64_t>> cand;
    size_t c_short = 0, c_border = 0, c_sharp = 0, c_link = 0;   // rejection breakdown
    for (size_t i = 0; i < keys.size(); ++i) {
        const uint64_t k = keys[i];
        if (lens[i] >= thresh || over2.count(k)) continue;
        const EInfo& e = edges[k];
        if (e.f0 == INVALID_INDEX || e.f1 == INVALID_INDEX) continue;
        c_short++;
        uint32_t u = static_cast<uint32_t>(k >> 32), v = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        if (u >= nv || v >= nv) continue;
        if (is_border[u] || is_border[v]) { c_border++; continue; }
        if (is_sharp[u] || is_sharp[v]) { c_sharp++; continue; }
        // Manifoldness of the collapse: the one-ring of the edge must stay a disk. For a QUAD
        // mesh the two faces adjacent to (u,v) contribute DIFFERENT opposite vertices, so a
        // regular interior edge has |N(u) n N(v)| = 0 (or 1-2 when the ring is pinched); only
        // MORE than 2 common neighbours signals a genuine non-manifold / shortcut collapse.
        // (The first version required exactly 2 - the triangle-mesh rule - and it rejected every
        // legal quad collapse: accepted=0 with 662 rej_link.) The batch validation below is the
        // real safety net: any non-manifold edge or grown boundary reverts the whole batch.
        std::vector<uint32_t> inter;
        std::set_intersection(nbr[u].begin(), nbr[u].end(), nbr[v].begin(), nbr[v].end(),
                              std::back_inserter(inter));
        if (inter.size() > 2) { c_link++; continue; }
        cand.emplace_back(lens[i], k);
    }
    std::sort(cand.begin(), cand.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first < b.first;
        return a.second < b.second;
    });

    // ---- greedy, one collapse at a time, geometry validated BEFORE it is applied -------
    // A batch version was tried first and rejected by measurement: at ratio 0.5 it removed 39 of
    // the 127 BUG-01 targets and raised median SJ, but it also minted fully inverted quads
    // (Min SJ -0.660 -> -0.9999) and one duplicate face reverted everything. Validating each
    // candidate against its own affected ring keeps the pass monotone: it can only ever accept a
    // collapse that does not worsen the worst quad it touches.
    std::vector<std::vector<uint32_t>> polys = qmesh.polygons;
    std::vector<uint8_t> alive(nf, 0);
    std::vector<std::vector<uint32_t>> vfaces(nv);
    std::unordered_map<uint64_t, uint32_t> ecount;
    ecount.reserve(edges.size() * 2);
    size_t boundary_live = 0;
    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& p = polys[fi];
        const size_t pn = p.size();
        if (pn < 3) continue;
        alive[fi] = 1;
        for (uint32_t v : p) if (v < nv) vfaces[v].push_back(static_cast<uint32_t>(fi));
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = p[i], b = p[(i + 1) % pn];
            if (a == b || a >= nv || b >= nv) continue;
            uint64_t k2 = ekey(a, b);
            uint32_t c = ++ecount[k2];
            if (c == 1) boundary_live++;
            else if (c == 2) boundary_live--;   // was a boundary edge, now interior
        }
    }

    auto face_sj = [&](const std::vector<uint32_t>& p, const Vec3f* pos) -> float {
        if (p.size() == 4) {
            return compute_scaled_jacobian(pos[p[0]], pos[p[1]], pos[p[2]], pos[p[3]]);
        }
        return 1.0f;    // triangles/ngons carry no Scaled Jacobian in this pipeline
    };
    auto face_area = [&](const std::vector<uint32_t>& p, const Vec3f* pos) -> double {
        if (p.size() < 3) return 0.0;
        double a = 0.0;
        for (size_t i = 1; i + 1 < p.size(); ++i) {
            Vec3f e1 = pos[p[i]] - pos[p[0]];
            Vec3f e2 = pos[p[i + 1]] - pos[p[0]];
            a += 0.5 * static_cast<double>(e1.cross(e2).norm());
        }
        return a;
    };
    std::vector<Vec3f> positions = qmesh.positions;

    size_t applied = 0, rej_topo = 0, rej_geom = 0, rej_dup = 0, killed_faces = 0;
    size_t r_move = 0, r_sj = 0, r_area = 0, r_neg = 0;
    std::vector<uint8_t> dead(nv, 0);

    for (const auto& [len0, k] : cand) {
        const uint32_t u = static_cast<uint32_t>(k >> 32);
        const uint32_t v = static_cast<uint32_t>(k & 0xFFFFFFFFu);
        if (u >= nv || v >= nv || dead[u] || dead[v]) continue;

        std::vector<uint32_t> aff = vfaces[u];
        aff.insert(aff.end(), vfaces[v].begin(), vfaces[v].end());
        std::sort(aff.begin(), aff.end());
        aff.erase(std::unique(aff.begin(), aff.end()), aff.end());
        if (aff.empty()) continue;

        // merged position: midpoint, snapped onto the source, allowed to travel <= 0.5 median edge
        Vec3f mid = 0.5f * (positions[u] + positions[v]);
        Vec3f snap, nrm;
        float d2;
        if (ctx.original_bvh.closest_point(mid, snap, nrm, d2) &&
            (snap - mid).norm() < 0.5f * med) mid = snap;
        if ((mid - positions[u]).norm() > 0.5f * med ||
            (mid - positions[v]).norm() > 0.5f * med) { r_move++; rej_geom++; continue; }

        // simulate: rewrite the affected ring with v -> u
        struct Chg { uint32_t fi; std::vector<uint32_t> np; };
        std::vector<Chg> chgs;
        chgs.reserve(aff.size());
        std::unordered_map<uint64_t, int> delta;
        bool dup_bad = false;
        std::vector<Vec3f> tmp = positions;   // only tmp[u] differs below
        tmp[u] = mid;
        float sj_before = 1.0f, sj_after = 1.0f;
        double area_before = 0.0, area_after = 0.0;
        for (uint32_t fi : aff) {
            if (!alive[fi]) continue;
            const auto& old = polys[fi];
            sj_before = std::min(sj_before, face_sj(old, positions.data()));
            area_before += face_area(old, positions.data());

            std::vector<uint32_t> np;
            np.reserve(old.size());
            for (uint32_t x : old) {
                uint32_t y = (x == v) ? u : x;
                if (np.empty() || np.back() != y) np.push_back(y);
            }
            while (np.size() > 1 && np.front() == np.back()) np.pop_back();
            if (np.size() >= 3) {
                std::vector<uint32_t> srt = np;
                std::sort(srt.begin(), srt.end());
                if (std::adjacent_find(srt.begin(), srt.end()) != srt.end()) dup_bad = true;
            }
            const size_t pn = old.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = old[i], b = old[(i + 1) % pn];
                if (a == b) continue;
                delta[ekey(a, b)] -= 1;
            }
            const size_t qn = np.size();
            for (size_t i = 0; i < qn; ++i) {
                uint32_t a = np[i], b = np[(i + 1) % qn];
                if (a == b) continue;
                delta[ekey(a, b)] += 1;
            }
            // Metrics MUST be computed before np is moved out (the first version read np after
            // std::move: area_after/sj_after were evaluated on an emptied vector, which rejected
            // 594/662 candidates in the area band and let the survivors mint inverted quads).
            if (qn >= 3) {
                sj_after = std::min(sj_after, face_sj(np, tmp.data()));
                area_after += face_area(np, tmp.data());
            }
            chgs.push_back(Chg{fi, std::move(np)});
        }
        if (dup_bad) { rej_dup++; continue; }

        // topology: no edge may end up with >2 faces, the boundary may not grow
        bool topo_ok = true;
        long boundary_after = static_cast<long>(boundary_live);
        for (const auto& [e, d] : delta) {
            if (d == 0) continue;
            long before = static_cast<long>(ecount.count(e) ? ecount[e] : 0u);
            long after = before + d;
            if (after > 2 || after < 0) { topo_ok = false; break; }
            if (before == 1 && after != 1) boundary_after--;   // boundary edge became interior
            if (before == 0 && after == 1) boundary_after++;   // new boundary edge
        }
        if (!topo_ok) { rej_topo++; continue; }
        if (boundary_after > static_cast<long>(boundary_live)) { rej_topo++; continue; }

        // geometry: the collapse must not worsen the worst quad it touches
        {
            size_t neg_before = 0, neg_after = 0;
            for (uint32_t fi : aff) {
                if (!alive[fi]) continue;
                if (face_sj(polys[fi], positions.data()) < 0.0f) neg_before++;
            }
            for (const auto& c : chgs) if (c.np.size() >= 3 && face_sj(c.np, tmp.data()) < 0.0f) neg_after++;
            if (neg_after > neg_before) { r_neg++; rej_geom++; continue; }
        }
        if (sj_after < sj_before - 0.02f) { r_sj++; rej_geom++; continue; }
        // Area is NOT the guard here: collapsing a folded flap legitimately removes the area it
        // double-covered (the first version kept a [0.55,1.45] band and it rejected 594/662
        // candidates precisely because the repair works). The real safety net is: no new negative
        // quad, no grown boundary, no non-manifold edge, plus the RMS/Hausdorff/boundary metrics
        // measured outside. Only INFLATION (wrinkles, invariant #7) stays bounded.
        // Area: only INFLATION is bounded (wrinkles, invariant #7). Shrinking is the point of the
        // operation - a folded flap contributes area the surface does not have. The shrink side is
        // watched from outside by RMS/Hausdorff/boundary.
        if (area_before > 1e-12 && area_after > 1.45 * area_before) {
            r_area++;
            rej_geom++;
            continue;
        }

        // commit
        for (const auto& c : chgs) {
            const auto& old = polys[c.fi];
            const size_t pn = old.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = old[i], b = old[(i + 1) % pn];
                if (a == b) continue;
                uint64_t e = ekey(a, b);
                auto it = ecount.find(e);
                if (it != ecount.end()) {
                    if (it->second > 1) it->second -= 1;
                    else { if (it->second == 1) boundary_live--; ecount.erase(it); }
                }
            }
            const size_t qn = c.np.size();
            for (size_t i = 0; i < qn; ++i) {
                uint32_t a = c.np[i], b = c.np[(i + 1) % qn];
                if (a == b) continue;
                uint64_t e = ekey(a, b);
                uint32_t before = ecount.count(e) ? ecount[e] : 0u;
                uint32_t& cref = ecount[e];
                cref = before + 1;
                if (before == 0) boundary_live++;
                else if (before == 1) boundary_live--;
            }
            if (c.np.size() < 3) { alive[c.fi] = 0; polys[c.fi].clear(); killed_faces++; }
            else polys[c.fi] = std::move(c.np);
        }
        if (std::getenv("QREMESH_COLLAPSE_TRACE") != nullptr) {
            // find the ring quad that ended up terrible, if any (guard-hole detector)
            for (const auto& c : chgs) {
                if (c.np.size() != 4) continue;
                float f = compute_scaled_jacobian(tmp[c.np[0]], tmp[c.np[1]], tmp[c.np[2]], tmp[c.np[3]]);
                if (f < -0.7f) {
                    std::cout << "  [collapse trace] face " << c.fi << " after collapse (" << u << "," << v
                              << ") sj=" << f << " corners(" << c.np[0] << "," << c.np[1] << ","
                              << c.np[2] << "," << c.np[3] << ") sj_before_ring=" << sj_before
                              << " sj_after_ring=" << sj_after << std::endl;
                }
            }
        }
        positions[u] = mid;
        dead[v] = 1;
        for (uint32_t fi : vfaces[v]) {
            if (std::find(vfaces[u].begin(), vfaces[u].end(), fi) == vfaces[u].end())
                vfaces[u].push_back(fi);
        }
        vfaces[v].clear();
        applied++;
    }

    std::cout << "[stage6/collapse/scan] short=" << c_short << " rej_border=" << c_border
              << " rej_sharp=" << c_sharp << " rej_link=" << c_link
              << " accepted=" << cand.size() << " (sharp_verts="
              << (size_t)std::count(is_sharp.begin(), is_sharp.end(), 1)
              << " border_verts=" << (size_t)std::count(is_border.begin(), is_border.end(), 1)
              << " sharp_edges=" << qmesh.sharp_edges.size() << ")" << std::endl;
    // Global sanity of the guards themselves: if the mesh-wide worst quad got worse while every
    // per-collapse check passed, the checks have a hole - so measure it instead of trusting it.
    float sj_min_before = 1.0f, sj_min_after = 1.0f;
    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& p0 = qmesh.polygons[fi];
        if (p0.size() == 4) {
            sj_min_before = std::min(sj_min_before, compute_scaled_jacobian(
                qmesh.positions[p0[0]], qmesh.positions[p0[1]],
                qmesh.positions[p0[2]], qmesh.positions[p0[3]]));
        }
    }
    for (const auto& p0 : polys) {
        if (p0.size() == 4) {
            sj_min_after = std::min(sj_min_after, compute_scaled_jacobian(
                positions[p0[0]], positions[p0[1]], positions[p0[2]], positions[p0[3]]));
        }
    }

    std::cout << "[stage6/collapse/qc] applied=" << applied << " rej_topo=" << rej_topo
              << " move=" << r_move << " sj_worst=" << r_sj << " area=" << r_area
              << " new_negative=" << r_neg << " rej_dup=" << rej_dup
              << " mesh_min_sj " << sj_min_before << "->" << sj_min_after << std::endl;

    if (applied == 0) {
        std::cout << "[stage6/collapse] thresh=" << thresh << " (" << ratio << " x median " << med
                  << ") candidates=" << cand.size() << " applied=0"
                  << " rej_topo=" << rej_topo << " rej_geom=" << rej_geom
                  << " rej_dup=" << rej_dup << std::endl;
        return;
    }

    // compact: rebuild the polygon list and the vertex table without dead vertices/faces
    std::vector<uint32_t> compact(nv, INVALID_INDEX);
    std::vector<Vec3f> new_pos;
    new_pos.reserve(nv);
    std::vector<std::vector<uint32_t>> new_polys;
    std::vector<uint16_t> new_mats;
    new_polys.reserve(nf);
    new_mats.reserve(nf);
    for (size_t fi = 0; fi < nf; ++fi) {
        if (!alive[fi]) continue;
        std::vector<uint32_t> p;
        p.reserve(polys[fi].size());
        bool ok = true;
        for (uint32_t v : polys[fi]) {
            if (v >= nv || dead[v]) { ok = false; break; }
            if (compact[v] == INVALID_INDEX) {
                compact[v] = static_cast<uint32_t>(new_pos.size());
                new_pos.push_back(positions[v]);
            }
            uint32_t c = compact[v];
            if (p.empty() || p.back() != c) p.push_back(c);
        }
        while (p.size() > 1 && p.front() == p.back()) p.pop_back();
        if (!ok || p.size() < 3) continue;
        new_polys.push_back(std::move(p));
        new_mats.push_back(fi < qmesh.material_ids.size() ? qmesh.material_ids[fi] : 0);
    }

    std::vector<std::pair<uint32_t, uint32_t>> new_sharp;
    for (const auto& se : qmesh.sharp_edges) {
        if (se.first < nv && se.second < nv && !dead[se.first] && !dead[se.second]) {
            uint32_t a = compact[se.first], b = compact[se.second];
            if (a != INVALID_INDEX && b != INVALID_INDEX && a != b) new_sharp.emplace_back(a, b);
        }
    }

    qmesh.positions = std::move(new_pos);
    qmesh.polygons = std::move(new_polys);
    qmesh.material_ids = std::move(new_mats);
    qmesh.sharp_edges = std::move(new_sharp);

    std::cout << "[stage6/collapse] thresh=" << thresh << " (" << ratio << " x median " << med
              << ") candidates=" << cand.size() << " applied=" << applied
              << " rej_topo=" << rej_topo << " rej_geom=" << rej_geom
              << "(move=" << r_move << " sj=" << r_sj << " area=" << r_area
              << " neg=" << r_neg << ") rej_dup=" << rej_dup
              << " faces " << nf << "->" << qmesh.polygons.size() << " killed=" << killed_faces
              << " verts " << nv << "->" << qmesh.positions.size() << std::endl;
}

// ---------------------------------------------------------------------------
// T9: re-wind the faces whose traversal disagrees with the orientation majority.
//
// Measured on the shipped baseline (test_out/quality_10k.obj): 200 interior edges are traversed in
// the SAME direction by both faces, touching 234 faces, 138 of which have a normal opposing the
// majority of their ring. In a render those are dark / inside-out facets - and they also poison
// every signed metric: the T7 detector reads a ~180 deg "crease" for a flat pair with flipped
// winding, which is why the winding audit had to be added to that pass (S42b).
//
// The fix moves nothing. Colour the dual graph so that a consistent edge demands equal colours and
// a conflicting edge demands opposite ones (2-colouring); per connected component keep the minority
// assignment if it is smaller than the majority, then reverse the polygon order of the faces marked
// -1. Odd parity cycles cannot be fixed by re-winding (they are genuine non-orientable junctions),
// so the residual is reported instead of hidden.
// ---------------------------------------------------------------------------
void Stage6Optimize::rewind_inconsistent_faces(RemeshContext& /*ctx*/, QuadMeshResult& qmesh) {
    const size_t nf = qmesh.polygons.size();
    if (nf < 2) return;

    auto ekey = [](uint32_t a, uint32_t b) -> uint64_t {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | static_cast<uint32_t>(std::max(a, b));
    };
    struct Rec { uint32_t f0 = INVALID_INDEX, f1 = INVALID_INDEX; int8_t d0 = 0, d1 = 0; };
    std::unordered_map<uint64_t, Rec> edges;
    std::unordered_set<uint64_t> over2;
    edges.reserve(nf * 2);

    for (size_t fi = 0; fi < nf; ++fi) {
        const auto& q = qmesh.polygons[fi];
        const size_t pn = q.size();
        if (pn < 3) continue;
        for (size_t i = 0; i < pn; ++i) {
            uint32_t a = q[i], b = q[(i + 1) % pn];
            if (a == b || a >= qmesh.positions.size() || b >= qmesh.positions.size()) continue;
            const uint64_t k = ekey(a, b);
            const int8_t dir = (a < b) ? 1 : -1;
            auto it = edges.find(k);
            if (it == edges.end()) {
                Rec r; r.f0 = static_cast<uint32_t>(fi); r.d0 = dir;
                edges.emplace(k, r);
            } else if (it->second.f1 == INVALID_INDEX && !over2.count(k)) {
                if (it->second.f0 != static_cast<uint32_t>(fi)) {
                    it->second.f1 = static_cast<uint32_t>(fi);
                    it->second.d1 = dir;
                }
            } else {
                over2.insert(k);
            }
        }
    }

    auto count_conflicts = [&]() -> size_t {
        size_t c = 0;
        for (const auto& [k, r] : edges) {
            if (r.f0 == INVALID_INDEX || r.f1 == INVALID_INDEX || over2.count(k)) continue;
            if (r.d0 == r.d1) c++;
        }
        return c;
    };
    const size_t conflicts_before = count_conflicts();
    if (conflicts_before == 0) {
        std::cout << "[stage6/rewind] already consistent (conflicts=0)" << std::endl;
        return;
    }

    // dual graph with parity: opposite colours demanded across an already-conflicting edge
    std::vector<std::vector<std::pair<uint32_t, int8_t>>> adj(nf);
    for (const auto& [k, r] : edges) {
        if (r.f0 == INVALID_INDEX || r.f1 == INVALID_INDEX || over2.count(k)) continue;
        const int8_t parity = (r.d0 == r.d1) ? -1 : 1;   // colour[other] = colour[this] * parity
        adj[r.f0].push_back({r.f1, parity});
        adj[r.f1].push_back({r.f0, parity});
    }
    for (auto& l : adj)
        std::sort(l.begin(), l.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    std::vector<int8_t> color(nf, 0);
    std::vector<std::vector<uint32_t>> components;
    size_t odd_cycles = 0;
    std::vector<uint32_t> stack;
    for (size_t s = 0; s < nf; ++s) {
        if (color[s] || qmesh.polygons[s].size() < 3) continue;
        color[s] = 1;
        stack.clear();
        stack.push_back(static_cast<uint32_t>(s));
        components.emplace_back();
        auto& comp = components.back();
        while (!stack.empty()) {
            const uint32_t f = stack.back();
            stack.pop_back();
            comp.push_back(f);
            for (const auto& [g, parity] : adj[f]) {
                const int8_t want = static_cast<int8_t>(color[f] * parity);
                if (color[g] == 0) {
                    color[g] = want;
                    stack.push_back(g);
                } else if (color[g] != want) {
                    odd_cycles++;          // genuine non-orientable junction: cannot be re-wound
                }
            }
        }
    }

    size_t flipped = 0;
    for (auto& comp : components) {
        size_t neg = 0;
        for (uint32_t f : comp) if (color[f] < 0) neg++;
        const bool invert = neg * 2 > comp.size();     // flip the minority, keep the majority
        for (uint32_t f : comp) {
            const bool should_flip = invert ? (color[f] > 0) : (color[f] < 0);
            if (!should_flip) continue;
            std::reverse(qmesh.polygons[f].begin() + 1, qmesh.polygons[f].end());
            flipped++;
        }
    }

    // Recompute from the NEW traversals - the map above still holds the pre-flip directions, so
    // reusing it would print "200->200" while the mesh is actually consistent (it did).
    size_t conflicts_after = 0;
    {
        struct Chk { uint32_t f0 = INVALID_INDEX; int8_t d0 = 0; };
        std::unordered_map<uint64_t, Chk> first;
        std::unordered_set<uint64_t> skip;
        first.reserve(nf * 2);
        for (size_t fi = 0; fi < nf; ++fi) {
            const auto& q = qmesh.polygons[fi];
            const size_t pn = q.size();
            if (pn < 3) continue;
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = q[i], b = q[(i + 1) % pn];
                if (a == b || a >= qmesh.positions.size() || b >= qmesh.positions.size()) continue;
                const uint64_t k = ekey(a, b);
                const int8_t dir = (a < b) ? 1 : -1;
                auto it = first.find(k);
                if (it == first.end()) {
                    Chk c; c.f0 = static_cast<uint32_t>(fi); c.d0 = dir;
                    first.emplace(k, c);
                } else if (!skip.count(k)) {
                    if (it->second.f0 != static_cast<uint32_t>(fi)) {
                        if (it->second.d0 == dir) conflicts_after++;
                        skip.insert(k);          // 3rd+ face on this edge: not counted either way
                    }
                }
            }
        }
    }
    std::cout << "[stage6/rewind] conflicts " << conflicts_before << "->" << conflicts_after
              << " faces_flipped=" << flipped << "/" << nf
              << " components=" << components.size()
              << " odd_cycles(unfixable)=" << odd_cycles << std::endl;
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
    //   QREMESH_T7=1             localized smooth-crease repair (BUG-01, default OFF)
    int relax_iters = ctx.params->vertex_relax_iters;
    bool skip_holefill = false;
    bool skip_untangle = false;
    bool t7_localized = false;
    if (const char* env_r = std::getenv("QREMESH_RELAX_ITERS")) {
        relax_iters = std::max(0, std::atoi(env_r));
    }
    if (const char* env_h = std::getenv("QREMESH_NO_HOLEFILL")) {
        skip_holefill = (std::atoi(env_h) != 0);
    }
    if (const char* env_u = std::getenv("QREMESH_NO_UNTANGLE")) {
        skip_untangle = (std::atoi(env_u) != 0);
    }
    if (const char* env_t7 = std::getenv("QREMESH_T7")) {
        t7_localized = (std::atoi(env_t7) != 0);
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

    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 64); log_area("after_fill"); }
    cleanup_topology(qmesh);
    log_area("after_cleanup");
    tangential_taubin_smoothing(ctx, qmesh, relax_iters);
    log_area("after_relax");
    cleanup_topology(qmesh);
    log_area("after_cleanup2");
    if (!skip_untangle) { untangle_low_jacobians(ctx, qmesh); log_area("after_untangle"); }
    // Second fill pass: cleanup/untangle can open small holes after the first pass
    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 64); log_area("after_fill2"); }

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
    if (!skip_holefill) { fill_small_boundary_holes(ctx, qmesh, 64); log_area("after_fill3"); }

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

    // T6 (S38): drop disconnected island SHELLS from the final quad mesh. In quality mode
    // (keep-best-2 removes sliver bridges) a self-touching AI surface separates into the
    // main shell + a few islands (tiny dust at low budget; a larger lobe at high). Keep
    // components whose face count is >= max(abs_floor, ratio * largest) and drop the rest; a
    // genuinely multi-part model keeps its real parts (they are large), dust disappears.
    // Runs BEFORE the quality report so reported boundary/components reflect the result.
    // Env QREMESH_DROP_ISLANDS (abs floor 50 quads default, ratio 0.01; QREMESH_ISLAND_*).
    if (ctx.repair.drop_islands) {
        size_t nq = qmesh.polygons.size();
        std::unordered_map<uint64_t, uint32_t> edge2face;  // undirected edge -> a face
        std::vector<uint32_t> par(nq);
        for (size_t i = 0; i < nq; ++i) par[i] = (uint32_t)i;
        std::function<uint32_t(uint32_t)> find = [&](uint32_t x) {
            while (par[x] != x) { par[x] = par[par[x]]; x = par[x]; } return x;
        };
        auto uni = [&](uint32_t a, uint32_t b) { a = find(a); b = find(b); if (a != b) par[a] = b; };
        for (uint32_t f = 0; f < nq; ++f) {
            const auto& p = qmesh.polygons[f];
            size_t pn = p.size();
            for (size_t i = 0; i < pn; ++i) {
                uint32_t a = p[i], b = p[(i + 1) % pn];
                uint64_t key = (uint64_t(std::min(a,b)) << 32) | std::max(a,b);
                auto it = edge2face.find(key);
                if (it == edge2face.end()) edge2face[key] = f;
                else uni(it->second, f);
            }
        }
        std::unordered_map<uint32_t, uint32_t> csz; uint32_t largest = 0;
        for (uint32_t f = 0; f < nq; ++f) { uint32_t r = find(f); uint32_t s = ++csz[r]; if (s > largest) largest = s; }
        float iratio = 0.01f;
        if (const char* e = std::getenv("QREMESH_ISLAND_RATIO")) { float r = std::atof(e); if (r > 0) iratio = r; }
        uint32_t ifloor = 50;
        if (const char* e = std::getenv("QREMESH_ISLAND_FLOOR")) { long r = std::atol(e); if (r >= 0) ifloor = (uint32_t)r; }
        uint32_t thresh = std::max(ifloor, (uint32_t)(iratio * largest));
        std::vector<std::vector<uint32_t>> keep_polys;
        std::vector<uint16_t> keep_mats;
        keep_polys.reserve(nq);
        uint32_t dropped = 0;
        for (uint32_t f = 0; f < nq; ++f) {
            if (csz[find(f)] >= thresh) { keep_polys.push_back(std::move(qmesh.polygons[f])); if (f < qmesh.material_ids.size()) keep_mats.push_back(qmesh.material_ids[f]); }
            else dropped++;
        }
        qmesh.polygons = std::move(keep_polys);
        qmesh.material_ids = std::move(keep_mats);
        std::cout << "[stage6/islands] kept=" << qmesh.polygons.size() << " dropped=" << dropped
                  << " thresh=" << thresh << " largest=" << largest << std::endl;
    }

    // T9/S52: make the traversal consistent before anything measures a signed dihedral. Reverses
    // polygon order only - no vertex moves, so SJ / Hausdorff / RMS / polygon count cannot change
    // (verified: median delta exactly 0.0000 on every geometric metric over 5 budgets, and
    // bit-identical files on the whole regression corpus). DEFAULT ON; --no-rewind / 
    // QREMESH_REWIND=0 restores the legacy traversal byte-for-byte.
    if (ctx.repair.rewind) {
        rewind_inconsistent_faces(ctx, qmesh);
    }

    // T8 (BUG-01): topological repair of the degenerate sliver quads that fold over their
    // neighbour. Runs before the T7 mask so T7 sees the repaired topology. Default OFF
    // (QREMESH_COLLAPSE=<ratio>, e.g. 0.35) - the baseline stays bit-identical without it.
    float collapse_ratio = 0.0f;
    if (const char* e = std::getenv("QREMESH_COLLAPSE")) {
        float v = std::atof(e);
        if (v > 0.0f) collapse_ratio = v;
    }
    if (collapse_ratio > 0.0f) {
        collapse_degenerate_edges(ctx, qmesh, collapse_ratio);
        log_area("after_collapse");
    }

    // T7 (BUG-01): localized smooth-crease repair. Runs LAST on geometry (after the final
    // fill + island policy, before the quality report) and only on the analytic target mask
    // (creased output x locally smooth source). Opt-in: default OFF keeps the baseline
    // bit-identical. Interior grid nodes only - borders (Dirichlet) and feature edges pinned.
    if (t7_localized) {
        localized_smooth_crease_repair(ctx, qmesh);
        log_area("after_t7");
        if (ctx.active_symmetry_axis >= 0 && ctx.active_symmetry_axis <= 2) {
            float tol = ctx.params->symmetry_tolerance * 0.01f * ctx.bbox_diag;
            mirror_symmetry(ctx, qmesh, ctx.active_symmetry_axis, tol);
        }
    }

    compute_final_quality_report(ctx, qmesh);

    ctx.dump_quads_obj("stage6_optimized", qmesh);

    ctx.notify_progress(1.0f, "Completed");
    return QRM_OK;
}

} // namespace qremesh
