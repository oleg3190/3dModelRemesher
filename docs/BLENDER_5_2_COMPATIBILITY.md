# Blender 5.2 LTS compatibility

The add-on is intended to remain compatible with Blender 4.2 LTS and newer, including Blender 5.2 LTS.

## Blender 5.2 API adjustments

- Scene annotations are accessed through `Scene.annotation`. The add-on now probes this public property first and keeps `annotation_data` only as a guarded fallback.
- UV seams are read and written through Blender's public `uv_seam` edge attribute. Reading also supports `MeshEdge.use_seam`, which is present in the Blender 5.2 Python API.
- The extension manifest declares the file-system permission with a reason and uses the SPDX-prefixed GPL license identifier required by the extension manifest format.
- The add-on package version is now 2.0.1.

## Smoke test in Blender 5.2

From the repository root, run:

```bash
blender --background --factory-startup --python addon/qremesh/tests/blender_52_smoke.py
```

The test checks add-on registration, edge UV seam extraction and classic Annotation guide extraction. It intentionally does not invoke the native QRemesh library; native binary loading and a full remesh should also be tested on the target OS.

## API references

- [Blender 5.2 Scene API](https://docs.blender.org/api/5.2/bpy.types.Scene.html) — `Scene.annotation`.
- [Blender 5.2 MeshEdge API](https://docs.blender.org/api/5.2/bpy.types.MeshEdge.html) — `MeshEdge.use_seam`.
- [Blender 5.2 extensions manifest](https://docs.blender.org/manual/en/5.2/advanced/extensions/getting_started.html) — manifest and permission fields.
