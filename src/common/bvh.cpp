#include "common/bvh.h"
#include "common/half_edge_mesh.h"
#include <numeric>
#include <algorithm>

namespace qremesh {

Vec3f closest_point_on_triangle(const Vec3f& p, const Vec3f& a, const Vec3f& b, const Vec3f& c) {
    Vec3f ab = b - a;
    Vec3f ac = c - a;
    Vec3f ap = p - a;
    float d1 = ab.dot(ap);
    float d2 = ac.dot(ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a; // Vertex region of A

    Vec3f bp = p - b;
    float d3 = ab.dot(bp);
    float d4 = ac.dot(bp);
    if (d3 >= 0.0f && d4 <= d3) return b; // Vertex region of B

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab; // Edge region of AB
    }

    Vec3f cp = p - c;
    float d5 = ab.dot(cp);
    float d6 = ac.dot(cp);
    if (d6 >= 0.0f && d5 <= d6) return c; // Vertex region of C

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + w * ac; // Edge region of AC
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b); // Edge region of BC
    }

    // Inside face region
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

float point_aabb_dist_sq(const Vec3f& p, const BoundingBox& b) {
    float dist_sq = 0.0f;
    for (int i = 0; i < 3; ++i) {
        if (p[i] < b.min_pt[i]) {
            float d = b.min_pt[i] - p[i];
            dist_sq += d * d;
        } else if (p[i] > b.max_pt[i]) {
            float d = p[i] - b.max_pt[i];
            dist_sq += d * d;
        }
    }
    return dist_sq;
}

bool BVHTree::build(const float* positions, uint32_t nv, const uint32_t* tri_indices, uint32_t nt) {
    triangles.clear();
    nodes.clear();

    if (nv == 0 || nt == 0 || !positions || !tri_indices) return false;

    triangles.resize(nt);
    for (uint32_t i = 0; i < nt; ++i) {
        uint32_t i0 = tri_indices[3 * i + 0];
        uint32_t i1 = tri_indices[3 * i + 1];
        uint32_t i2 = tri_indices[3 * i + 2];

        triangles[i].v0 = Vec3f(positions[3 * i0 + 0], positions[3 * i0 + 1], positions[3 * i0 + 2]);
        triangles[i].v1 = Vec3f(positions[3 * i1 + 0], positions[3 * i1 + 1], positions[3 * i1 + 2]);
        triangles[i].v2 = Vec3f(positions[3 * i2 + 0], positions[3 * i2 + 1], positions[3 * i2 + 2]);
        triangles[i].normal = triangle_normal(triangles[i].v0, triangles[i].v1, triangles[i].v2);
        triangles[i].tri_idx = i;
    }

    nodes.reserve(nt * 2);
    build_recursive(0, nt, 0);
    return true;
}

bool BVHTree::build_from_halfedge(const HalfEdgeMesh& mesh) {
    triangles.clear();
    nodes.clear();

    for (size_t f = 0; f < mesh.faces.size(); ++f) {
        if (!mesh.is_valid_face(static_cast<uint32_t>(f))) continue;
        auto f_v = mesh.get_face_vertices(static_cast<uint32_t>(f));
        if (f_v.size() >= 3) {
            BVHTriangle tri;
            tri.v0 = mesh.vertices[f_v[0]].pos;
            tri.v1 = mesh.vertices[f_v[1]].pos;
            tri.v2 = mesh.vertices[f_v[2]].pos;
            tri.normal = triangle_normal(tri.v0, tri.v1, tri.v2);
            tri.tri_idx = static_cast<uint32_t>(f);
            triangles.push_back(tri);
        }
    }

    if (triangles.empty()) return false;
    nodes.reserve(triangles.size() * 2);
    build_recursive(0, static_cast<uint32_t>(triangles.size()), 0);
    return true;
}

uint32_t BVHTree::build_recursive(uint32_t start, uint32_t count, int depth) {
    uint32_t node_idx = static_cast<uint32_t>(nodes.size());
    nodes.emplace_back();

    BoundingBox bbox;
    for (uint32_t i = 0; i < count; ++i) {
        bbox.expand(triangles[start + i].bbox().min_pt);
        bbox.expand(triangles[start + i].bbox().max_pt);
    }
    nodes[node_idx].bbox = bbox;

    constexpr uint32_t LEAF_THRESHOLD = 8;
    if (count <= LEAF_THRESHOLD || depth >= 32) {
        nodes[node_idx].first_tri = start;
        nodes[node_idx].tri_count = count;
        nodes[node_idx].left_child = INVALID_INDEX;
        nodes[node_idx].right_child = INVALID_INDEX;
        return node_idx;
    }

    // Split on largest axis of bbox
    Vec3f ext = bbox.extents();
    int axis = 0;
    if (ext[1] > ext[0] && ext[1] > ext[2]) axis = 1;
    else if (ext[2] > ext[0] && ext[2] > ext[1]) axis = 2;

    float mid = bbox.center()[axis];
    auto mid_iter = std::partition(
        triangles.begin() + start,
        triangles.begin() + start + count,
        [axis, mid](const BVHTriangle& tri) {
            return tri.centroid()[axis] < mid;
        }
    );

    uint32_t left_count = static_cast<uint32_t>(std::distance(triangles.begin() + start, mid_iter));
    if (left_count == 0 || left_count == count) {
        left_count = count / 2;
    }

    nodes[node_idx].first_tri = 0;
    nodes[node_idx].tri_count = 0;
    nodes[node_idx].left_child = build_recursive(start, left_count, depth + 1);
    nodes[node_idx].right_child = build_recursive(start + left_count, count - left_count, depth + 1);

    return node_idx;
}

