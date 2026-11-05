#include "stages/stage3_field.h"
#include "stages/stage2_size.h"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <cmath>
#include <iostream>

namespace qremesh {

std::vector<FaceTangentFrame> Stage3CrossField::build_face_frames(const HalfEdgeMesh& mesh) {
    std::vector<FaceTangentFrame> frames(mesh.faces.size());

    for (size_t f = 0; f < mesh.faces.size(); ++f) {
        if (!mesh.is_valid_face(static_cast<uint32_t>(f))) continue;
        uint32_t h = mesh.faces[f].halfedge;
        uint32_t u = mesh.from_vertex(h);
        uint32_t v = mesh.halfedges[h].to_vertex;

        Vec3f n = mesh.compute_face_normal(static_cast<uint32_t>(f));
        Vec3f edge_vec = (mesh.vertices[v].pos - mesh.vertices[u].pos).normalized();

        Vec3f t1 = edge_vec;
        Vec3f t2 = n.cross(t1).normalized();

        frames[f].t1 = t1;
        frames[f].t2 = t2;
        frames[f].normal = n;
    }

    return frames;
}

float Stage3CrossField::compute_levi_civita_connection(
    const FaceTangentFrame& frame_i,
    const FaceTangentFrame& frame_j,
    const Vec3f& shared_edge_dir
) {
    float alpha_i = std::atan2(shared_edge_dir.dot(frame_i.t2), shared_edge_dir.dot(frame_i.t1));
    float alpha_j = std::atan2(shared_edge_dir.dot(frame_j.t2), shared_edge_dir.dot(frame_j.t1));
    return alpha_j - alpha_i;
}

void Stage3CrossField::detect_and_cancel_singularities(
    RemeshContext& ctx,
    const std::vector<FaceTangentFrame>& frames,
    const std::vector<std::complex<double>>& field
) {
    const auto& mesh = ctx.working_mesh;
    ctx.vertex_singularities.assign(mesh.vertices.size(), 0);
    std::vector<uint32_t> poles;

    for (size_t v = 0; v < mesh.vertices.size(); ++v) {
        if (mesh.vertices[v].is_deleted || mesh.is_boundary_vertex(static_cast<uint32_t>(v))) continue;
        auto out_h = mesh.get_vertex_outgoing_halfedges(static_cast<uint32_t>(v));
        if (out_h.size() < 3) continue;

        double total_angle_defect = 0.0;
        size_t n = out_h.size();

        for (size_t i = 0; i < n; ++i) {
            uint32_t h0 = out_h[i];
            uint32_t f0 = mesh.halfedges[h0].face;
            uint32_t twin0 = mesh.halfedges[h0].twin;
            if (twin0 == INVALID_INDEX || f0 == INVALID_INDEX) continue;
            uint32_t next_h = mesh.halfedges[twin0].next;
            uint32_t f1 = mesh.halfedges[next_h].face;
            if (f1 == INVALID_INDEX) continue;

            double th0 = std::arg(field[f0]) / 4.0;
            double th1 = std::arg(field[f1]) / 4.0;

            uint32_t w = mesh.halfedges[h0].to_vertex;
            Vec3f shared_edge = (mesh.vertices[w].pos - mesh.vertices[v].pos).normalized();
            float phi = compute_levi_civita_connection(frames[f0], frames[f1], shared_edge);

            double diff = th1 - th0 - phi;
            // Wrap to [-pi/4, pi/4]
            diff = std::fmod(diff + PI_D / 4.0, PI_D / 2.0);
            if (diff < 0) diff += PI_D / 2.0;
            diff -= PI_D / 4.0;

            total_angle_defect += diff;
        }

        int index = static_cast<int>(std::round(total_angle_defect / (PI_D / 2.0)));
        if (index != 0) {
            ctx.vertex_singularities[v] = index; // +1 = valence 3, -1 = valence 5
            poles.push_back(static_cast<uint32_t>(v));
        }
    }

    // Dipole cancellation only over identified poles
    for (size_t i = 0; i < poles.size(); ++i) {
        uint32_t u = poles[i];
        if (ctx.vertex_singularities[u] == 0) continue;
        for (size_t j = i + 1; j < poles.size(); ++j) {
            uint32_t v = poles[j];
            if (ctx.vertex_singularities[v] != 0 && ctx.vertex_singularities[u] == -ctx.vertex_singularities[v]) {
                float dist = (mesh.vertices[u].pos - mesh.vertices[v].pos).norm();
                float max_d = 2.0f * std::max(ctx.sizing_field[u], ctx.sizing_field[v]);
                if (dist < max_d) {
                    ctx.vertex_singularities[u] = 0;
                    ctx.vertex_singularities[v] = 0;
                    break;
                }
            }
        }
    }
}

QrmStatus Stage3CrossField::execute(RemeshContext& ctx) {
    if (!ctx.notify_progress(0.40f, "Stage 3: 4-RoSy Direction Field & Singularities")) {
        return QRM_ERR_CANCELLED;
    }

    const auto& mesh = ctx.working_mesh;
    uint32_t nf = static_cast<uint32_t>(mesh.faces.size());
    if (nf == 0) return QRM_ERR_INVALID_INPUT;

    auto frames = build_face_frames(mesh);
    auto curvatures = Stage2SizingField::compute_curvatures(mesh);

    // Identify fixed Dirichlet constraints on faces adjacent to feature edges
    std::vector<bool> is_fixed(nf, false);
    std::vector<std::complex<double>> fixed_val(nf, std::complex<double>(1.0, 0.0));

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        bool is_feature = mesh.edges[e].is_sharp || mesh.edges[e].is_seam ||
                          mesh.edges[e].is_symmetry || mesh.is_boundary_edge(static_cast<uint32_t>(e));

        if (is_feature) {
            uint32_t h = mesh.edges[e].halfedge;
            uint32_t u = mesh.from_vertex(h);
            uint32_t v = mesh.halfedges[h].to_vertex;
            Vec3f edge_dir = (mesh.vertices[v].pos - mesh.vertices[u].pos).normalized();

            uint32_t f0 = mesh.halfedges[h].face;
            uint32_t f1 = (mesh.halfedges[h].twin != INVALID_INDEX) ? mesh.halfedges[mesh.halfedges[h].twin].face : INVALID_INDEX;

            if (f0 != INVALID_INDEX && mesh.is_valid_face(f0)) {
                float theta = std::atan2(edge_dir.dot(frames[f0].t2), edge_dir.dot(frames[f0].t1));
                is_fixed[f0] = true;
                fixed_val[f0] = std::polar(1.0, 4.0 * theta);
            }
            if (f1 != INVALID_INDEX && mesh.is_valid_face(f1)) {
                float theta = std::atan2(edge_dir.dot(frames[f1].t2), edge_dir.dot(frames[f1].t1));
                is_fixed[f1] = true;
                fixed_val[f1] = std::polar(1.0, 4.0 * theta);
            }
        }
    }

