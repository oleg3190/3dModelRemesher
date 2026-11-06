#include "qremesh/qremesh_c_api.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>

// Topology health of the produced polygon soup: boundary / non-manifold edges,
// connected components (union-find over shared vertices of the final mesh).
static void print_topology_stats(const QrmMeshOut* m) {
    std::unordered_map<uint64_t, int> edge_count;
    edge_count.reserve(static_cast<size_t>(m->total_corners) * 2);
    auto key_of = [](uint32_t a, uint32_t b) {
        return (static_cast<uint64_t>(std::min(a, b)) << 32) | std::max(a, b);
    };
    uint32_t corners = 0;
    for (uint32_t p = 0; p < m->np_out; ++p) {
        uint32_t s = m->poly_offsets[p], e = m->poly_offsets[p + 1];
        for (uint32_t c = s; c < e; ++c) {
            uint32_t a = m->poly_indices[c];
            uint32_t b = m->poly_indices[(c + 1 == e) ? s : c + 1];
            if (a != b) edge_count[key_of(a, b)]++;
        }
        corners += e - s;
    }
    uint32_t boundary = 0, nonmanifold = 0;
    for (const auto& kv : edge_count) {
        if (kv.second == 1) boundary++;
        else if (kv.second > 2) nonmanifold++;
    }

    // Connected components: union faces that share an edge (stricter than vertex contact)
    std::unordered_map<uint64_t, uint32_t> edge_owner; // edge -> first face seen
    edge_owner.reserve(edge_count.size());
    std::vector<uint32_t> parent(m->np_out);
    for (uint32_t i = 0; i < m->np_out; ++i) parent[i] = i;
    std::function<uint32_t(uint32_t)> find = [&](uint32_t x) {
        while (parent[x] != x) { parent[x] = parent[parent[x]]; x = parent[x]; }
        return x;
    };
    for (uint32_t p = 0; p < m->np_out; ++p) {
        uint32_t s = m->poly_offsets[p], e = m->poly_offsets[p + 1];
        for (uint32_t c = s; c < e; ++c) {
            uint32_t a = m->poly_indices[c];
            uint32_t b = m->poly_indices[(c + 1 == e) ? s : c + 1];
            if (a == b) continue;
            uint64_t key = key_of(a, b);
            auto it = edge_owner.find(key);
            if (it == edge_owner.end()) {
                edge_owner.emplace(key, p);
            } else {
                uint32_t ra = find(it->second), rb = find(p);
                if (ra != rb) parent[rb] = ra;
            }
        }
    }
    uint32_t comps = 0;
    for (uint32_t i = 0; i < m->np_out; ++i) if (find(i) == i) comps++;

    std::cout << "Polys: " << m->np_out << ", corners: " << corners << std::endl;
    std::cout << "Boundary edges: " << boundary << " / " << edge_count.size()
              << " (" << (100.0 * boundary / std::max<size_t>(1, edge_count.size())) << "%)" << std::endl;
    std::cout << "Non-manifold edges: " << nonmanifold << std::endl;
    std::cout << "Connected components: " << comps << " (of " << m->np_out << " polys)" << std::endl;
}

static bool progress_callback(float p, const char* stage, void*) {
    std::cout << "[" << static_cast<int>(p * 100.0f) << "%] " << stage << std::endl;
    return true;
}

