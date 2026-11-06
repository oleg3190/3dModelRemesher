#pragma once

#include "remesh_context.h"

namespace qremesh {

class Stage6Optimize {
public:
    static QrmStatus execute(RemeshContext& ctx, QuadMeshResult& qmesh);

    static float compute_scaled_jacobian(
        const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3
    );

    static float compute_warp_angle_deg(
        const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3
    );

private:
    static void cleanup_topology(QuadMeshResult& qmesh);

    static void fill_small_boundary_holes(
        RemeshContext& ctx,
        QuadMeshResult& qmesh,
        size_t max_hole_verts = 16
    );

    static void tangential_taubin_smoothing(
        RemeshContext& ctx,
        QuadMeshResult& qmesh,
        int iters
    );

    static void untangle_low_jacobians(
        RemeshContext& ctx,
        QuadMeshResult& qmesh
    );

    static void mirror_symmetry(
        RemeshContext& ctx,
        QuadMeshResult& qmesh,
        int axis,
        float tolerance
    );

    static void compute_final_quality_report(
        RemeshContext& ctx,
        QuadMeshResult& qmesh
    );
};

} // namespace qremesh
