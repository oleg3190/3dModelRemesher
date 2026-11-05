import bpy

class VIEW3D_PT_qremesh(bpy.types.Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'QRemesh'
    bl_label = "QRemesh 2.0"

    def draw(self, context):
        layout = self.layout
        obj = context.active_object

        if not obj or obj.type != 'MESH':
            layout.label(text="Select a Mesh Object", icon='INFO')
            return

        props = obj.qremesh_props

        # Presets row
        row = layout.row(align=True)
        op = row.operator("qremesh.apply_preset", text="Organic")
        op.preset_name = "Organic_Rigging"
        op = row.operator("qremesh.apply_preset", text="Hard Surface")
        op.preset_name = "HardSurface_CAD"
        op = row.operator("qremesh.apply_preset", text="Fast")
        op.preset_name = "Fast_Sculpt"

        layout.separator()

        # Main remesh button
        col = layout.column(align=True)
        col.scale_y = 1.6
        col.operator("qremesh.remesh", text="Remesh Object", icon='MOD_REMESH')

        # Target Geometry
        box = layout.box()
        box.label(text="Resolution & Density", icon='MESH_GRID')
        col = box.column(align=True)
        col.prop(props, "target_quad_count")
        if props.target_quad_count == 0:
            col.prop(props, "target_edge_length")
        col.prop(props, "adaptive_density", slider=True)
        col.prop(props, "exact_quad_count")
        col.prop(props, "allow_triangles")

        # Interactive density & guides
        row = box.row(align=True)
        row.operator("qremesh.paint_density", text="Paint Density", icon='BRUSH_DATA')
        row.operator("qremesh.draw_guides", text="Draw Guides", icon='GREASEPENCIL')

        # Feature Retention
        box = layout.box()
        box.label(text="Feature Retention", icon='EDGESEL')
        col = box.column(align=True)
        col.prop(props, "preserve_sharp_edges")
        if props.preserve_sharp_edges:
            col.prop(props, "sharp_angle_threshold")
        col.prop(props, "preserve_uv_seams")
        col.prop(props, "preserve_material_borders")
        col.prop(props, "preserve_open_borders")
        col.prop(props, "auto_transfer_uv")

        # Topology & Loops
        box = layout.box()
        box.label(text="Topology & Anti-Spiral", icon='FORCE_VORTEX')
        col = box.column(align=True)
        col.prop(props, "enforce_closed_loops")
        col.prop(props, "symmetry_axis")
        if props.symmetry_axis != '-1':
            col.prop(props, "symmetry_tolerance")
        col.prop(props, "field_smooth_iters")
        col.prop(props, "vertex_relax_iters")
        col.prop(props, "shape_preservation", slider=True)

        # Quality Report from last run
        if props.last_report_quads > 0:
            box = layout.box()
            box.label(text="Quality Report", icon='CHECKMARK')
            col = box.column(align=True)
            col.label(text=f"Quads: {props.last_report_quads:,} | Tris: {props.last_report_tris}")
            col.label(text=f"Median Jacobian: {props.last_report_jacobian_med:.2f}")
            col.label(text=f"RMS Deviation: {props.last_report_rms:.3f}%")
            col.label(text=f"Processing Time: {props.last_report_time:.2f} s")