int main(int argc, char** argv) {
    std::cout << "=== QRemesh CLI ===" << std::endl;
    if (argc < 2) {
        std::cout << "Usage: qremesh_cli <input.obj> [-o <output.obj>] [target_quads] [debug_dump_dir] [adaptive_density]\n"
                  << "       positional args keep working; named flags add:\n"
                  << "         -o, --output PATH        output path\n"
                  << "         --target N               target quad count\n"
                  << "         --quality[=off|safe|full] input-repair preset (default full)\n"
                  << "             safe = keep-best-2 only; full = weld500 + keep-best + debris\n"
                  << "         --nm-policy=off|keep-best   non-manifold repair (wins over preset)\n"
                  << "         --no-nm-repair              alias --nm-policy=off\n"
                  << "         --no-weld                  disable the preset weld relaxation\n"
                  << "         --drop-debris=on|off       dust removal\n"
                  << "         --single-shell / --drop-islands[=on|off]  final island drop\n"
                  << "Priority: explicit flag > --quality preset > QREMESH_* env > built-in default(off).\n"
                  << "--quality never changes the polygon target.\n";
        return 0;
    }

    std::string in_path, out_path;
    int target_quads = 3000;
    std::string dump_dir;
    float density_override = -1.0f;
    // Start every repair field at -1 (auto): only explicit flags / a preset make them
    // authoritative; otherwise the library falls back to env then OFF (baseline preserved).
    int8_t nm_keep_best = -1, drop_debris = -1, drop_islands = -1;
    float weld_scale = 0.0f;

    auto needs_val = [&](int i) -> bool { return i + 1 < argc; };
    std::vector<std::string> positional;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-o" || a == "--output") { if (needs_val(i)) out_path = argv[++i]; }
        else if (a == "--target") { if (needs_val(i)) target_quads = std::atoi(argv[++i]); }
        else if (a.rfind("--target=", 0) == 0) target_quads = std::atoi(a.substr(9).c_str());
        else if (a == "--quality") { // bare = full
            nm_keep_best = 1; drop_debris = 1; weld_scale = 500.0f; drop_islands = 0;
        }
        else if (a.rfind("--quality=", 0) == 0) {
            std::string v = a.substr(10);
            if (v == "off") { nm_keep_best = 0; drop_debris = 0; drop_islands = 0; weld_scale = 0.0f; }
            else if (v == "safe") { nm_keep_best = 1; drop_debris = 0; drop_islands = 0; weld_scale = 0.0f; }
            else /* full */ { nm_keep_best = 1; drop_debris = 1; weld_scale = 500.0f; drop_islands = 0; }
        }
        else if (a.rfind("--nm-policy=", 0) == 0) {
            std::string v = a.substr(12);
            nm_keep_best = (v == "off" || v == "keep-all" || v == "none") ? 0 : 1;
        }
        else if (a == "--no-nm-repair") nm_keep_best = 0;
        else if (a == "--no-weld") weld_scale = 0.0f;
        else if (a.rfind("--weld-scale=", 0) == 0) weld_scale = std::atof(a.substr(13).c_str());
        else if (a.rfind("--drop-debris", 0) == 0) {
            std::string v = a.size() > 13 && a[13] == '=' ? a.substr(14) : "on";
            drop_debris = (v == "off") ? 0 : 1;
        }
        else if (a == "--single-shell" || a == "--drop-islands") drop_islands = 1;
        else if (a.rfind("--drop-islands=", 0) == 0) drop_islands = (a.substr(15) == "off") ? 0 : 1;
        else if (a.rfind("--dump=", 0) == 0) dump_dir = a.substr(7);
        else if (a.rfind("--density=", 0) == 0) density_override = std::atof(a.substr(10).c_str());
        else if (!a.empty() && a[0] == '-') { std::cerr << "Unknown option: " << a << std::endl; }
        else positional.push_back(a);
    }
    // Positional fallback (legacy): in, out, target, dump, density.
    if (!positional.empty()) in_path = positional[0];
    if (out_path.empty() && positional.size() >= 2) out_path = positional[1];
    if (positional.size() >= 3 && target_quads == 3000) target_quads = std::atoi(positional[2].c_str());
    if (dump_dir.empty() && positional.size() >= 4) dump_dir = positional[3];
    if (density_override < 0.0f && positional.size() >= 5) density_override = std::atof(positional[4].c_str());

    if (in_path.empty() || out_path.empty()) {
        std::cerr << "Error: need input and output paths" << std::endl;
        return 1;
    }

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
    params.nm_keep_best = nm_keep_best;
    params.drop_debris = drop_debris;
    params.drop_islands = drop_islands;
    params.weld_scale = weld_scale;
    std::string dump_dir_local = dump_dir;
    if (!dump_dir_local.empty()) {
        params.debug_dump_dir = dump_dir_local.c_str();
    }
    if (density_override >= 0.0f) {
        params.adaptive_density = density_override;
    }

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
    std::cout << "--- Stage Timings ---" << std::endl;
    const char* stage_names[] = {
        "Stage 0 (Prep)",
        "Stage 1 (Feature Cut)",
        "Stage 2 (Sizing Field)",
        "Stage 3 (Cross Field)",
        "Stage 4 (Patch Graph)",
        "Stage 5 (Param & Extract)",
        "Stage 6 (Relax & Opt)"
    };
    for (int i = 0; i < 7; ++i) {
        std::cout << "  " << stage_names[i] << ": " << mesh_out->report.stage_times_sec[i] << " s" << std::endl;
    }
    std::cout << "Total Time: " << mesh_out->report.total_time_sec << " s" << std::endl;
    print_topology_stats(mesh_out);

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
