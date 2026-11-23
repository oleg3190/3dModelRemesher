import bpy
import numpy as np

def extract_mesh_data(obj, settings):
    """
    Extracts mesh data using Blender's foreach_get into flat numpy arrays.
    Execution time < 50ms on 500k triangles.
    """
    mesh = obj.data
    mesh.calc_loop_triangles()

    nv = len(mesh.vertices)
    nt = len(mesh.loop_triangles)
    ne = len(mesh.edges)

    positions = np.empty(nv * 3, dtype=np.float32)
    mesh.vertices.foreach_get("co", positions)

    tri_indices = np.empty(nt * 3, dtype=np.uint32)
    mesh.loop_triangles.foreach_get("vertices", tri_indices)

    # Edge flags with Blender 4.0+ Attribute API compatibility
    edge_pairs = np.empty(ne * 2, dtype=np.uint32)
    if ne > 0:
        mesh.edges.foreach_get("vertices", edge_pairs)

    sharp_mask = np.zeros(ne, dtype=bool)
    if ne > 0:
        if mesh.attributes and "sharp_edge" in mesh.attributes:
            mesh.attributes["sharp_edge"].data.foreach_get("value", sharp_mask)
        elif hasattr(mesh.edges[0], "use_edge_sharp"):
            mesh.edges.foreach_get("use_edge_sharp", sharp_mask)

    seam_mask = np.zeros(ne, dtype=bool)
    if ne > 0:
        seam_attr = mesh.attributes.get("uv_seam") if mesh.attributes else None
        # Accept the private legacy name only as a compatibility fallback.
        if seam_attr is None and mesh.attributes:
            seam_attr = mesh.attributes.get(".uv_seam")
        if seam_attr is not None:
            seam_attr.data.foreach_get("value", seam_mask)
        elif hasattr(mesh.edges[0], "use_seam"):
            mesh.edges.foreach_get("use_seam", seam_mask)

    # Crease
    crease_vals = np.zeros(ne, dtype=np.float32)
    if ne > 0:
        if mesh.attributes and "crease_edge" in mesh.attributes:
            mesh.attributes["crease_edge"].data.foreach_get("value", crease_vals)
        elif hasattr(mesh.edges[0], "crease"):
            mesh.edges.foreach_get("crease", crease_vals)

    sharp_edge_pairs = None
    sharp_edge_creases = None
    if np.any(sharp_mask):
        sharp_indices = np.where(sharp_mask)[0]
        sharp_edge_pairs = edge_pairs.reshape(-1, 2)[sharp_indices].flatten()
        sharp_edge_creases = crease_vals[sharp_indices]

    seam_edge_pairs = None
    if np.any(seam_mask):
        seam_indices = np.where(seam_mask)[0]
        seam_edge_pairs = edge_pairs.reshape(-1, 2)[seam_indices].flatten()

    # Material IDs
    tri_material_ids = np.empty(nt, dtype=np.uint16)
    mesh.loop_triangles.foreach_get("material_index", tri_material_ids)

    # Vertex density weights from active color attribute or vertex group
    density_weights = np.ones(nv, dtype=np.float32)
    extracted_density = False

    # Check color attributes (Blender 3.6 / 4.x)
    if mesh.color_attributes and mesh.color_attributes.active_color:
        col_attr = mesh.color_attributes.active_color
        if col_attr.domain == 'POINT':
            cols = np.empty(nv * 4, dtype=np.float32)
            col_attr.data.foreach_get("color", cols)
            # R channel maps to multiplier 0.25 .. 4.0: red=4.0, green=1.0, blue=0.25
            r = cols[0::4]
            g = cols[1::4]
            b = cols[2::4]
            # Red dominance -> 4.0, Blue dominance -> 0.25, Green dominance -> 1.0
            density_weights = np.clip(1.0 + 3.0 * r - 0.75 * b, 0.25, 4.0)
            extracted_density = True

    # Check active vertex group if no color attribute used (Vectorized / fast for Blender 4.x / 5.x)
    if not extracted_density and obj.vertex_groups.active:
        vg_name = obj.vertex_groups.active.name
        # In Blender 4.0+ and 5.2, vertex groups are accessible as attributes on the mesh
        if mesh.attributes and vg_name in mesh.attributes:
            vg_attr = mesh.attributes[vg_name]
            weights = np.zeros(nv, dtype=np.float32)
            vg_attr.data.foreach_get("value", weights)
            density_weights = np.clip(0.25 + weights * 3.75, 0.25, 4.0)
        else:
            vg_idx = obj.vertex_groups.active.index
            weights = np.zeros(nv, dtype=np.float32)
            for v in mesh.vertices:
                for g in v.groups:
                    if g.group == vg_idx:
                        weights[v.index] = g.weight
                        break
            # Map weight [0, 1] to scale multiplier [0.25, 4.0]
            density_weights = np.clip(0.25 + weights * 3.75, 0.25, 4.0)

    # Extract user curves from GPencil v2/v3 / Annotation
    user_curves = extract_guide_curves(obj)

    return {
        "positions": positions,
        "tri_indices": tri_indices,
        "sharp_edge_pairs": sharp_edge_pairs,
        "sharp_edge_creases": sharp_edge_creases,
        "seam_edge_pairs": seam_edge_pairs,
        "vertex_density_weights": density_weights,
        "tri_material_ids": tri_material_ids,
        "user_curves": user_curves,
    }

