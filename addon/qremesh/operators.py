import bpy
import threading
import queue
import time
import os
import json
import numpy as np

from .bridge import invoke_remesh
from .extractors import extract_mesh_data
from .uv_transfer import transfer_uvs_from_source

class QREMESH_OT_remesh(bpy.types.Operator):
    bl_idname = "qremesh.remesh"
    bl_label = "Remesh"
    bl_description = "Execute automatic anti-spiral quad remeshing on the active object"
    bl_options = {'REGISTER', 'UNDO'}

    _timer = None
    _thread = None
    _queue = None
    _cancel_event = None
    _source_obj = None
    _progress_val = 0.0
    _stage_label = ""
    _start_time = 0.0

    @classmethod
    def poll(cls, context):
        return context.active_object and context.active_object.type == 'MESH' and context.mode == 'OBJECT'

    def modal(self, context, event):
        if event.type == 'ESC':
            if self._cancel_event:
                self._cancel_event.set()
            self.report({'WARNING'}, "QRemesh operation cancelled by user.")
            self.clean_up(context)
            return {'CANCELLED'}

        if event.type == 'TIMER':
            # Poll progress and results from worker thread
            while not self._queue.empty():
                msg_type, data = self._queue.get()
                if msg_type == "PROGRESS":
                    p, label = data
                    self._progress_val = p
                    self._stage_label = label
                    context.workspace.status_text_set(
                        f"QRemesh [{int(p * 100)}%]: {label} (Press ESC to cancel)"
                    )
                elif msg_type == "ERROR":
                    self.report({'ERROR'}, f"QRemesh failed: {data}")
                    self.clean_up(context)
                    return {'CANCELLED'}
                elif msg_type == "SUCCESS":
                    self.apply_result(context, data)
                    self.clean_up(context)
                    return {'FINISHED'}

        return {'PASS_THROUGH'}

    def execute(self, context):
        obj = context.active_object
        self._source_obj = obj
        props = obj.qremesh_props

        # Collect params dictionary
        params_dict = {
            "target_quad_count": props.target_quad_count,
            "target_edge_length": props.target_edge_length,
            "adaptive_density": props.adaptive_density,
            "max_surface_deviation": props.max_surface_deviation,
            "preserve_sharp_edges": props.preserve_sharp_edges,
            "sharp_angle_threshold": props.sharp_angle_threshold,
            "preserve_uv_seams": props.preserve_uv_seams,
            "preserve_material_borders": props.preserve_material_borders,
            "preserve_open_borders": props.preserve_open_borders,
            "symmetry_axis": int(props.symmetry_axis),
            "symmetry_tolerance": props.symmetry_tolerance,
            "enforce_closed_loops": props.enforce_closed_loops,
            "exact_quad_count": props.exact_quad_count,
            "allow_triangles": props.allow_triangles,
            "field_smooth_iters": props.field_smooth_iters,
            "vertex_relax_iters": props.vertex_relax_iters,
            "shape_preservation": props.shape_preservation,
            "proxy_tri_threshold": props.proxy_tri_threshold,
        }

        # Extract mesh buffers zero-copy
        try:
            mesh_in = extract_mesh_data(obj, props)
        except Exception as e:
            self.report({'ERROR'}, f"Failed to extract input mesh: {e}")
            return {'CANCELLED'}

        self._queue = queue.Queue()
        self._cancel_event = threading.Event()
        self._start_time = time.time()

        def worker():
            try:
                def progress_cb(p, stage):
                    if self._cancel_event.is_set():
                        return False # Cancel requested
                    self._queue.put(("PROGRESS", (p, stage)))
                    return True

                result = invoke_remesh(mesh_in, params_dict, progress_cb)
                self._queue.put(("SUCCESS", result))
            except Exception as e:
                self._queue.put(("ERROR", str(e)))

        self._thread = threading.Thread(target=worker, daemon=True)
        self._thread.start()

        wm = context.window_manager
        self._timer = wm.event_timer_add(0.05, window=context.window)
        wm.modal_handler_add(self)

        return {'RUNNING_MODAL'}

    def apply_result(self, context, result_dict):
        obj = self._source_obj
        props = obj.qremesh_props
        rep = result_dict["report"]

        # Update property report
        props.last_report_quads = rep["quad_count"]
        props.last_report_tris = rep["tri_count"]
        props.last_report_jacobian_med = rep["scaled_jacobian_median"]
        props.last_report_rms = rep["rms_deviation_rel"]
        props.last_report_time = rep["total_time_sec"]

        # Duplicate original object for UV transfer if needed
        temp_src = None
        if props.auto_transfer_uv and obj.data.uv_layers:
            temp_src = obj.copy()
            temp_src.data = obj.data.copy()
            context.collection.objects.link(temp_src)
            temp_src.hide_viewport = True
            temp_src.hide_render = True

        # Build new mesh
        new_mesh = bpy.data.meshes.new(name=obj.name + "_remeshed")
        positions = result_dict["positions"].tolist()
        polygons = result_dict["polygons"]

        new_mesh.from_pydata(positions, [], polygons)
        new_mesh.update()

        # Restore material slots
        for mat in obj.data.materials:
            new_mesh.materials.append(mat)

        # Assign material IDs
        if "material_ids" in result_dict and len(result_dict["material_ids"]) == len(new_mesh.polygons):
            new_mesh.polygons.foreach_set("material_index", result_dict["material_ids"].astype(np.int32))

        # Assign sharp, seam and crease flags via vectorized foreach_set (Blender 4.0 - 5.2 LTS)
        ne_out = len(new_mesh.edges)
        if ne_out > 0:
            out_edge_pairs = np.empty(ne_out * 2, dtype=np.uint32)
            new_mesh.edges.foreach_get("vertices", out_edge_pairs)
            u_arr = np.minimum(out_edge_pairs[0::2], out_edge_pairs[1::2])
            v_arr = np.maximum(out_edge_pairs[0::2], out_edge_pairs[1::2])
            edge_keys = (u_arr.astype(np.uint64) << 32) | v_arr.astype(np.uint64)

            if result_dict.get("sharp_edges") is not None and len(result_dict["sharp_edges"]) > 0:
                s_arr = result_dict["sharp_edges"]
                su = np.minimum(s_arr[:, 0], s_arr[:, 1]).astype(np.uint64)
                sv = np.maximum(s_arr[:, 0], s_arr[:, 1]).astype(np.uint64)
                s_keys_arr = (su << 32) | sv
                sharp_keys = set(s_keys_arr)
                sharp_mask = np.array([k in sharp_keys for k in edge_keys], dtype=bool)

                # Write sharp edges using generic attribute (Blender 4.x / 5.x standard)
                sharp_attr = new_mesh.attributes.get("sharp_edge")
                if not sharp_attr:
                    sharp_attr = new_mesh.attributes.new(name="sharp_edge", type='BOOLEAN', domain='EDGE')
                sharp_attr.data.foreach_set("value", sharp_mask)

                # Map per-sharp-edge crease values onto full mesh edge array (length ne_out)
                if result_dict.get("crease_values") is not None and len(result_dict["crease_values"]) == len(s_keys_arr):
                    crease_vals = result_dict["crease_values"]
                    if np.any(crease_vals > 1e-4):
                        crease_map = dict(zip(s_keys_arr.tolist(), crease_vals.tolist()))
                        full_creases = np.array([crease_map.get(int(k), 0.0) for k in edge_keys], dtype=np.float32)
                        crease_attr = new_mesh.attributes.get("crease_edge")
                        if not crease_attr:
                            crease_attr = new_mesh.attributes.new(name="crease_edge", type='FLOAT', domain='EDGE')
                        crease_attr.data.foreach_set("value", full_creases)

            if result_dict.get("seam_edges") is not None and len(result_dict["seam_edges"]) > 0:
                sm_arr = result_dict["seam_edges"]
                smu = np.minimum(sm_arr[:, 0], sm_arr[:, 1]).astype(np.uint64)
                smv = np.maximum(sm_arr[:, 0], sm_arr[:, 1]).astype(np.uint64)
                seam_keys = set((smu << 32) | smv)
                seam_mask = np.array([k in seam_keys for k in edge_keys], dtype=bool)

                # Blender 5.2's public built-in edge attribute is named "uv_seam".
                seam_attr = new_mesh.attributes.get("uv_seam")
                if seam_attr is None:
                    seam_attr = new_mesh.attributes.new(name="uv_seam", type='BOOLEAN', domain='EDGE')
                seam_attr.data.foreach_set("value", seam_mask)

        # Smooth shading via face attribute (Blender 4.2 - 5.2 LTS standard)
        np_polys = len(new_mesh.polygons)
        if np_polys > 0:
            smooth_attr = new_mesh.attributes.get("shade_smooth")
            if not smooth_attr:
                smooth_attr = new_mesh.attributes.new(name="shade_smooth", type='BOOLEAN', domain='FACE')
            smooth_attr.data.foreach_set("value", np.ones(np_polys, dtype=bool))

        # Swap mesh data on object
        old_mesh = obj.data
        obj.data = new_mesh
        bpy.data.meshes.remove(old_mesh)

        # Auto-transfer UV if requested
        if temp_src:
            transfer_uvs_from_source(temp_src, obj)
            bpy.data.objects.remove(temp_src, do_unlink=True)

        summary_msg = (
            f"QRemesh: {rep['quad_count']} Quads | "
            f"J_med: {rep['scaled_jacobian_median']:.2f} | "
            f"Dev: {rep['rms_deviation_rel']:.2f}% | "
            f"{rep['total_time_sec']:.2f}s"
        )
        self.report({'INFO'}, summary_msg)

    def clean_up(self, context):
        if self._timer:
            context.window_manager.event_timer_remove(self._timer)
            self._timer = None
        context.workspace.status_text_set(None)

