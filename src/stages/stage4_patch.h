#pragma once

#include "remesh_context.h"
#include <vector>

namespace qremesh {

struct PatchBoundaryEdge {
    uint32_t edge_id = INVALID_INDEX;
    uint32_t v_start = INVALID_INDEX;
    uint32_t v_end = INVALID_INDEX;
    std::vector<uint32_t> path_vertices;
    float continuous_len = 1.0f;
    int integer_divisions = 1;
    bool is_sharp = false;
    bool is_seam = false;
};

struct QuasiQuadPatch {
    uint32_t patch_id = 0;
    std::array<uint32_t, 4> boundary_edges; // indices into PatchGraph::edges
    std::array<bool, 4> edge_orientations;  // true if edge matches CCW loop
    std::array<uint32_t, 4> corners;        // 4 corner vertices
    int nu = 1;                             // subdivisions along u
    int nv = 1;                             // subdivisions along v
    uint16_t material_id = 0;
    std::vector<uint32_t> member_faces;    // faces in working_mesh inside patch
};

struct PatchGraph {
    std::vector<PatchBoundaryEdge> edges;
    std::vector<QuasiQuadPatch> patches;
};

class Stage4PatchGraph {
public:
    static QrmStatus execute(RemeshContext& ctx, PatchGraph& out_graph);

private:
    static void trace_separatrices(
        const RemeshContext& ctx,
        std::vector<std::vector<uint32_t>>& separatrices
    );

    static void enforce_closed_loops_on_cylinders(
        const RemeshContext& ctx,
        std::vector<std::vector<uint32_t>>& separatrices
    );

    static void build_quasi_quad_patches(
        const RemeshContext& ctx,
        const std::vector<std::vector<uint32_t>>& separatrices,
        PatchGraph& out_graph
    );
};

} // namespace qremesh
