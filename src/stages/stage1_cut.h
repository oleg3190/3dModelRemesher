#pragma once

#include "remesh_context.h"

namespace qremesh {

class Stage1FeatureCut {
public:
    static QrmStatus execute(RemeshContext& ctx);

private:
    static void detect_sharp_dihedral_edges(HalfEdgeMesh& mesh, float threshold_deg);
    static void mark_material_boundaries(HalfEdgeMesh& mesh);
    static void project_and_embed_curves(HalfEdgeMesh& mesh, const RemeshContext& ctx);
    static void filter_micro_features(HalfEdgeMesh& mesh, float min_feature_len);
};

} // namespace qremesh