void BVHTree::closest_point_recursive(
    uint32_t node_idx,
    const Vec3f& query_pt,
    float& best_dist_sq,
    Vec3f& best_pt,
    Vec3f& best_normal,
    uint32_t& best_tri_idx
) const {
    if (node_idx >= nodes.size()) return;
    const auto& node = nodes[node_idx];

    if (point_aabb_dist_sq(query_pt, node.bbox) >= best_dist_sq) {
        return; // Box is further than current best
    }

    if (node.is_leaf()) {
        for (uint32_t i = 0; i < node.tri_count; ++i) {
            const auto& tri = triangles[node.first_tri + i];
            Vec3f cp = closest_point_on_triangle(query_pt, tri.v0, tri.v1, tri.v2);
            float d2 = (query_pt - cp).squaredNorm();
            if (d2 < best_dist_sq) {
                best_dist_sq = d2;
                best_pt = cp;
                best_normal = tri.normal;
                best_tri_idx = tri.tri_idx;
            }
        }
        return;
    }

    // Traverse closer child first
    float dist_left = point_aabb_dist_sq(query_pt, nodes[node.left_child].bbox);
    float dist_right = point_aabb_dist_sq(query_pt, nodes[node.right_child].bbox);

    if (dist_left < dist_right) {
        closest_point_recursive(node.left_child, query_pt, best_dist_sq, best_pt, best_normal, best_tri_idx);
        closest_point_recursive(node.right_child, query_pt, best_dist_sq, best_pt, best_normal, best_tri_idx);
    } else {
        closest_point_recursive(node.right_child, query_pt, best_dist_sq, best_pt, best_normal, best_tri_idx);
        closest_point_recursive(node.left_child, query_pt, best_dist_sq, best_pt, best_normal, best_tri_idx);
    }
}

bool BVHTree::closest_point(
    const Vec3f& query_pt,
    Vec3f& out_pt,
    Vec3f& out_normal,
    float& out_dist_sq,
    uint32_t* out_tri_idx
) const {
    if (nodes.empty()) return false;
    float best_dist_sq = std::numeric_limits<float>::infinity();
    uint32_t best_tri = INVALID_INDEX;
    closest_point_recursive(0, query_pt, best_dist_sq, out_pt, out_normal, best_tri);
    out_dist_sq = best_dist_sq;
    if (out_tri_idx) *out_tri_idx = best_tri;
    return best_dist_sq < std::numeric_limits<float>::infinity();
}

bool BVHTree::ray_intersect(
    const Vec3f& origin,
    const Vec3f& dir,
    float max_t,
    float& out_t,
    Vec3f& out_pt,
    Vec3f& out_normal
) const {
    // Möller–Trumbore intersection on leaf triangles
    if (nodes.empty()) return false;
    bool hit = false;
    float best_t = max_t;

    std::vector<uint32_t> stack;
    stack.push_back(0);

    while (!stack.empty()) {
        uint32_t curr = stack.back();
        stack.pop_back();
        const auto& node = nodes[curr];

        if (node.is_leaf()) {
            for (uint32_t i = 0; i < node.tri_count; ++i) {
                const auto& tri = triangles[node.first_tri + i];
                Vec3f edge1 = tri.v1 - tri.v0;
                Vec3f edge2 = tri.v2 - tri.v0;
                Vec3f pvec = dir.cross(edge2);
                float det = edge1.dot(pvec);

                if (std::abs(det) < 1e-8f) continue;
                float inv_det = 1.0f / det;
                Vec3f tvec = origin - tri.v0;
                float u = tvec.dot(pvec) * inv_det;
                if (u < 0.0f || u > 1.0f) continue;

                Vec3f qvec = tvec.cross(edge1);
                float v = dir.dot(qvec) * inv_det;
                if (v < 0.0f || u + v > 1.0f) continue;

                float t = edge2.dot(qvec) * inv_det;
                if (t > 1e-6f && t < best_t) {
                    best_t = t;
                    out_t = t;
                    out_pt = origin + t * dir;
                    out_normal = tri.normal;
                    hit = true;
                }
            }
        } else {
            stack.push_back(node.left_child);
            stack.push_back(node.right_child);
        }
    }
    return hit;
}

} // namespace qremesh
