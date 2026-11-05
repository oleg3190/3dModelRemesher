#include "qremesh/qremesh_c_api.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

static bool progress_callback(float p, const char* stage, void*) {
    std::cout << "[" << static_cast<int>(p * 100.0f) << "%] " << stage << std::endl;
    return true;
}

int main(int argc, char** argv) {
    std::cout << "=== QRemesh CLI ===" << std::endl;
    if (argc < 3) {
        std::cout << "Usage: qremesh_cli <input.obj> <output.obj> [target_quads]" << std::endl;
        return 0;
    }

    std::string in_path = argv[1];
    std::string out_path = argv[2];
    int target_quads = (argc > 3) ? std::atoi(argv[3]) : 3000;

    std::ifstream file(in_path);
    if (!file.is_open()) {
        std::cerr << "Error: cannot open input file " << in_path << std::endl;
        return 1;
    }

    std::vector<float> positions;
    std::vector<uint32_t> tri_indices;
    std::string line;

    while (std::getline(file, line)) {
        if (line.rfind("v ", 0) == 0) {
            std::istringstream ss(line.substr(2));
            float x, y, z;
            if (ss >> x >> y >> z) {
                positions.push_back(x);
                positions.push_back(y);
                positions.push_back(z);
            }
        } else if (line.rfind("f ", 0) == 0) {
            std::istringstream ss(line.substr(2));
            std::string tok;
            std::vector<uint32_t> face_v;
            while (ss >> tok) {
                auto slash = tok.find('/');
                uint32_t idx = std::stoul(tok.substr(0, slash)) - 1;
                face_v.push_back(idx);
            }
            if (face_v.size() >= 3) {
                // Fan triangulation
                for (size_t i = 1; i + 1 < face_v.size(); ++i) {
                    tri_indices.push_back(face_v[0]);
                    tri_indices.push_back(face_v[i]);
                    tri_indices.push_back(face_v[i + 1]);
                }
            }
        }
    }
    file.close();

    uint32_t nv = static_cast<uint32_t>(positions.size() / 3);
    uint32_t nt = static_cast<uint32_t>(tri_indices.size() / 3);
    std::cout << "Loaded " << nv << " vertices, " << nt << " triangles." << std::endl;

    QrmMeshIn mesh_in = {};
    mesh_in.positions = positions.data();
    mesh_in.tri_indices = tri_indices.data();
    mesh_in.nv = nv;
    mesh_in.nt = nt;

    QrmParams params;
    qrm_default_params(&params);
    params.target_quad_count = target_quads;

    QrmMeshOut* mesh_out = nullptr;
    QrmStatus status = qrm_remesh(&mesh_in, &params, progress_callback, nullptr, &mesh_out);

    if (status != QRM_OK || !mesh_out) {
        std::cerr << "Remeshing failed with error code: " << status << std::endl;
        return 1;
    }

    std::cout << "\n--- Quality Report ---" << std::endl;
    std::cout << "Quads: " << mesh_out->report.quad_count << std::endl;
    std::cout << "Triangles: " << mesh_out->report.tri_count << std::endl;
    std::cout << "Min Scaled Jacobian: " << mesh_out->report.scaled_jacobian_min << std::endl;
    std::cout << "Median Scaled Jacobian: " << mesh_out->report.scaled_jacobian_median << std::endl;
    std::cout << "Max Warp Angle: " << mesh_out->report.max_warp_angle_deg << " deg" << std::endl;
    std::cout << "RMS Deviation: " << mesh_out->report.rms_deviation_rel << "%" << std::endl;
    std::cout << "Hausdorff Max: " << mesh_out->report.hausdorff_max_rel << "%" << std::endl;
    std::cout << "Total Time: " << mesh_out->report.total_time_sec << " s" << std::endl;

    std::ofstream out_file(out_path);
    if (!out_file.is_open()) {
        std::cerr << "Error: cannot write output file " << out_path << std::endl;
        qrm_free_mesh_out(mesh_out);
        return 1;
    }

    for (uint32_t i = 0; i < mesh_out->nv_out; ++i) {
        out_file << "v " << mesh_out->positions[3 * i + 0] << " "
                 << mesh_out->positions[3 * i + 1] << " "
                 << mesh_out->positions[3 * i + 2] << "\n";
    }

    for (uint32_t p = 0; p < mesh_out->np_out; ++p) {
        uint32_t start = mesh_out->poly_offsets[p];
        uint32_t end = mesh_out->poly_offsets[p + 1];
        out_file << "f";
        for (uint32_t c = start; c < end; ++c) {
            out_file << " " << (mesh_out->poly_indices[c] + 1);
        }
        out_file << "\n";
    }
    out_file.close();

    std::cout << "Saved remeshed model to " << out_path << std::endl;
    qrm_free_mesh_out(mesh_out);
    return 0;
}
