"""Blender 5.2 smoke test for the QRemesh add-on.

Run from the repository root:
    blender --background --factory-startup --python addon/qremesh/tests/blender_52_smoke.py

This checks add-on registration, public UV seam attribute IO, and Scene.annotation
guide-curve extraction. It does not invoke the native remeshing library.
"""
from pathlib import Path
import sys

import bpy

if bpy.app.version < (5, 2, 0):
    raise RuntimeError(
        f"Blender 5.2+ is required for this smoke test; got {bpy.app.version_string}"
    )

ADDON_PARENT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ADDON_PARENT))

import qremesh  # noqa: E402
from qremesh.extractors import extract_mesh_data  # noqa: E402


def main():
    scene = bpy.context.scene
    previous_annotation = getattr(scene, "annotation", None)
    mesh = None
    obj = None
    annotation = None
    registered = False

    try:
        qremesh.register()
        registered = True

        mesh = bpy.data.meshes.new("QRemesh_52_SmokeMesh")
        mesh.from_pydata(
            [(0.0, 0.0, 0.0), (1.0, 0.0, 0.0), (0.0, 1.0, 0.0)],
            [],
            [(0, 1, 2)],
        )
        mesh.update()
        assert len(mesh.edges) == 3, "Smoke mesh must have three edges"

        # MeshEdge.use_seam is exposed by Blender 5.2 and maps to uv_seam.
        mesh.edges[0].use_seam = True
        obj = bpy.data.objects.new("QRemesh_52_SmokeObject", mesh)
        scene.collection.objects.link(obj)

        annotation = bpy.data.annotations.new("QRemesh_52_SmokeAnnotations")
        scene.annotation = annotation
        layer = annotation.layers.new("QRemesh Smoke Guides")
        frame = layer.frames.new(scene.frame_current)
        stroke = frame.strokes.new()
        stroke.points.add(count=2)
        stroke.points[0].co = (0.0, 0.0, 0.0)
        stroke.points[1].co = (1.0, 0.0, 0.0)

        extracted = extract_mesh_data(obj, obj.qremesh_props)
        assert extracted["seam_edge_pairs"] is not None, (
            "The public uv_seam/use_seam edge flag was not extracted"
        )
        assert len(extracted["user_curves"]) >= 1, (
            "Scene.annotation strokes were not extracted as guide curves"
        )

        print(
            "[QRemesh] Blender 5.2 smoke test passed: "
            "registration, UV seam extraction, Annotation guide extraction"
        )
    finally:
        if obj is not None:
            bpy.data.objects.remove(obj, do_unlink=True)
        if mesh is not None and mesh.users == 0:
            bpy.data.meshes.remove(mesh)
        if annotation is not None:
            scene.annotation = previous_annotation
            bpy.data.annotations.remove(annotation)
        if registered:
            qremesh.unregister()


if __name__ == "__main__":
    main()