class QREMESH_OT_paint_density(bpy.types.Operator):
    bl_idname = "qremesh.paint_density"
    bl_label = "Paint Quad Density"
    bl_description = "Switch to Vertex Paint mode to paint local quad density (Red = 4x, Green = 1x, Blue = 0.25x)"
    bl_options = {'REGISTER', 'UNDO'}

    @classmethod
    def poll(cls, context):
        return context.active_object and context.active_object.type == 'MESH'

    def execute(self, context):
        obj = context.active_object
        mesh = obj.data

        # Ensure active color attribute exists
        attr_name = "QRemesh_Density"
        if not mesh.color_attributes.get(attr_name):
            mesh.color_attributes.new(name=attr_name, type='FLOAT_COLOR', domain='POINT')
        mesh.color_attributes.active_color_name = attr_name

        bpy.ops.object.mode_set(mode='VERTEX_PAINT')
        self.report({'INFO'}, "Vertex Paint mode: Red = Dense, Green = Base, Blue = Sparse")
        return {'FINISHED'}

class QREMESH_OT_draw_guides(bpy.types.Operator):
    bl_idname = "qremesh.draw_guides"
    bl_label = "Draw Flow Guides"
    bl_description = "Activate Annotation brush on surface to guide quad edge flow"
    bl_options = {'REGISTER'}

    def execute(self, context):
        bpy.ops.wm.tool_set_by_id(name="builtin.annotate")
        context.scene.tool_settings.annotation_stroke_placement_view3d = 'SURFACE'
        self.report({'INFO'}, "Annotation tool activated with Surface placement.")
        return {'FINISHED'}

class QREMESH_OT_apply_preset(bpy.types.Operator):
    bl_idname = "qremesh.apply_preset"
    bl_label = "Apply Preset"
    bl_description = "Apply a built-in parameter preset"
    bl_options = {'REGISTER', 'UNDO'}

    preset_name: bpy.props.StringProperty()

    def execute(self, context):
        base_dir = os.path.dirname(os.path.abspath(__file__))
        path = os.path.join(base_dir, "presets", f"{self.preset_name}.json")
        if not os.path.exists(path):
            self.report({'ERROR'}, f"Preset {self.preset_name} not found.")
            return {'CANCELLED'}

        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)

        obj = context.active_object
        if not obj:
            return {'CANCELLED'}

        props = obj.qremesh_props
        for k, v in data.items():
            if hasattr(props, k):
                setattr(props, k, v)

        self.report({'INFO'}, f"Preset applied: {data.get('name', self.preset_name)}")
        return {'FINISHED'}