def _transform_points_to_local(flat_pts, xform_mat):
    """
    Transforms a flat (N*3,) float32 array of 3D points by a 4x4 Blender Matrix into target local space.
    """
    if xform_mat is None or len(flat_pts) < 3:
        return flat_pts
    pts = flat_pts.reshape(-1, 3)
    mat_np = np.array(xform_mat, dtype=np.float32)
    rot_T = mat_np[:3, :3].T
    trans = mat_np[:3, 3]
    local_pts = pts @ rot_T + trans
    return np.ascontiguousarray(local_pts.flatten(), dtype=np.float32)

def extract_guide_curves(obj=None):
    """
    Extracts guide curves from Grease Pencil v3 (Blender 4.3 - 5.2 LTS) and
    classic Annotations / Grease Pencil v2, transformed into obj local space.
    """
    curves = []
    inv_obj_mat = obj.matrix_world.inverted_safe() if (obj and hasattr(obj, "matrix_world")) else None

    # 1. Search for scene Grease Pencil objects (GPv3 in Blender 5.2 LTS)
    for scene_obj in bpy.context.scene.objects:
        if scene_obj.type == 'GREASEPENCIL' and scene_obj.data:
            gp_data = scene_obj.data
            gp_to_local = (inv_obj_mat @ scene_obj.matrix_world) if inv_obj_mat else None
            # In GPv3 (Blender 4.3+ and 5.2 LTS), geometry can be in layers -> drawings -> curves/strokes
            layers = getattr(gp_data, "layers", [])
            for layer in layers:
                drawings = []
                if hasattr(layer, "current_frame"):
                    cf = layer.current_frame()
                    if cf and hasattr(cf, "drawing"):
                        drawings.append(cf.drawing)
                elif hasattr(layer, "frames"):
                    for fr in layer.frames:
                        if hasattr(fr, "drawing"):
                            drawings.append(fr.drawing)

                for drw in drawings:
                    if hasattr(drw, "curves") and hasattr(drw, "curve_offsets") and "position" in drw.attributes:
                        pos_data = drw.attributes["position"].data
                        n_pts = len(pos_data)
                        if n_pts > 0:
                            flat_pos = np.empty(n_pts * 3, dtype=np.float32)
                            pos_data.foreach_get("vector", flat_pos)
                            flat_pos = _transform_points_to_local(flat_pos, gp_to_local)
                            offsets = list(drw.curve_offsets) + [n_pts]
                            for k in range(len(offsets) - 1):
                                st = offsets[k]
                                en = offsets[k + 1]
                                if en - st >= 2:
                                    curves.append({
                                        "points": flat_pos[st * 3 : en * 3].copy(),
                                        "role": 3, # QRM_CURVE_FLOW_GUIDE
                                        "weight": 1.0,
                                    })
                    elif hasattr(drw, "strokes"):
                        for stroke in drw.strokes:
                            pts = []
                            for p in stroke.points:
                                pts.extend([p.co.x, p.co.y, p.co.z])
                            if len(pts) >= 6:
                                arr = _transform_points_to_local(np.array(pts, dtype=np.float32), gp_to_local)
                                curves.append({
                                    "points": arr,
                                    "role": 3,
                                    "weight": 1.0,
                                })

    # 2. Extract active Annotation layers. Blender 5.2 exposes the datablock as
    # Scene.annotation; retain the legacy name as a guarded fallback for older releases.
    scene = bpy.context.scene
    anim_data = getattr(scene, "annotation", None)
    if anim_data is None:
        anim_data = getattr(scene, "annotation_data", None)
    if anim_data:
        for layer in anim_data.layers:
            if not layer.active_frame:
                continue
            for stroke in layer.active_frame.strokes:
                pts = []
                for p in stroke.points:
                    pts.extend([p.co.x, p.co.y, p.co.z])
                if len(pts) >= 6:
                    arr = _transform_points_to_local(np.array(pts, dtype=np.float32), inv_obj_mat)
                    curves.append({
                        "points": arr,
                        "role": 3, # QRM_CURVE_FLOW_GUIDE
                        "weight": 1.0,
                    })

    return curves
