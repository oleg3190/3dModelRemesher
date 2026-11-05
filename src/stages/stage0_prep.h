#pragma once

#include "remesh_context.h"

namespace qremesh {

class Stage0Prep {
public:
    static QrmStatus execute(RemeshContext& ctx);

private:
    static bool weld_vertices(
        const float* in_pos, uint32_t in_nv,
        const uint32_t* in_tris, uint32_t in_nt,
        const uint16_t* in_mats,
        float weld_eps,
        std::vector<Vec3f>& out_pos,
        std::vector<uint32_t>& out_tris,
        std::vector<uint16_t>& out_mats,
        std::vector<uint32_t>& old_to_new_v
    );

    static bool fix_manifold_and_orient(
        std::vector<Vec3f>& positions,
        std::vector<uint32_t>& triangles,
        std::vector<uint16_t>& materials
    );

    static void fill_small_holes(
        HalfEdgeMesh& mesh,
        float max_hole_perimeter
    );

    static void simplify_qem(
        HalfEdgeMesh& mesh,
        uint32_t target_face_count
    );

    static int detect_symmetry_axis(
        const HalfEdgeMesh& mesh,
        const BVHTree& bvh,
        float tolerance
    );

    static void slice_symmetry(
        HalfEdgeMesh& mesh,
        int axis,
        float tolerance
    );
};

} // namespace qremesh
