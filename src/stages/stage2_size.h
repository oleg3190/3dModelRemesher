#pragma once

#include "remesh_context.h"

namespace qremesh {

struct VertexCurvature {
    float k1 = 0.0f; // max principal curvature
    float k2 = 0.0f; // min principal curvature
    Vec3f dir1 = Vec3f::Zero(); // principal direction 1
    Vec3f dir2 = Vec3f::Zero(); // principal direction 2
};

class Stage2SizingField {
public:
    static QrmStatus execute(RemeshContext& ctx);

    static std::vector<VertexCurvature> compute_curvatures(const HalfEdgeMesh& mesh);

private:
    static void apply_lipschitz_limiter(
        const HalfEdgeMesh& mesh,
        std::vector<float>& sizing,
        float max_gradient
    );
};

} // namespace qremesh
