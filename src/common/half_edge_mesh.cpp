#include "common/half_edge_mesh.h"
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <iostream>

namespace qremesh {

struct DirectedEdgeHash {
    size_t operator()(const std::pair<uint32_t, uint32_t>& p) const {
        return (static_cast<size_t>(p.first) * 73856093) ^ (static_cast<size_t>(p.second) * 19349663);
    }
};

bool HalfEdgeMesh::build_from_triangles(
    const float* positions, uint32_t nv,
    const uint32_t* tri_indices, uint32_t nt,
    const uint16_t* material_ids
) {
    vertices.clear();
    halfedges.clear();
    edges.clear();
    faces.clear();

    if (nv == 0 || nt == 0 || !positions || !tri_indices) {
        return false;
    }

    vertices.resize(nv);
    for (uint32_t i = 0; i < nv; ++i) {
        vertices[i].pos = Vec3f(positions[3 * i + 0], positions[3 * i + 1], positions[3 * i + 2]);
        vertices[i].halfedge = INVALID_INDEX;
        vertices[i].is_deleted = false;
    }

    faces.resize(nt);
    std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, DirectedEdgeHash> edge_map;

    for (uint32_t f_idx = 0; f_idx < nt; ++f_idx) {
        uint32_t v0 = tri_indices[3 * f_idx + 0];
        uint32_t v1 = tri_indices[3 * f_idx + 1];
        uint32_t v2 = tri_indices[3 * f_idx + 2];

        if (v0 >= nv || v1 >= nv || v2 >= nv || v0 == v1 || v1 == v2 || v2 == v0) {
            continue; // Skip invalid triangle
        }

        faces[f_idx].material_id = material_ids ? material_ids[f_idx] : 0;
        faces[f_idx].is_deleted = false;

        uint32_t h0 = static_cast<uint32_t>(halfedges.size());
        uint32_t h1 = h0 + 1;
        uint32_t h2 = h0 + 2;
        halfedges.resize(h0 + 3);

        faces[f_idx].halfedge = h0;

        uint32_t tri_v[3] = { v0, v1, v2 };
        uint32_t tri_h[3] = { h0, h1, h2 };

        for (int i = 0; i < 3; ++i) {
            uint32_t cur_h = tri_h[i];
            uint32_t next_h = tri_h[(i + 1) % 3];
            uint32_t prev_h = tri_h[(i + 2) % 3];
            uint32_t cur_v = tri_v[i];
            uint32_t next_v = tri_v[(i + 1) % 3];

            halfedges[cur_h].to_vertex = next_v;
            halfedges[cur_h].face = f_idx;
            halfedges[cur_h].next = next_h;
            halfedges[cur_h].prev = prev_h;
            halfedges[cur_h].is_deleted = false;

            if (vertices[cur_v].halfedge == INVALID_INDEX) {
                vertices[cur_v].halfedge = cur_h;
            }

            edge_map[{cur_v, next_v}] = cur_h;
        }
    }

    // Now resolve twins and create edges
    for (size_t h = 0; h < halfedges.size(); ++h) {
        if (halfedges[h].is_deleted) continue;
        uint32_t u = from_vertex(static_cast<uint32_t>(h));
        uint32_t v = halfedges[h].to_vertex;

        if (halfedges[h].twin != INVALID_INDEX) continue; // Already linked

        auto it = edge_map.find({v, u});
        uint32_t twin_h = (it != edge_map.end()) ? it->second : INVALID_INDEX;

        uint32_t edge_idx = static_cast<uint32_t>(edges.size());
        edges.emplace_back();
        edges[edge_idx].halfedge = static_cast<uint32_t>(h);

        halfedges[h].edge = edge_idx;
        halfedges[h].twin = twin_h;

        if (twin_h != INVALID_INDEX) {
            halfedges[twin_h].twin = static_cast<uint32_t>(h);
            halfedges[twin_h].edge = edge_idx;
        }
    }

    rebuild_edge_lut();
    return true;
}

uint32_t HalfEdgeMesh::add_vertex(const Vec3f& p) {
    uint32_t v_idx = static_cast<uint32_t>(vertices.size());
    vertices.emplace_back();
    vertices[v_idx].pos = p;
    vertices[v_idx].halfedge = INVALID_INDEX;
    vertices[v_idx].is_deleted = false;
    return v_idx;
}

uint32_t HalfEdgeMesh::add_face(const std::vector<uint32_t>& v_indices, uint16_t mat_id) {
    if (v_indices.size() < 3) return INVALID_INDEX;

    uint32_t f_idx = static_cast<uint32_t>(faces.size());
    faces.emplace_back();
    faces[f_idx].material_id = mat_id;
    faces[f_idx].is_deleted = false;

    uint32_t base_h = static_cast<uint32_t>(halfedges.size());
    size_t n = v_indices.size();
    halfedges.resize(base_h + n);
    faces[f_idx].halfedge = base_h;

    for (size_t i = 0; i < n; ++i) {
        uint32_t cur_h = base_h + static_cast<uint32_t>(i);
        uint32_t next_h = base_h + static_cast<uint32_t>((i + 1) % n);
        uint32_t prev_h = base_h + static_cast<uint32_t>((i + n - 1) % n);
        uint32_t cur_v = v_indices[i];
        uint32_t next_v = v_indices[(i + 1) % n];

        halfedges[cur_h].to_vertex = next_v;
        halfedges[cur_h].face = f_idx;
        halfedges[cur_h].next = next_h;
        halfedges[cur_h].prev = prev_h;
        halfedges[cur_h].is_deleted = false;

        if (vertices[cur_v].halfedge == INVALID_INDEX) {
            vertices[cur_v].halfedge = cur_h;
        }

        // Check if opposite halfedge exists
        uint32_t opp_e = find_edge(cur_v, next_v);
        if (opp_e != INVALID_INDEX) {
            uint32_t other_h = edges[opp_e].halfedge;
            halfedges[cur_h].twin = other_h;
            halfedges[cur_h].edge = opp_e;
            halfedges[other_h].twin = cur_h;
        } else {
            uint32_t new_e = static_cast<uint32_t>(edges.size());
            edges.emplace_back();
            edges[new_e].halfedge = cur_h;
            halfedges[cur_h].edge = new_e;
            halfedges[cur_h].twin = INVALID_INDEX;
        }
    }

    return f_idx;
}

bool HalfEdgeMesh::is_boundary_halfedge(uint32_t h) const {
    if (!is_valid_halfedge(h)) return true;
    return halfedges[h].twin == INVALID_INDEX || halfedges[h].face == INVALID_INDEX;
}

bool HalfEdgeMesh::is_boundary_edge(uint32_t e) const {
    if (!is_valid_edge(e)) return true;
    uint32_t h = edges[e].halfedge;
    if (h == INVALID_INDEX) return true;
    return halfedges[h].twin == INVALID_INDEX;
}

bool HalfEdgeMesh::is_boundary_vertex(uint32_t v) const {
    if (!is_valid_vertex(v)) return true;
    for (uint32_t h : get_vertex_outgoing_halfedges(v)) {
        if (halfedges[h].twin == INVALID_INDEX) return true;
    }
    return false;
}

std::vector<uint32_t> HalfEdgeMesh::get_vertex_outgoing_halfedges(uint32_t v) const {
    std::vector<uint32_t> result;
    if (!is_valid_vertex(v) || vertices[v].halfedge == INVALID_INDEX) return result;

    uint32_t h = vertices[v].halfedge;
    uint32_t start_h = h;
    size_t guard = 0;

    // Scan backwards if on boundary to start at boundary edge
    uint32_t curr = h;
    while (guard++ < 200) {
        uint32_t prev_h = halfedges[curr].prev;
        uint32_t twin_prev = halfedges[prev_h].twin;
        if (twin_prev == INVALID_INDEX) {
            start_h = curr;
            break;
        }
        curr = twin_prev;
        if (curr == h) {
            start_h = h;
            break;
        }
    }

    curr = start_h;
    guard = 0;
    do {
        if (!is_valid_halfedge(curr)) break;
        result.push_back(curr);
        uint32_t twin_h = halfedges[curr].twin;
        if (twin_h == INVALID_INDEX) break;
        curr = halfedges[twin_h].next;
    } while (curr != start_h && guard++ < 200);

    return result;
}

std::vector<uint32_t> HalfEdgeMesh::get_vertex_neighbors(uint32_t v) const {
    std::vector<uint32_t> neighbors;
    auto out_h = get_vertex_outgoing_halfedges(v);
    neighbors.reserve(out_h.size());
    for (uint32_t h : out_h) {
        neighbors.push_back(halfedges[h].to_vertex);
    }
    return neighbors;
}

std::vector<uint32_t> HalfEdgeMesh::get_vertex_faces(uint32_t v) const {
    std::vector<uint32_t> v_faces;
    auto out_h = get_vertex_outgoing_halfedges(v);
    for (uint32_t h : out_h) {
        uint32_t f = halfedges[h].face;
        if (f != INVALID_INDEX && is_valid_face(f)) {
            v_faces.push_back(f);
        }
    }
    return v_faces;
}

std::vector<uint32_t> HalfEdgeMesh::get_face_vertices(uint32_t f) const {
    std::vector<uint32_t> f_vertices;
    if (!is_valid_face(f)) return f_vertices;

    uint32_t h = faces[f].halfedge;
    uint32_t curr = h;
    size_t guard = 0;
    do {
        f_vertices.push_back(halfedges[curr].to_vertex);
        curr = halfedges[curr].next;
    } while (curr != h && guard++ < 100);

    return f_vertices;
}

std::vector<uint32_t> HalfEdgeMesh::get_face_halfedges(uint32_t f) const {
    std::vector<uint32_t> f_hedges;
    if (!is_valid_face(f)) return f_hedges;

    uint32_t h = faces[f].halfedge;
    uint32_t curr = h;
    size_t guard = 0;
    do {
        f_hedges.push_back(curr);
        curr = halfedges[curr].next;
    } while (curr != h && guard++ < 100);

    return f_hedges;
}

Vec3f HalfEdgeMesh::compute_face_normal(uint32_t f) const {
    auto f_v = get_face_vertices(f);
    if (f_v.size() < 3) return Vec3f(0.0f, 0.0f, 1.0f);
    return triangle_normal(vertices[f_v[0]].pos, vertices[f_v[1]].pos, vertices[f_v[2]].pos);
}

float HalfEdgeMesh::compute_face_area(uint32_t f) const {
    auto f_v = get_face_vertices(f);
    if (f_v.size() < 3) return 0.0f;
    if (f_v.size() == 3) {
        return triangle_area(vertices[f_v[0]].pos, vertices[f_v[1]].pos, vertices[f_v[2]].pos);
    }
    // Polygon area via fan
    float area = 0.0f;
    for (size_t i = 1; i + 1 < f_v.size(); ++i) {
        area += triangle_area(vertices[f_v[0]].pos, vertices[f_v[i]].pos, vertices[f_v[i + 1]].pos);
    }
    return area;
}

Vec3f HalfEdgeMesh::compute_face_centroid(uint32_t f) const {
    auto f_v = get_face_vertices(f);
    if (f_v.empty()) return Vec3f::Zero();
    Vec3f c = Vec3f::Zero();
    for (uint32_t v : f_v) c += vertices[v].pos;
    return c / static_cast<float>(f_v.size());
}

Vec3f HalfEdgeMesh::compute_vertex_normal(uint32_t v) const {
    if (!is_valid_vertex(v)) return Vec3f(0.0f, 0.0f, 1.0f);
    Vec3f n_sum = Vec3f::Zero();
    const Vec3f& p = vertices[v].pos;

    for (uint32_t f : get_vertex_faces(v)) {
        auto f_v = get_face_vertices(f);
        if (f_v.size() < 3) continue;
        int idx = -1;
        for (int i = 0; i < static_cast<int>(f_v.size()); ++i) {
            if (f_v[i] == v) { idx = i; break; }
        }
        if (idx < 0) continue;
        int prev_i = (idx + static_cast<int>(f_v.size()) - 1) % f_v.size();
        int next_i = (idx + 1) % f_v.size();
        Vec3f d1 = (vertices[f_v[prev_i]].pos - p).normalized();
        Vec3f d2 = (vertices[f_v[next_i]].pos - p).normalized();
        float angle = std::acos(clampf(d1.dot(d2), -1.0f, 1.0f));
        Vec3f fn = compute_face_normal(f);
        n_sum += angle * fn;
    }

    float len = n_sum.norm();
    if (len > 1e-8f) return n_sum / len;
    return Vec3f(0.0f, 0.0f, 1.0f);
}

float HalfEdgeMesh::compute_edge_length(uint32_t e) const {
    if (!is_valid_edge(e)) return 0.0f;
    uint32_t h = edges[e].halfedge;
    if (!is_valid_halfedge(h)) return 0.0f;
    uint32_t u = from_vertex(h);
    uint32_t v = halfedges[h].to_vertex;
    return (vertices[u].pos - vertices[v].pos).norm();
}

Vec3f HalfEdgeMesh::compute_edge_midpoint(uint32_t e) const {
    if (!is_valid_edge(e)) return Vec3f::Zero();
    uint32_t h = edges[e].halfedge;
    if (!is_valid_halfedge(h)) return Vec3f::Zero();
    uint32_t u = from_vertex(h);
    uint32_t v = halfedges[h].to_vertex;
    return (vertices[u].pos + vertices[v].pos) * 0.5f;
}

BoundingBox HalfEdgeMesh::compute_bounding_box() const {
    BoundingBox bbox;
    for (const auto& v : vertices) {
        if (!v.is_deleted) bbox.expand(v.pos);
    }
    return bbox;
}

float HalfEdgeMesh::compute_total_surface_area() const {
    float total = 0.0f;
    for (uint32_t f = 0; f < faces.size(); ++f) {
        if (is_valid_face(f)) total += compute_face_area(f);
    }
    return total;
}

void HalfEdgeMesh::rebuild_edge_lut() const {
    edge_lut.clear();
    edge_lut.reserve(edges.size() * 2);
    for (size_t e = 0; e < edges.size(); ++e) {
        if (edges[e].is_deleted) continue;
        uint32_t h0 = edges[e].halfedge;
        if (h0 == INVALID_INDEX || halfedges[h0].is_deleted) continue;
        uint32_t u = from_vertex(h0);
        uint32_t v = halfedges[h0].to_vertex;
        uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
        edge_lut[key] = static_cast<uint32_t>(e);
    }
}

uint32_t HalfEdgeMesh::find_edge(uint32_t v0, uint32_t v1) const {
    if (!is_valid_vertex(v0) || !is_valid_vertex(v1)) return INVALID_INDEX;
    if (edge_lut.empty() && !edges.empty()) {
        rebuild_edge_lut();
    }
    uint64_t key = (static_cast<uint64_t>(std::min(v0, v1)) << 32) | std::max(v0, v1);
    auto it = edge_lut.find(key);
    if (it != edge_lut.end()) return it->second;
    return INVALID_INDEX;
}

void HalfEdgeMesh::set_sharp_edge(uint32_t v0, uint32_t v1, float crease) {
    uint32_t e = find_edge(v0, v1);
    if (e != INVALID_INDEX) {
        edges[e].is_sharp = true;
        edges[e].crease = crease;
        vertices[v0].is_sharp = true;
        vertices[v1].is_sharp = true;
    }
}

void HalfEdgeMesh::set_seam_edge(uint32_t v0, uint32_t v1) {
    uint32_t e = find_edge(v0, v1);
    if (e != INVALID_INDEX) {
        edges[e].is_seam = true;
        vertices[v0].is_seam = true;
        vertices[v1].is_seam = true;
    }
}

uint32_t HalfEdgeMesh::split_edge(uint32_t e, const Vec3f& new_pt) {
    if (!is_valid_edge(e)) return INVALID_INDEX;
    uint32_t h0 = edges[e].halfedge;
    uint32_t h1 = halfedges[h0].twin;

    uint32_t u = from_vertex(h0);
    uint32_t v = halfedges[h0].to_vertex;

    uint32_t new_v = add_vertex(new_pt);
    vertices[new_v].density_weight = (vertices[u].density_weight + vertices[v].density_weight) * 0.5f;
    vertices[new_v].is_sharp = edges[e].is_sharp;
    vertices[new_v].is_seam = edges[e].is_seam;

    bool was_sharp = edges[e].is_sharp;
    bool was_seam = edges[e].is_seam;
    float crease_val = edges[e].crease;

    uint32_t f0 = halfedges[h0].face;
    uint32_t f1 = (h1 != INVALID_INDEX) ? halfedges[h1].face : INVALID_INDEX;

    if (f0 != INVALID_INDEX && is_valid_face(f0)) {
        uint32_t next_h = halfedges[h0].next;
        uint32_t opp_v = halfedges[next_h].to_vertex;
        uint16_t mat = faces[f0].material_id;

        faces[f0].is_deleted = true;
        add_face({u, new_v, opp_v}, mat);
        add_face({new_v, v, opp_v}, mat);
    }

    if (f1 != INVALID_INDEX && is_valid_face(f1)) {
        uint32_t next_h = halfedges[h1].next;
        uint32_t opp_v = halfedges[next_h].to_vertex;
        uint16_t mat = faces[f1].material_id;

        faces[f1].is_deleted = true;
        add_face({v, new_v, opp_v}, mat);
        add_face({new_v, u, opp_v}, mat);
    }

    edges[e].is_deleted = true;

    // Propagate feature attributes to the two split half-edges
    if (was_sharp) {
        set_sharp_edge(u, new_v, crease_val);
        set_sharp_edge(new_v, v, crease_val);
    }
    if (was_seam) {
        set_seam_edge(u, new_v);
        set_seam_edge(new_v, v);
    }

    edge_lut.clear();
    return new_v;
}

bool HalfEdgeMesh::flip_edge(uint32_t e) {
    if (!is_valid_edge(e)) return false;
    uint32_t h0 = edges[e].halfedge;
    uint32_t h1 = halfedges[h0].twin;
    if (h1 == INVALID_INDEX) return false; // Boundary edge cannot be flipped
    if (edges[e].is_sharp || edges[e].is_seam) return false; // Feature edge protected

    uint32_t f0 = halfedges[h0].face;
    uint32_t f1 = halfedges[h1].face;
    if (!is_valid_face(f0) || !is_valid_face(f1)) return false;

    uint32_t v0 = from_vertex(h0);
    uint32_t v1 = halfedges[h0].to_vertex;
    uint32_t v2 = halfedges[halfedges[h0].next].to_vertex;
    uint32_t v3 = halfedges[halfedges[h1].next].to_vertex;

    // Topological flip on 2 triangles (v0, v1, v2) and (v1, v0, v3) -> (v2, v3, v1) and (v3, v2, v0)
    uint32_t h0_next = halfedges[h0].next;
    uint32_t h0_prev = halfedges[h0].prev;
    uint32_t h1_next = halfedges[h1].next;
    uint32_t h1_prev = halfedges[h1].prev;

    halfedges[h0].to_vertex = v3;
    halfedges[h1].to_vertex = v2;

    halfedges[h0].next = h1_prev;
    halfedges[h0].prev = h0_next;
    halfedges[h1].next = h0_prev;
    halfedges[h1].prev = h1_next;

    halfedges[h0_next].next = h0;
    halfedges[h0_next].prev = h1_prev;
    halfedges[h0_next].face = f0;

    halfedges[h1_prev].next = h0_next;
    halfedges[h1_prev].prev = h0;
    halfedges[h1_prev].face = f0;

    halfedges[h1_next].next = h1;
    halfedges[h1_next].prev = h0_prev;
    halfedges[h1_next].face = f1;

    halfedges[h0_prev].next = h1_next;
    halfedges[h0_prev].prev = h1;
    halfedges[h0_prev].face = f1;

    faces[f0].halfedge = h0;
    faces[f1].halfedge = h1;

    vertices[v0].halfedge = h0_prev;
    vertices[v1].halfedge = h1_prev;

    // Edge e now spans (v2,v3) instead of (v0,v1): cached key->index pairs lie.
    edge_lut.clear();

    return true;
}

bool HalfEdgeMesh::collapse_edge(uint32_t e, const Vec3f& target_pt) {
    if (!is_valid_edge(e)) return false;
    uint32_t h0 = edges[e].halfedge;
    uint32_t h1 = halfedges[h0].twin;

    uint32_t v0 = from_vertex(h0);
    uint32_t v1 = halfedges[h0].to_vertex;

    if (vertices[v0].is_corner && vertices[v1].is_corner) return false;
    if (edges[e].is_sharp && !vertices[v0].is_sharp) return false;

    // Link condition check
    auto nbrs0 = get_vertex_neighbors(v0);
    auto nbrs1 = get_vertex_neighbors(v1);
    std::unordered_set<uint32_t> set0(nbrs0.begin(), nbrs0.end());
    size_t common_count = 0;
    for (uint32_t n : nbrs1) {
        if (set0.count(n)) common_count++;
    }

    bool is_bnd = is_boundary_edge(e);
    if (is_bnd && common_count != 1) return false;
    if (!is_bnd && common_count != 2) return false;

    // Normal inversion check for triangles around v0
    uint32_t f0 = halfedges[h0].face;
    uint32_t f1 = (h1 != INVALID_INDEX) ? halfedges[h1].face : INVALID_INDEX;

    for (uint32_t f : get_vertex_faces(v0)) {
        if (f == f0 || f == f1 || !is_valid_face(f)) continue;
        auto f_v = get_face_vertices(f);
        if (f_v.size() != 3) continue;

        Vec3f old_n = triangle_normal(vertices[f_v[0]].pos, vertices[f_v[1]].pos, vertices[f_v[2]].pos);
        Vec3f p[3];
        for (int i = 0; i < 3; ++i) {
            p[i] = (f_v[i] == v0) ? target_pt : vertices[f_v[i]].pos;
        }
        Vec3f new_n = triangle_normal(p[0], p[1], p[2]);
        if (old_n.dot(new_n) < 0.1f) return false; // Inversion detected
    }

    // Helper to relink face boundary on collapse
    auto relink_collapsed_face = [&](uint32_t f_idx, uint32_t h_edge) {
        if (f_idx == INVALID_INDEX || !is_valid_face(f_idx)) return;
        uint32_t h_next = halfedges[h_edge].next;
        uint32_t h_prev = halfedges[h_edge].prev;
        uint32_t t_next = halfedges[h_next].twin;
        uint32_t t_prev = halfedges[h_prev].twin;

        if (t_next != INVALID_INDEX) {
            halfedges[t_next].twin = t_prev;
        }
        if (t_prev != INVALID_INDEX) {
            halfedges[t_prev].twin = t_next;
        }

        uint32_t e_next = halfedges[h_next].edge;
        if (e_next != INVALID_INDEX && edges[e_next].halfedge == h_next) {
            edges[e_next].halfedge = t_prev;
        }
        uint32_t e_prev = halfedges[h_prev].edge;
        if (e_prev != INVALID_INDEX && edges[e_prev].halfedge == h_prev) {
            edges[e_prev].halfedge = t_next;
        }

        halfedges[h_next].is_deleted = true;
        halfedges[h_prev].is_deleted = true;
        faces[f_idx].is_deleted = true;
    };

    vertices[v1].pos = target_pt;
    if (vertices[v0].is_sharp) vertices[v1].is_sharp = true;
    if (vertices[v0].is_seam) vertices[v1].is_seam = true;

    // Move all edges from v0 to v1
    for (uint32_t h : get_vertex_outgoing_halfedges(v0)) {
        uint32_t twin_h = halfedges[h].twin;
        if (twin_h != INVALID_INDEX) {
            halfedges[twin_h].to_vertex = v1;
        }
    }

    relink_collapsed_face(f0, h0);
    if (h1 != INVALID_INDEX && f1 != INVALID_INDEX) {
        relink_collapsed_face(f1, h1);
    }

    edges[e].is_deleted = true;
    halfedges[h0].is_deleted = true;
    if (h1 != INVALID_INDEX) halfedges[h1].is_deleted = true;

    // Connectivity changed: cached (u,v) -> edge index pairs are stale from here on.
    edge_lut.clear();

    vertices[v0].is_deleted = true;
    // Find any valid outgoing halfedge for v1 from its surviving faces
    vertices[v1].halfedge = INVALID_INDEX;
    for (uint32_t f : get_vertex_faces(v1)) {
        if (is_valid_face(f)) {
            for (uint32_t h : get_face_halfedges(f)) {
                if (from_vertex(h) == v1) {
                    vertices[v1].halfedge = h;
                    break;
                }
            }
            if (vertices[v1].halfedge != INVALID_INDEX) break;
        }
    }
    return true;
}

void HalfEdgeMesh::compact() {
    // Re-index mesh elements removing deleted entries
    std::vector<uint32_t> v_map(vertices.size(), INVALID_INDEX);
    std::vector<Vertex> new_vertices;
    new_vertices.reserve(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        if (!vertices[i].is_deleted) {
            v_map[i] = static_cast<uint32_t>(new_vertices.size());
            new_vertices.push_back(vertices[i]);
            new_vertices.back().halfedge = INVALID_INDEX;
        }
    }

    // Preserve edge attributes across compaction
    struct SavedEdgeAttr { float crease = 0.0f; bool is_sharp = false; bool is_seam = false; bool is_symmetry = false; };
    std::unordered_map<uint64_t, SavedEdgeAttr> edge_attrs;
    for (size_t e = 0; e < edges.size(); ++e) {
        if (!is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h = edges[e].halfedge;
        if (!is_valid_halfedge(h)) continue;
        uint32_t u = from_vertex(h);
        uint32_t v = halfedges[h].to_vertex;
        if (u < vertices.size() && v < vertices.size() && !vertices[u].is_deleted && !vertices[v].is_deleted) {
            uint32_t nu = v_map[u];
            uint32_t nv = v_map[v];
            if (nu != INVALID_INDEX && nv != INVALID_INDEX) {
                uint64_t key = (static_cast<uint64_t>(std::min(nu, nv)) << 32) | std::max(nu, nv);
                edge_attrs[key] = { edges[e].crease, edges[e].is_sharp, edges[e].is_seam, edges[e].is_symmetry };
            }
        }
    }

    // Extract faces as vertex loops
    std::vector<std::vector<uint32_t>> f_loops;
    std::vector<uint16_t> mat_ids;
    for (size_t f = 0; f < faces.size(); ++f) {
        if (!faces[f].is_deleted) {
            auto old_v = get_face_vertices(static_cast<uint32_t>(f));
            if (old_v.size() >= 3) {
                std::vector<uint32_t> loop;
                bool ok = true;
                for (uint32_t ov : old_v) {
                    if (v_map[ov] == INVALID_INDEX) { ok = false; break; }
                    loop.push_back(v_map[ov]);
                }
                if (ok) {
                    f_loops.push_back(std::move(loop));
                    mat_ids.push_back(faces[f].material_id);
                }
            }
        }
    }

    vertices = std::move(new_vertices);
    halfedges.clear();
    edges.clear();
    faces.clear();

    faces.resize(f_loops.size());
    std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, DirectedEdgeHash> edge_map;

    for (size_t f = 0; f < f_loops.size(); ++f) {
        faces[f].material_id = mat_ids[f];
        faces[f].is_deleted = false;

        const auto& loop = f_loops[f];
        size_t n = loop.size();
        uint32_t base_h = static_cast<uint32_t>(halfedges.size());
        halfedges.resize(base_h + n);
        faces[f].halfedge = base_h;

        for (size_t i = 0; i < n; ++i) {
            uint32_t cur_h = base_h + static_cast<uint32_t>(i);
            uint32_t next_h = base_h + static_cast<uint32_t>((i + 1) % n);
            uint32_t prev_h = base_h + static_cast<uint32_t>((i + n - 1) % n);
            uint32_t cur_v = loop[i];
            uint32_t next_v = loop[(i + 1) % n];

            halfedges[cur_h].to_vertex = next_v;
            halfedges[cur_h].face = static_cast<uint32_t>(f);
            halfedges[cur_h].next = next_h;
            halfedges[cur_h].prev = prev_h;
            halfedges[cur_h].twin = INVALID_INDEX;
            halfedges[cur_h].edge = INVALID_INDEX;
            halfedges[cur_h].is_deleted = false;

            if (vertices[cur_v].halfedge == INVALID_INDEX) {
                vertices[cur_v].halfedge = cur_h;
            }

            edge_map[{cur_v, next_v}] = cur_h;
        }
    }

    // Resolve twins and create edges
    for (size_t h = 0; h < halfedges.size(); ++h) {
        if (halfedges[h].is_deleted || halfedges[h].twin != INVALID_INDEX) continue;
        uint32_t u = from_vertex(static_cast<uint32_t>(h));
        uint32_t v = halfedges[h].to_vertex;

        auto it = edge_map.find({v, u});
        uint32_t twin_h = (it != edge_map.end()) ? it->second : INVALID_INDEX;

        uint32_t edge_idx = static_cast<uint32_t>(edges.size());
        edges.emplace_back();
        edges[edge_idx].halfedge = static_cast<uint32_t>(h);

        halfedges[h].edge = edge_idx;
        halfedges[h].twin = twin_h;

        if (twin_h != INVALID_INDEX) {
            halfedges[twin_h].twin = static_cast<uint32_t>(h);
            halfedges[twin_h].edge = edge_idx;
        }
    }

    // Restore edge attributes
    for (size_t e = 0; e < edges.size(); ++e) {
        uint32_t h = edges[e].halfedge;
        if (h != INVALID_INDEX) {
            uint32_t u = from_vertex(h);
            uint32_t v = halfedges[h].to_vertex;
            uint64_t key = (static_cast<uint64_t>(std::min(u, v)) << 32) | std::max(u, v);
            auto it = edge_attrs.find(key);
            if (it != edge_attrs.end()) {
                edges[e].crease = it->second.crease;
                edges[e].is_sharp = it->second.is_sharp;
                edges[e].is_seam = it->second.is_seam;
                edges[e].is_symmetry = it->second.is_symmetry;
            }
        }
    }

    // The edge array was rebuilt (shrunk/renumbered): drop cached indices, they
    // may now point past the end of `edges` and corrupt memory on next find_edge.
    edge_lut.clear();
}

} // namespace qremesh
