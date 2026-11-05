#pragma once

#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <memory>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <functional>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include "qremesh/qremesh_c_api.h"

namespace qremesh {

using Vec3f = Eigen::Vector3f;
using Vec3d = Eigen::Vector3d;
using Vec2f = Eigen::Vector2f;
using Vec2d = Eigen::Vector2d;
using Mat3f = Eigen::Matrix3f;
using Mat3d = Eigen::Matrix3d;

constexpr float PI_F = 3.14159265358979323846f;
constexpr double PI_D = 3.141592653589793238462643383279502884;

constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();

struct BoundingBox {
    Vec3f min_pt = Vec3f::Constant(std::numeric_limits<float>::infinity());
    Vec3f max_pt = Vec3f::Constant(-std::numeric_limits<float>::infinity());

    void expand(const Vec3f& p) {
        min_pt = min_pt.cwiseMin(p);
        max_pt = max_pt.cwiseMax(p);
    }

    Vec3f center() const {
        return (min_pt + max_pt) * 0.5f;
    }

    Vec3f extents() const {
        return (max_pt - min_pt).cwiseMax(0.0f);
    }

    float diagonal() const {
        return (max_pt - min_pt).norm();
    }

    bool contains(const Vec3f& p) const {
        return (p.array() >= min_pt.array()).all() && (p.array() <= max_pt.array()).all();
    }
};

inline float clampf(float val, float low, float high) {
    return std::max(low, std::min(high, val));
}

inline float angle_between(const Vec3f& a, const Vec3f& b) {
    float dot = a.dot(b);
    float l = a.norm() * b.norm();
    if (l <= 1e-12f) return 0.0f;
    float cos_val = clampf(dot / l, -1.0f, 1.0f);
    return std::acos(cos_val);
}

inline Vec3f triangle_normal(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2) {
    Vec3f n = (p1 - p0).cross(p2 - p0);
    float len = n.norm();
    if (len > 1e-12f) {
        return n / len;
    }
    return Vec3f(0.0f, 0.0f, 1.0f);
}

inline float triangle_area(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2) {
    return 0.5f * (p1 - p0).cross(p2 - p0).norm();
}

} // namespace qremesh
