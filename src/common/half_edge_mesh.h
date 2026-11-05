#pragma once

#include "common/types.h"
#include <vector>
#include <unordered_map>
#include <map>

namespace qremesh {

class HalfEdgeMesh {
public:
    struct Vertex {
        Vec3f pos = Vec3f::Zero();
        uint32_t halfedge = INVALID_INDEX; // outgoing halfedge
        float density_weight = 1.0f;
        float target_scale = 1.0f;
        bool is_deleted = false;
        bool is_sharp = false;
        bool is_seam = false;
        bool is_corner = false;
        bool is_on_symmetry_plane = false;
    };

    struct HalfEdge {
        uint32_t to_vertex = INVALID_INDEX; // target vertex
        uint32_t face = INVALID_INDEX;      // incident face or INVALID_INDEX
        uint32_t edge = INVALID_INDEX;      // parent edge
        uint32_t next = INVALID_INDEX;      // next halfedge in face loop
        uint32_t prev = INVALID_INDEX;      // prev halfedge in face loop
        uint32_t twin = INVALID_INDEX;      // opposite halfedge
        bool is_deleted = false;
    };

    struct Edge {
        uint32_t halfedge = INVALID_INDEX; // one of the two halfedges
        float crease = 0.0f;
        bool is_sharp = false;
        bool is_seam = false;
        bool is_symmetry = false;
        bool is_deleted = false;
    };

    struct Face {
        uint32_t halfedge = INVALID_INDEX; // one halfedge in face
        uint16_t material_id = 0;
        bool is_deleted = false;
    };

    std::vector<Vertex> vertices;
    std::vector<HalfEdge> halfedges;
    std::vector<Edge> edges;
    std::vector<Face> faces;

    HalfEdgeMesh() = default;

    // Build from triangle soup
    bool build_from_triangles(
        const float* positions, uint32_t nv,
        const uint32_t* tri_indices, uint32_t nt,
        const uint16_t* material_ids = nullptr
    );

    // Helpers
    uint32_t add_vertex(const Vec3f& p);
    uint32_t add_face(const std::vector<uint32_t>& v_indices, uint16_t mat_id = 0);

    bool is_valid_vertex(uint32_t v) const {
        return v < vertices.size() && !vertices[v].is_deleted;
    }
    bool is_valid_halfedge(uint32_t h) const {
        return h < halfedges.size() && !halfedges[h].is_deleted;
    }
    bool is_valid_edge(uint32_t e) const {
        return e < edges.size() && !edges[e].is_deleted;
    }
    bool is_valid_face(uint32_t f) const {
        return f < faces.size() && !faces[f].is_deleted;
    }

    uint32_t from_vertex(uint32_t h) const {
        assert(is_valid_halfedge(h));
        uint32_t t = halfedges[h].twin;
        if (t != INVALID_INDEX) return halfedges[t].to_vertex;
        return halfedges[halfedges[h].prev].to_vertex;
    }

    uint32_t to_vertex(uint32_t h) const {
        assert(is_valid_halfedge(h));
        return halfedges[h].to_vertex;
    }

    bool is_boundary_vertex(uint32_t v) const;
    bool is_boundary_edge(uint32_t e) const;
    bool is_boundary_halfedge(uint32_t h) const;

    // Circulators
    std::vector<uint32_t> get_vertex_neighbors(uint32_t v) const;
    std::vector<uint32_t> get_vertex_faces(uint32_t v) const;
    std::vector<uint32_t> get_vertex_outgoing_halfedges(uint32_t v) const;
    std::vector<uint32_t> get_face_vertices(uint32_t f) const;
    std::vector<uint32_t> get_face_halfedges(uint32_t f) const;

    // Geometry
    Vec3f compute_face_normal(uint32_t f) const;
    float compute_face_area(uint32_t f) const;
    Vec3f compute_face_centroid(uint32_t f) const;
    Vec3f compute_vertex_normal(uint32_t v) const;
    float compute_edge_length(uint32_t e) const;
    Vec3f compute_edge_midpoint(uint32_t e) const;
    BoundingBox compute_bounding_box() const;
    float compute_total_surface_area() const;

    // Topological operations
    uint32_t split_edge(uint32_t e, const Vec3f& new_pt);
    bool flip_edge(uint32_t e);
    bool collapse_edge(uint32_t e, const Vec3f& target_pt);

    // Garbage collection / rebuild indexing
    void compact();

    // Mark features
    void set_sharp_edge(uint32_t v0, uint32_t v1, float crease = 1.0f);
    void set_seam_edge(uint32_t v0, uint32_t v1);
    uint32_t find_edge(uint32_t v0, uint32_t v1) const;
};

} // namespace qremesh