    // Build BVH on working mesh for fast geometric queries
    BVHTree working_bvh;
    working_bvh.build_from_halfedge(mesh);

    // Also account for user FLOW_GUIDE curves with soft weights
    std::vector<float> soft_weights(nf, 0.0f);
    std::vector<std::complex<double>> soft_tgt(nf, std::complex<double>(0.0, 0.0));

    if (ctx.in_mesh->user_curves && ctx.in_mesh->num_curves > 0) {
        for (uint32_t c = 0; c < ctx.in_mesh->num_curves; ++c) {
            const auto& curve = ctx.in_mesh->user_curves[c];
            if (curve.role != QRM_CURVE_FLOW_GUIDE || curve.point_count < 2) continue;

            for (uint32_t p = 0; p + 1 < curve.point_count; ++p) {
                Vec3f p0(curve.points[3 * p + 0], curve.points[3 * p + 1], curve.points[3 * p + 2]);
                Vec3f p1(curve.points[3 * (p + 1) + 0], curve.points[3 * (p + 1) + 1], curve.points[3 * (p + 1) + 2]);
                Vec3f dir = (p1 - p0).normalized();

                Vec3f hit_pt, n;
                float d2;
                uint32_t hit_tri = INVALID_INDEX;
                if (working_bvh.closest_point((p0 + p1) * 0.5f, hit_pt, n, d2, &hit_tri) && hit_tri != INVALID_INDEX) {
                    float theta = std::atan2(dir.dot(frames[hit_tri].t2), dir.dot(frames[hit_tri].t1));
                    double w_guide = 50.0 * static_cast<double>(clampf(curve.weight, 0.1f, 1.0f));
                    soft_weights[hit_tri] += static_cast<float>(w_guide);
                    soft_tgt[hit_tri] += std::polar(w_guide, 4.0 * static_cast<double>(theta));
                }
            }
        }
    }

