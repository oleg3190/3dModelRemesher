#pragma once

#include "remesh_context.h"
#include "stages/stage4_patch.h"

namespace qremesh {

class Stage5ParamAndExtract {
public:
    static QrmStatus execute(
        RemeshContext& ctx,
        PatchGraph& graph,
        QuadMeshResult& out_quads
    );

private:
    static void balance_and_quantize_patch_sizes(
        RemeshContext& ctx,
        PatchGraph& graph,
        double effective_target_override = -1.0
    );

    static void extract_patch_quads(
        const RemeshContext& ctx,
        const QuasiQuadPatch& patch,
        PatchGraph& graph,
        QuadMeshResult& out_quads
    );

    // T16: monotone re-lay of a patch's interior grid nodes (borders stay Dirichlet).
    static void untangle_patch_grids(
        RemeshContext& ctx,
        QuadMeshResult& out_quads,
        const std::vector<uint32_t>& face_patch,
        const std::vector<uint32_t>& patches
    );

    static void weld_and_inherit_attributes(
        RemeshContext& ctx,
        QuadMeshResult& out_quads
    );
};

} // namespace qremesh
