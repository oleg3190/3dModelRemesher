#pragma once

#include "common/types.h"
#include <vector>
#include <memory>

namespace qremesh {

class HalfEdgeMesh;

struct BVHTriangle {
    Vec3f v0, v1, v2;
    Vec3f normal;
    uint32_t tri_idx = 0;

    BoundingBox bbox() const {
        BoundingBox b;
        b.expand(v0);
        b.expand(v1);
        b.expand(v2);
        return b;
    }

    Vec3f centroid() const {
        return (v0 + v1 + v2) / 3.0f;
    }
};

struct BVHNode {
    BoundingBox bbox;
    uint32_t left_child = INVALID_INDEX;
    uint32_t right_child = INVALID_INDEX;
    uint32_t first_tri = 0;
    uint32_t tri_count = 0;

    bool is_leaf() const {
        return left_child == INVALID_INDEX && right_child == INVALID_INDEX;
    }
};

class BVHTree {
public:
    BVHTree() = default;

    // Build from triangle positions array
    bool build(const float* positions, uint32_t nv, const uint32_t* tri_indices, uint32_t nt);

    // Build from HalfEdgeMesh
    bool build_from_halfedge(const HalfEdgeMesh& mesh);

    // Queries
    bool closest_point(
        const Vec3f& query_pt,
        Vec3f& out_pt,
        Vec3f& out_normal,
        float& out_dist_sq,
        uint32_t* out_tri_idx = nullptr
    ) const;

    bool ray_intersect(
        const Vec3f& origin,
        const Vec3f& dir,
        float max_t,
        float& out_t,
        Vec3f& out_pt,
        Vec3f& out_normal
    ) const;

    const BoundingBox& get_root_bbox() const {
        static BoundingBox empty;
        return nodes.empty() ? empty : nodes[0].bbox;
    }

private:
    std::vector<BVHTriangle> triangles;
    std::vector<BVHNode> nodes;

    uint32_t build_recursive(uint32_t start, uint32_t count, int depth);
    void closest_point_recursive(
        uint32_t node_idx,
        const Vec3f& query_pt,
        float& best_dist_sq,
        Vec3f& best_pt,
        Vec3f& best_normal,
        uint32_t& best_tri_idx
    ) const;
};

// Distance from point to 3D triangle (Christer Ericson, Real-Time Collision Detection)
Vec3f closest_point_on_triangle(const Vec3f& p, const Vec3f& a, const Vec3f& b, const Vec3f& c);

// Distance from point to AABB squared
float point_aabb_dist_sq(const Vec3f& p, const BoundingBox& b);

} // namespace qremesh