    // Add principal curvature alignment
    for (uint32_t f = 0; f < nf; ++f) {
        if (!mesh.is_valid_face(f) || is_fixed[f]) continue;
        auto f_v = mesh.get_face_vertices(f);
        if (f_v.size() < 3) continue;

        float k1 = 0.0f, k2 = 0.0f;
        Vec3f d1 = Vec3f::Zero();
        for (uint32_t v : f_v) {
            k1 += curvatures[v].k1;
            k2 += curvatures[v].k2;
            d1 += curvatures[v].dir1;
        }
        k1 /= static_cast<float>(f_v.size());
        k2 /= static_cast<float>(f_v.size());
        d1.normalize();

        float abs_k1 = std::abs(k1);
        float abs_k2 = std::abs(k2);
        float denom = abs_k1 + abs_k2 + 1e-5f;
        float anisotropy = clampf(std::abs(abs_k1 - abs_k2) / denom, 0.0f, 1.0f);

        if (anisotropy > 0.15f && d1.norm() > 0.5f) {
            float theta = std::atan2(d1.dot(frames[f].t2), d1.dot(frames[f].t1));
            double w_curv = 0.5 * static_cast<double>(anisotropy * anisotropy);
            soft_weights[f] += static_cast<float>(w_curv);
            soft_tgt[f] += std::polar(w_curv, 4.0 * static_cast<double>(theta));
        }
    }

    // Build sparse Dirichlet energy system
    // We formulate on Real and Imaginary parts: 2 * nf unknowns
    std::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd rhs = Eigen::VectorXd::Zero(2 * nf);

    for (size_t e = 0; e < mesh.edges.size(); ++e) {
        if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
        uint32_t h0 = mesh.edges[e].halfedge;
        uint32_t h1 = mesh.halfedges[h0].twin;
        if (h1 == INVALID_INDEX) continue;

        uint32_t f0 = mesh.halfedges[h0].face;
        uint32_t f1 = mesh.halfedges[h1].face;
        if (!mesh.is_valid_face(f0) || !mesh.is_valid_face(f1)) continue;

        uint32_t u = mesh.from_vertex(h0);
        uint32_t v = mesh.halfedges[h0].to_vertex;
        Vec3f edge_dir = (mesh.vertices[v].pos - mesh.vertices[u].pos).normalized();

        float phi = compute_levi_civita_connection(frames[f0], frames[f1], edge_dir);
        double c4 = std::cos(4.0 * phi);
        double s4 = std::sin(4.0 * phi);
        double w = 1.0;

        // Energy term: w * |u_1 - r_01 * u_0|^2
        // r_01 = (c4 + i * s4)
        // u_0 = (x0 + i * y0), u_1 = (x1 + i * y1)
        // r_01 * u_0 = (c4*x0 - s4*y0) + i*(s4*x0 + c4*y0)
        // Residual: [x1 - (c4*x0 - s4*y0)] and [y1 - (s4*x0 + c4*y0)]

        uint32_t idx_x0 = 2 * f0, idx_y0 = 2 * f0 + 1;
        uint32_t idx_x1 = 2 * f1, idx_y1 = 2 * f1 + 1;

        // f0 diagonal
        triplets.emplace_back(idx_x0, idx_x0, w);
        triplets.emplace_back(idx_y0, idx_y0, w);
        // f1 diagonal
        triplets.emplace_back(idx_x1, idx_x1, w);
        triplets.emplace_back(idx_y1, idx_y1, w);

        // Off-diagonals
        triplets.emplace_back(idx_x0, idx_x1, -w * c4);
        triplets.emplace_back(idx_x0, idx_y1, -w * s4);
        triplets.emplace_back(idx_y0, idx_x1,  w * s4);
        triplets.emplace_back(idx_y0, idx_y1, -w * c4);

        triplets.emplace_back(idx_x1, idx_x0, -w * c4);
        triplets.emplace_back(idx_x1, idx_y0,  w * s4);
        triplets.emplace_back(idx_y1, idx_x0, -w * s4);
        triplets.emplace_back(idx_y1, idx_y0, -w * c4);
    }

    // Apply fixed Dirichlet constraints
    constexpr double PENALTY_WEIGHT = 1e5;
    bool has_any_fixed = false;

    for (uint32_t f = 0; f < nf; ++f) {
        if (!mesh.is_valid_face(f)) continue;
        if (is_fixed[f]) {
            has_any_fixed = true;
            uint32_t idx_x = 2 * f;
            uint32_t idx_y = 2 * f + 1;
            triplets.emplace_back(idx_x, idx_x, PENALTY_WEIGHT);
            triplets.emplace_back(idx_y, idx_y, PENALTY_WEIGHT);
            rhs[idx_x] += PENALTY_WEIGHT * fixed_val[f].real();
            rhs[idx_y] += PENALTY_WEIGHT * fixed_val[f].imag();
        } else if (soft_weights[f] > 0.0f) {
            uint32_t idx_x = 2 * f;
            uint32_t idx_y = 2 * f + 1;
            triplets.emplace_back(idx_x, idx_x, soft_weights[f]);
            triplets.emplace_back(idx_y, idx_y, soft_weights[f]);
            rhs[idx_x] += soft_tgt[f].real();
            rhs[idx_y] += soft_tgt[f].imag();
        }
    }

