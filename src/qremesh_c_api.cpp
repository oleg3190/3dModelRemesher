#include "qremesh/qremesh_c_api.h"
#include "remesh_context.h"
#include "stages/stage0_prep.h"
#include "stages/stage1_cut.h"
#include "stages/stage2_size.h"
#include "stages/stage3_field.h"
#include "stages/stage4_patch.h"
#include "stages/stage5_param.h"
#include "stages/stage6_opt.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

void qrm_default_params(QrmParams* p) {
    if (!p) return;
    p->target_quad_count = 5000;
    p->target_edge_length = 0.0f;
    p->adaptive_density = 0.5f;
    p->max_surface_deviation = 0.20f;

    p->preserve_sharp_edges = true;
    p->sharp_angle_threshold = 45.0f;
    p->preserve_uv_seams = true;
    p->preserve_material_borders = true;
    p->preserve_open_borders = true;

    p->symmetry_axis = -1;
    p->symmetry_tolerance = 0.05f;

    p->enforce_closed_loops = true;
    p->exact_quad_count = false;
    p->allow_triangles = true;

    p->field_smooth_iters = 50;
    p->vertex_relax_iters = 20;
    p->shape_preservation = 0.90f;

    p->proxy_tri_threshold = 300000;
    p->num_threads = 0;
    p->random_seed = 42;
    p->debug_dump_dir = nullptr;

    // Input-repair / quality fields default to "auto" (-1): the library falls back to the
    // QREMESH_* env override, then to the built-in default. Post-S41 that default is
    // nm_keep_best=ON (a no-op on clean manifold input; separates genuinely self-touching
    // shells without deleting them); weld_scale / drop_debris / drop_islands default OFF and
    // are turned on only by --quality or their own flags.
    p->nm_keep_best = -1;
    p->drop_debris = -1;
    p->drop_islands = -1;
    p->weld_scale = 0.0f;
}

