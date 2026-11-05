import bpy
import gpu
from gpu_extras.batch import batch_for_shader

_draw_handler = None

def draw_callback_px():
    # Overlay preview for user guide curves (Annotation strokes)
    anim_data = getattr(bpy.context.scene, "annotation_data", None)
    if not anim_data or not anim_data.layers:
        return

    try:
        try:
            shader = gpu.shader.from_builtin('UNIFORM_COLOR')
        except ValueError:
            shader = gpu.shader.from_builtin('3D_UNIFORM_COLOR')
    except Exception:
        return

    gpu.state.depth_test_set('LESS_EQUAL')
    gpu.state.line_width_set(2.5)

    shader.bind()
    shader.uniform_float("color", (0.0, 0.85, 1.0, 0.9)) # Cyan guide lines

    for layer in anim_data.layers:
        if not layer.active_frame:
            continue
        for stroke in layer.active_frame.strokes:
            coords = [p.co for p in stroke.points]
            if len(coords) >= 2:
                batch = batch_for_shader(shader, 'LINE_STRIP', {"pos": coords})
                batch.draw(shader)

def register_overlay():
    global _draw_handler
    if _draw_handler is None:
        try:
            _draw_handler = bpy.types.SpaceView3D.draw_handler_add(
                draw_callback_px, (), 'WINDOW', 'POST_VIEW'
            )
        except Exception:
            _draw_handler = None

def unregister_overlay():
    global _draw_handler
    if _draw_handler is not None:
        try:
            bpy.types.SpaceView3D.draw_handler_remove(_draw_handler, 'WINDOW')
        except Exception:
            pass
        _draw_handler = None
