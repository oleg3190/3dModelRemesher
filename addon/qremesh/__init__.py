bl_info = {
    "name": "QRemesh - Quad Remesher",
    "author": "QRemesh Team",
    "version": (2, 0, 1),
    "blender": (4, 2, 0),
    "location": "View3D > Sidebar > QRemesh",
    "description": "QRemesh for Blender 4.2 LTS through Blender 5.2 LTS",
    "category": "Mesh",
}

import bpy
from .props import QRemeshProperties
from .operators import (
    QREMESH_OT_remesh,
    QREMESH_OT_paint_density,
    QREMESH_OT_draw_guides,
    QREMESH_OT_apply_preset,
)
from .ui_panel import VIEW3D_PT_qremesh
from .gpu_overlay import register_overlay, unregister_overlay

classes = (
    QRemeshProperties,
    QREMESH_OT_remesh,
    QREMESH_OT_paint_density,
    QREMESH_OT_draw_guides,
    QREMESH_OT_apply_preset,
    VIEW3D_PT_qremesh,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    bpy.types.Object.qremesh_props = bpy.props.PointerProperty(type=QRemeshProperties)
    register_overlay()

def unregister():
    unregister_overlay()
    del bpy.types.Object.qremesh_props

    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()