QrmStatus qrm_remesh(
    const QrmMeshIn*  in_mesh,
    const QrmParams*  params,
    QrmProgressFn     progress_cb,
    void*             user_data,
    QrmMeshOut**      out_mesh
) {
    if (!in_mesh || !params || !out_mesh) {
        return QRM_ERR_INVALID_INPUT;
    }
    *out_mesh = nullptr;

    if (in_mesh->nv < 4 || in_mesh->nt < 4 || !in_mesh->positions || !in_mesh->tri_indices) {
        return QRM_ERR_INVALID_INPUT;
    }

    // Input validation: check indices and finite coordinates
    for (uint32_t i = 0; i < in_mesh->nv * 3; ++i) {
        float val = in_mesh->positions[i];
        if (!std::isfinite(val)) {
            return QRM_ERR_INVALID_INPUT;
        }
    }
    for (uint32_t i = 0; i < in_mesh->nt * 3; ++i) {
        if (in_mesh->tri_indices[i] >= in_mesh->nv) {
            return QRM_ERR_INVALID_INPUT;
        }
    }

    try {
        qremesh::RemeshContext ctx(in_mesh, params, progress_cb, user_data);

        auto time_stage = [&](int stage_idx, auto fn) -> QrmStatus {
            auto t0 = std::chrono::high_resolution_clock::now();
            QrmStatus s = fn();
            auto t1 = std::chrono::high_resolution_clock::now();
            ctx.stage_times[stage_idx] = std::chrono::duration<float>(t1 - t0).count();
            return s;
        };

        // Stage 0: Normalization, Manifold Repair & QEM Proxy
        QrmStatus status = time_stage(0, [&]() { return qremesh::Stage0Prep::execute(ctx); });
        if (status != QRM_OK) return status;

        // Stage 1: Feature Cut & Boundary Embedding
        status = time_stage(1, [&]() { return qremesh::Stage1FeatureCut::execute(ctx); });
        if (status != QRM_OK) return status;

        // Stage 2: Curvature & Sizing Field
        status = time_stage(2, [&]() { return qremesh::Stage2SizingField::execute(ctx); });
        if (status != QRM_OK) return status;

        // Stage 3: 4-RoSy Cross Field & Singularities
        status = time_stage(3, [&]() { return qremesh::Stage3CrossField::execute(ctx); });
        if (status != QRM_OK) return status;

        // Stage 4: Motorcycle Graph & Quasi-Quad Patches
        qremesh::PatchGraph graph;
        status = time_stage(4, [&]() { return qremesh::Stage4PatchGraph::execute(ctx, graph); });
        if (status != QRM_OK) return status;

        // Stage 5: Integer Parametrization & Quad Mesh Extraction
        status = time_stage(5, [&]() { return qremesh::Stage5ParamAndExtract::execute(ctx, graph, ctx.result); });
        if (status != QRM_OK) return status;

        // Stage 6: Topological Cleanup & Tangential Relaxation
        status = time_stage(6, [&]() { return qremesh::Stage6Optimize::execute(ctx, ctx.result); });
        if (status != QRM_OK) return status;

        for (int i = 0; i < 7; ++i) {
            ctx.result.report.stage_times_sec[i] = ctx.stage_times[i];
        }

        // Allocate and pack QrmMeshOut
        auto* out = static_cast<QrmMeshOut*>(std::calloc(1, sizeof(QrmMeshOut)));
        if (!out) return QRM_ERR_OUT_OF_MEMORY;

        uint32_t nv_out = static_cast<uint32_t>(ctx.result.positions.size());
        uint32_t np_out = static_cast<uint32_t>(ctx.result.polygons.size());

        out->nv_out = nv_out;
        out->np_out = np_out;

        // Positions
        out->positions = static_cast<float*>(std::malloc(nv_out * 3 * sizeof(float)));
        if (!out->positions) {
            qrm_free_mesh_out(out);
            return QRM_ERR_OUT_OF_MEMORY;
        }
        for (uint32_t i = 0; i < nv_out; ++i) {
            out->positions[3 * i + 0] = ctx.result.positions[i].x();
            out->positions[3 * i + 1] = ctx.result.positions[i].y();
            out->positions[3 * i + 2] = ctx.result.positions[i].z();
        }

        // Polygon offsets & indices
        uint32_t total_corners = 0;
        for (const auto& poly : ctx.result.polygons) {
            total_corners += static_cast<uint32_t>(poly.size());
        }
        out->total_corners = total_corners;

        out->poly_offsets = static_cast<uint32_t*>(std::malloc((np_out + 1) * sizeof(uint32_t)));
        out->poly_indices = static_cast<uint32_t*>(std::malloc(total_corners * sizeof(uint32_t)));
        out->poly_material_ids = static_cast<uint16_t*>(std::malloc(np_out * sizeof(uint16_t)));

        if (!out->poly_offsets || !out->poly_indices || !out->poly_material_ids) {
            qrm_free_mesh_out(out);
            return QRM_ERR_OUT_OF_MEMORY;
        }

        uint32_t curr_offset = 0;
        for (uint32_t p = 0; p < np_out; ++p) {
            out->poly_offsets[p] = curr_offset;
            const auto& poly = ctx.result.polygons[p];
            for (size_t c = 0; c < poly.size(); ++c) {
                out->poly_indices[curr_offset + c] = poly[c];
            }
            curr_offset += static_cast<uint32_t>(poly.size());
            out->poly_material_ids[p] = (p < ctx.result.material_ids.size()) ? ctx.result.material_ids[p] : 0;
        }
        out->poly_offsets[np_out] = curr_offset;

        // Sharp edges
        uint32_t num_sharp = static_cast<uint32_t>(ctx.result.sharp_edges.size());
        out->num_sharp_out = num_sharp;
        if (num_sharp > 0) {
            out->sharp_edge_out = static_cast<uint32_t*>(std::malloc(num_sharp * 2 * sizeof(uint32_t)));
            out->crease_edge_out = static_cast<float*>(std::malloc(num_sharp * sizeof(float)));
            if (out->sharp_edge_out && out->crease_edge_out) {
                for (uint32_t i = 0; i < num_sharp; ++i) {
                    out->sharp_edge_out[2 * i + 0] = ctx.result.sharp_edges[i].first;
                    out->sharp_edge_out[2 * i + 1] = ctx.result.sharp_edges[i].second;
                    out->crease_edge_out[i] = (i < ctx.result.crease_values.size()) ? ctx.result.crease_values[i] : 1.0f;
                }
            }
        }

        // Seam edges
        uint32_t num_seam = static_cast<uint32_t>(ctx.result.seam_edges.size());
        out->num_seam_out = num_seam;
        if (num_seam > 0) {
            out->seam_edge_out = static_cast<uint32_t*>(std::malloc(num_seam * 2 * sizeof(uint32_t)));
            if (out->seam_edge_out) {
                for (uint32_t i = 0; i < num_seam; ++i) {
                    out->seam_edge_out[2 * i + 0] = ctx.result.seam_edges[i].first;
                    out->seam_edge_out[2 * i + 1] = ctx.result.seam_edges[i].second;
                }
            }
        }

        out->report = ctx.result.report;
        *out_mesh = out;
        return QRM_OK;
    } catch (const std::bad_alloc&) {
        return QRM_ERR_OUT_OF_MEMORY;
    } catch (...) {
        return QRM_ERR_INTERNAL;
    }
}

void qrm_free_mesh_out(QrmMeshOut* out) {
    if (!out) return;
    if (out->positions) std::free(out->positions);
    if (out->poly_indices) std::free(out->poly_indices);
    if (out->poly_offsets) std::free(out->poly_offsets);
    if (out->sharp_edge_out) std::free(out->sharp_edge_out);
    if (out->crease_edge_out) std::free(out->crease_edge_out);
    if (out->seam_edge_out) std::free(out->seam_edge_out);
    if (out->poly_material_ids) std::free(out->poly_material_ids);
    std::free(out);
}
