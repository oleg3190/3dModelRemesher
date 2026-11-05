#include "stages/stage2_size.h"
#include <cmath>
#include <iostream>
#include <Eigen/Eigenvalues>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

namespace qremesh {

std::vector<VertexCurvature> Stage2SizingField::compute_curvatures(const HalfEdgeMesh& mesh) {
    std::vector<VertexCurvature> result(mesh.vertices.size());
    size_t nv = mesh.vertices.size();

    // Precompute vertex normals in parallel
    std::vector<Vec3f> v_normals(nv);
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nv), [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
            if (!mesh.vertices[i].is_deleted) {
                v_normals[i] = mesh.compute_vertex_normal(static_cast<uint32_t>(i));
            }
        }
    });

    tbb::parallel_for(tbb::blocked_range<size_t>(0, nv), [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
            if (mesh.vertices[i].is_deleted) continue;
            uint32_t v = static_cast<uint32_t>(i);
            const Vec3f& n_v = v_normals[v];
            const Vec3f& p_v = mesh.vertices[v].pos;

            // Construct local tangent frame (t1, t2, n_v)
            Vec3f arb = (std::abs(n_v.x()) < 0.8f) ? Vec3f(1.0f, 0.0f, 0.0f) : Vec3f(0.0f, 1.0f, 0.0f);
            Vec3f t1 = n_v.cross(arb).normalized();
            Vec3f t2 = n_v.cross(t1).normalized();

            Eigen::Matrix2f shape_tensor = Eigen::Matrix2f::Zero();
            float weight_sum = 0.0f;

            auto nbrs = mesh.get_vertex_neighbors(v);
            for (uint32_t u : nbrs) {
                if (mesh.vertices[u].is_deleted) continue;
                Vec3f edge = mesh.vertices[u].pos - p_v;
                float len = edge.norm();
                if (len <= 1e-8f) continue;

                Vec3f d_norm = v_normals[u] - n_v;
                // Project onto tangent plane
                float u1 = edge.dot(t1);
                float u2 = edge.dot(t2);
                float dn1 = d_norm.dot(t1);
                float dn2 = d_norm.dot(t2);

                float w = 1.0f / len;
                Eigen::Vector2f dir_2d(u1, u2);
                Eigen::Vector2f dn_2d(dn1, dn2);

                shape_tensor += w * (dir_2d * dn_2d.transpose() + dn_2d * dir_2d.transpose()) * 0.5f / (len * len);
                weight_sum += w;
            }

            if (weight_sum > 1e-8f) {
                shape_tensor /= weight_sum;
                Eigen::SelfAdjointEigenSolver<Eigen::Matrix2f> eigensolver(shape_tensor);
                if (eigensolver.info() == Eigen::Success) {
                    result[v].k1 = eigensolver.eigenvalues()[1];
                    result[v].k2 = eigensolver.eigenvalues()[0];

                    Eigen::Vector2f ev1 = eigensolver.eigenvectors().col(1);
                    Eigen::Vector2f ev2 = eigensolver.eigenvectors().col(0);

                    result[v].dir1 = (ev1.x() * t1 + ev1.y() * t2).normalized();
                    result[v].dir2 = (ev2.x() * t1 + ev2.y() * t2).normalized();
                }
            }
        }
    });

    return result;
}

void Stage2SizingField::apply_lipschitz_limiter(
    const HalfEdgeMesh& mesh,
    std::vector<float>& sizing,
    float max_gradient
) {
    constexpr int MAX_PASSES = 6;
    for (int pass = 0; pass < MAX_PASSES; ++pass) {
        bool changed = false;
        for (size_t e = 0; e < mesh.edges.size(); ++e) {
            if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
            uint32_t h = mesh.edges[e].halfedge;
            uint32_t u = mesh.from_vertex(h);
            uint32_t v = mesh.halfedges[h].to_vertex;

            float d = (mesh.vertices[u].pos - mesh.vertices[v].pos).norm();
            float max_diff = max_gradient * d;

            if (sizing[u] - sizing[v] > max_diff) {
                sizing[u] = sizing[v] + max_diff;
                changed = true;
            } else if (sizing[v] - sizing[u] > max_diff) {
                sizing[v] = sizing[u] + max_diff;
                changed = true;
            }
        }
        if (!changed) break;
    }
}

QrmStatus Stage2SizingField::execute(RemeshContext& ctx) {
    if (!ctx.notify_progress(0.25f, "Stage 2: Sizing Field & Curvature Analysis")) {
        return QRM_ERR_CANCELLED;
    }

    auto curvatures = compute_curvatures(ctx.working_mesh);

    // Compute reference curvature kappa_ref from 90th percentile
    std::vector<float> all_k;
    all_k.reserve(ctx.working_mesh.vertices.size());
    for (size_t i = 0; i < ctx.working_mesh.vertices.size(); ++i) {
        if (!ctx.working_mesh.vertices[i].is_deleted) {
            all_k.push_back(std::max(std::abs(curvatures[i].k1), std::abs(curvatures[i].k2)));
        }
    }

    float kappa_ref = 1.0f;
    if (!all_k.empty()) {
        std::nth_element(all_k.begin(), all_k.begin() + all_k.size() * 9 / 10, all_k.end());
        kappa_ref = std::max(all_k[all_k.size() * 9 / 10], 1e-4f);
    }

    float alpha = clampf(ctx.params->adaptive_density, 0.0f, 1.0f);
    ctx.sizing_field.resize(ctx.working_mesh.vertices.size());

    for (size_t i = 0; i < ctx.working_mesh.vertices.size(); ++i) {
        if (ctx.working_mesh.vertices[i].is_deleted) continue;
        float max_k = std::max(std::abs(curvatures[i].k1), std::abs(curvatures[i].k2));
        float curv_factor = 1.0f / (1.0f + alpha * clampf(max_k / kappa_ref, 0.0f, 1.0f));

        float user_w = ctx.working_mesh.vertices[i].density_weight;
        float weight_factor = 1.0f / std::sqrt(std::max(user_w, 0.25f));

        ctx.sizing_field[i] = ctx.base_target_length * curv_factor * weight_factor;
    }

    // Calibrate sizing field against target_quad_count if exact budget requested
    if (ctx.params->target_quad_count > 0) {
        double estimated_quads = 0.0;
        for (size_t f = 0; f < ctx.working_mesh.faces.size(); ++f) {
            if (!ctx.working_mesh.is_valid_face(static_cast<uint32_t>(f))) continue;
            auto f_v = ctx.working_mesh.get_face_vertices(static_cast<uint32_t>(f));
            if (f_v.size() < 3) continue;

            float area = ctx.working_mesh.compute_face_area(static_cast<uint32_t>(f));
            float avg_l = 0.0f;
            for (uint32_t v : f_v) avg_l += ctx.sizing_field[v];
            avg_l /= static_cast<float>(f_v.size());

            if (avg_l > 1e-6f) {
                estimated_quads += area / (avg_l * avg_l);
            }
        }

        if (estimated_quads > 1.0) {
            float scale_factor = static_cast<float>(std::sqrt(estimated_quads / ctx.params->target_quad_count));
            for (size_t i = 0; i < ctx.sizing_field.size(); ++i) {
                ctx.sizing_field[i] *= scale_factor;
            }
        }
    }

    // Apply Lipschitz gradient limiter (|nabla L| <= 0.3)
    apply_lipschitz_limiter(ctx.working_mesh, ctx.sizing_field, 0.3f);

    return QRM_OK;
}

} // namespace qremesh
