#pragma once

#include "remesh_context.h"
#include <complex>

namespace qremesh {

struct FaceTangentFrame {
    Vec3f t1 = Vec3f::Zero();
    Vec3f t2 = Vec3f::Zero();
    Vec3f normal = Vec3f::Zero();
};

class Stage3CrossField {
public:
    static QrmStatus execute(RemeshContext& ctx);

private:
    static std::vector<FaceTangentFrame> build_face_frames(const HalfEdgeMesh& mesh);

    static float compute_levi_civita_connection(
        const FaceTangentFrame& frame_i,
        const FaceTangentFrame& frame_j,
        const Vec3f& shared_edge_dir
    );

    static void detect_and_cancel_singularities(
        RemeshContext& ctx,
        const std::vector<FaceTangentFrame>& frames,
        const std::vector<std::complex<double>>& field
    );
};

} // namespace qremesh
