#include "remesh_context.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace qremesh {

RemeshContext::RemeshContext(
    const QrmMeshIn* in_mesh_,
    const QrmParams* params_,
    QrmProgressFn progress_cb_,
    void* user_data_
)
    : in_mesh(in_mesh_)
    , params(params_)
    , progress_cb(progress_cb_)
    , user_data(user_data_)
{
    start_time = std::chrono::high_resolution_clock::now();
    if (params && params->num_threads > 0) {
        tbb_control = std::make_unique<tbb::global_control>(
            tbb::global_control::max_allowed_parallelism,
            static_cast<size_t>(params->num_threads)
        );
    }
}

bool RemeshContext::notify_progress(float progress, const char* stage_name) {
    if (progress_cb) {
        return progress_cb(progress, stage_name, user_data);
    }
    return true; // continue
}

void RemeshContext::dump_stage_obj(const std::string& stage_name, const HalfEdgeMesh& mesh) const {
    if (!params || !params->debug_dump_dir || std::string(params->debug_dump_dir).empty()) return;

    std::string path = std::string(params->debug_dump_dir) + "/" + stage_name + ".obj";
    std::ofstream out(path);
    if (!out.is_open()) return;

    std::vector<uint32_t> v_map(mesh.vertices.size(), 0);
    uint32_t count = 1;
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        if (!mesh.vertices[i].is_deleted) {
            v_map[i] = count++;
            out << "v " << mesh.vertices[i].pos.x() << " "
                << mesh.vertices[i].pos.y() << " "
                << mesh.vertices[i].pos.z() << "\n";
        }
    }

    for (size_t f = 0; f < mesh.faces.size(); ++f) {
        if (!mesh.faces[f].is_deleted) {
            auto f_v = mesh.get_face_vertices(static_cast<uint32_t>(f));
            if (f_v.size() >= 3) {
                out << "f";
                for (uint32_t v : f_v) {
                    out << " " << v_map[v];
                }
                out << "\n";
            }
        }
    }
}

void RemeshContext::dump_quads_obj(const std::string& stage_name, const QuadMeshResult& qmesh) const {
    if (!params || !params->debug_dump_dir || std::string(params->debug_dump_dir).empty()) return;

    std::string path = std::string(params->debug_dump_dir) + "/" + stage_name + ".obj";
    std::ofstream out(path);
    if (!out.is_open()) return;

    for (const auto& p : qmesh.positions) {
        out << "v " << p.x() << " " << p.y() << " " << p.z() << "\n";
    }

    for (const auto& poly : qmesh.polygons) {
        out << "f";
        for (uint32_t idx : poly) {
            out << " " << (idx + 1);
        }
        out << "\n";
    }
}

} // namespace qremesh
