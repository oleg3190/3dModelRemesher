#pragma once

#include "remesh_context.h"
#include <vector>
#include <array>

namespace qremesh {

// A canonical path connecting two junctions (or a closed loop cut)
struct CanonicalEdgePath {
    uint32_t edge_path_id = INVALID_INDEX;
    uint32_t node_start = INVALID_INDEX;       // junction vertex in working_mesh
    uint32_t node_end = INVALID_INDEX;         // junction vertex in working_mesh
    std::vector<uint32_t> path_vertices;       // sequence of working_mesh vertices from node_start to node_end
    float continuous_len = 0.0f;               // metric length
    int integer_divisions = 2;                 // number of quad edge segments along this path (always even >= 2)
    bool is_sharp = false;
    bool is_seam = false;
    bool is_open_border = false;
    uint32_t left_patch = INVALID_INDEX;
    uint32_t right_patch = INVALID_INDEX;

    // Output quad mesh vertex indices corresponding to samples along path: size == integer_divisions + 1
    std::vector<uint32_t> out_sample_vertices;
};

struct OrientedEdgeRef {
    uint32_t edge_path_id = INVALID_INDEX;
    bool forward = true;                       // true if traversed from node_start to node_end
};

struct QuasiQuadPatch {
    uint32_t patch_id = 0;
    std::vector<OrientedEdgeRef> boundary_loop; // ordered sequence of canonical edge paths forming CCW outer boundary
    std::array<size_t, 4> corner_indices = {0, 1, 2, 3}; // indices into boundary_loop of the 4 corners
    std::vector<uint32_t> member_faces;         // faces in working_mesh inside patch
    uint16_t material_id = 0;
    Vec3f avg_normal = Vec3f(0.0f, 0.0f, 1.0f);
    int nu = 1;
    int nv = 1;
};

struct PatchGraph {
    std::vector<CanonicalEdgePath> edge_paths;
    std::vector<QuasiQuadPatch> patches;
    std::vector<uint32_t> junction_to_out_vertex; // maps working_mesh junction vertex to out_quads vertex
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
        RemeshContext& ctx,
        const std::vector<std::vector<uint32_t>>& separatrices,
        PatchGraph& out_graph
    );
};

} // namespace qremesh
