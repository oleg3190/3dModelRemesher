import bpy

def transfer_uvs_from_source(source_obj, target_obj):
    """
    Transfers UV maps from source_obj to target_obj using DATA_TRANSFER modifier
    (Nearest Face Interpolated).
    """
    if not source_obj or not target_obj:
        return False

    src_mesh = source_obj.data
    if not src_mesh.uv_layers:
        return False # No UV to transfer

    ctx = bpy.context
    original_active = ctx.view_layer.objects.active
    ctx.view_layer.objects.active = target_obj

    # Create UV layer if target doesn't have one
    if not target_obj.data.uv_layers:
        target_obj.data.uv_layers.new(name="UVMap")

    # Add Data Transfer modifier
    mod = target_obj.modifiers.new(name="QRemesh_UV_Transfer", type='DATA_TRANSFER')
    mod.object = source_obj
    mod.use_loop_data = True
    mod.data_types_loops = {'UV'}
    mod.loop_mapping = 'NEAREST_POLYNOR' # Nearest face interpolated

    # Generate data layout
    try:
        bpy.ops.object.datalayout_transfer(modifier=mod.name)
        bpy.ops.object.modifier_apply(modifier=mod.name)
        success = True
    except Exception as e:
        print(f"[QRemesh] UV transfer warning: {e}")
        target_obj.modifiers.remove(mod)
        success = False

    ctx.view_layer.objects.active = original_active
    return success
