#pragma once

#include "common/types.h"
#include "common/half_edge_mesh.h"
#include "common/bvh.h"
#include "qremesh/qremesh_c_api.h"
#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <tbb/global_control.h>

namespace qremesh {

struct QuadMeshResult {
    std::vector<Vec3f> positions;
    std::vector<std::vector<uint32_t>> polygons; // Quads (or triangles where allowed)
    std::vector<std::pair<uint32_t, uint32_t>> sharp_edges;
    std::vector<float> crease_values;
    std::vector<std::pair<uint32_t, uint32_t>> seam_edges;
    std::vector<uint16_t> material_ids;
    QrmQualityReport report;
};

class RemeshContext {
public:
    const QrmMeshIn* in_mesh = nullptr;
    const QrmParams* params = nullptr;
    QrmProgressFn progress_cb = nullptr;
    void* user_data = nullptr;

    // Timing
    std::chrono::high_resolution_clock::time_point start_time;

    // Working structures
    BVHTree original_bvh;
    HalfEdgeMesh working_mesh;
    BoundingBox bbox;
    float bbox_diag = 1.0f;
    float base_target_length = 0.1f;
    int active_symmetry_axis = -1;

    // Stage results
    std::vector<float> sizing_field; // per-vertex target edge length
    std::vector<Vec3f> cross_field_dir1; // primary direction per face (in 3D)
    std::vector<Vec3f> cross_field_dir2; // secondary direction per face (in 3D)
    std::vector<int> vertex_singularities; // index: 0 = regular, +1 = 3-val, -1 = 5-val

    // Output quad mesh
    QuadMeshResult result;

    std::unique_ptr<tbb::global_control> tbb_control;

    RemeshContext(
        const QrmMeshIn* in_mesh_,
        const QrmParams* params_,
        QrmProgressFn progress_cb_,
        void* user_data_
    );

    bool notify_progress(float progress, const char* stage_name);

    void dump_stage_obj(const std::string& stage_name, const HalfEdgeMesh& mesh) const;
    void dump_quads_obj(const std::string& stage_name, const QuadMeshResult& qmesh) const;
};

} // namespace qremesh