    // If no boundary constraints exist, fix one arbitrary face to prevent nullspace
    if (!has_any_fixed && nf > 0) {
        triplets.emplace_back(0, 0, PENALTY_WEIGHT);
        triplets.emplace_back(1, 1, PENALTY_WEIGHT);
        rhs[0] += PENALTY_WEIGHT * 1.0;
        rhs[1] += PENALTY_WEIGHT * 0.0;
    }

    Eigen::SparseMatrix<double> A(2 * nf, 2 * nf);
    A.setFromTriplets(triplets.begin(), triplets.end());

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(A);

    Eigen::VectorXd sol = rhs;
    if (solver.info() == Eigen::Success) {
        sol = solver.solve(rhs);
    } else {
        // Fallback: ConjugateGradient
        Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> cg;
        cg.compute(A);
        sol = cg.solve(rhs);
        if (cg.info() != Eigen::Success) {
            return QRM_ERR_FIELD_DIVERGED;
        }
    }

    // Extract complex field and normalize
    std::vector<std::complex<double>> field(nf, std::complex<double>(1.0, 0.0));
    for (uint32_t f = 0; f < nf; ++f) {
        if (!mesh.is_valid_face(f)) continue;
        if (is_fixed[f]) {
            field[f] = fixed_val[f];
            continue;
        }
        double re = sol[2 * f];
        double im = sol[2 * f + 1];
        std::complex<double> c(re, im);
        double len = std::abs(c);
        if (len > 1e-12) {
            c /= len;
        } else {
            c = std::complex<double>(1.0, 0.0);
        }
        field[f] = c;
    }

    // Iterative unit-norm 4-RoSy Gauss-Seidel / Jacobi smoothing (params->field_smooth_iters)
    int smooth_iters = std::clamp(ctx.params->field_smooth_iters, 0, 500);
    if (smooth_iters > 0) {
        struct FaceTransport {
            uint32_t nbr_face;
            std::complex<double> rot; // exp(i * 4 * phi_{nbr -> f})
        };
        std::vector<std::vector<FaceTransport>> face_adj(nf);
        for (size_t e = 0; e < mesh.edges.size(); ++e) {
            if (!mesh.is_valid_edge(static_cast<uint32_t>(e))) continue;
            uint32_t h0 = mesh.edges[e].halfedge;
            uint32_t h1 = mesh.halfedges[h0].twin;
            if (h1 == INVALID_INDEX) continue;

            uint32_t f0 = mesh.halfedges[h0].face;
            uint32_t f1 = mesh.halfedges[h1].face;
            if (!mesh.is_valid_face(f0) || !mesh.is_valid_face(f1)) continue;

            uint32_t u = mesh.from_vertex(h0);
            uint32_t v = mesh.halfedges[h0].to_vertex;
            Vec3f edge_dir = (mesh.vertices[v].pos - mesh.vertices[u].pos).normalized();

            float phi = compute_levi_civita_connection(frames[f0], frames[f1], edge_dir);
            std::complex<double> r01 = std::polar(1.0, 4.0 * static_cast<double>(phi));
            face_adj[f1].push_back({ f0, r01 });
            face_adj[f0].push_back({ f1, std::conj(r01) });
        }

        for (int iter = 0; iter < smooth_iters; ++iter) {
            for (uint32_t f = 0; f < nf; ++f) {
                if (!mesh.is_valid_face(f) || is_fixed[f] || face_adj[f].empty()) continue;
                std::complex<double> acc = soft_tgt[f];
                for (const auto& tr : face_adj[f]) {
                    acc += tr.rot * field[tr.nbr_face];
                }
                double len = std::abs(acc);
                if (len > 1e-12) {
                    field[f] = acc / len;
                }
            }
        }
    }

    ctx.cross_field_dir1.resize(nf, Vec3f(1.0f, 0.0f, 0.0f));
    ctx.cross_field_dir2.resize(nf, Vec3f(0.0f, 1.0f, 0.0f));

    for (uint32_t f = 0; f < nf; ++f) {
        if (!mesh.is_valid_face(f)) continue;
        double theta = std::arg(field[f]) / 4.0;
        Vec3f d1 = (std::cos(theta) * frames[f].t1 + std::sin(theta) * frames[f].t2).normalized();
        Vec3f d2 = frames[f].normal.cross(d1).normalized();

        ctx.cross_field_dir1[f] = d1;
        ctx.cross_field_dir2[f] = d2;
    }

    // Detect singularities and perform dipole cancellation
    detect_and_cancel_singularities(ctx, frames, field);

    return QRM_OK;
}

} // namespace qremesh
