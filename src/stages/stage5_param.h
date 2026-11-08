#pragma once

#include "remesh_context.h"
#include "stages/stage4_patch.h"

namespace qremesh {

// T17 record: a pleat whose two faces live in DIFFERENT patches (pa != pb) and share the seam
// loop edge (a,b). Captured unconditionally by the stage5 pleat census; consumed only by the
// env-gated paired probe t17_paired_probe (QREMESH_T17, default OFF).
struct T17CrossPleat {
    uint32_t f0 = 0, f1 = 0, a = 0, b = 0, pa = 0, pb = 0;
};

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

    // T17: paired probe - rotate the free vertices of BOTH patches around the shared seam edge
    // of a cross-patch pleat (symmetric flag reflection; the seam loop a,b never moves).
    // Env QREMESH_T17, default OFF: without it the mesh stays byte-identical.
    static void t17_paired_probe(
        RemeshContext& ctx,
        QuadMeshResult& out_quads,
        const std::vector<uint32_t>& face_patch,
        const std::vector<T17CrossPleat>& records,
        const std::vector<float>& ov,
        const std::vector<float>& gp
    );

    static void weld_and_inherit_attributes(
        RemeshContext& ctx,
        QuadMeshResult& out_quads
    );
};

} // namespace qremesh
