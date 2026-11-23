import bpy
from bpy.props import (
    IntProperty,
    FloatProperty,
    BoolProperty,
    EnumProperty,
    StringProperty,
    PointerProperty,
)

class QRemeshProperties(bpy.types.PropertyGroup):
    target_quad_count: IntProperty(
        name="Target Quads",
        description="Target number of quad polygons (0 = auto via edge length)",
        default=5000,
        min=0,
        max=10000000,
    )

    target_edge_length: FloatProperty(
        name="Edge Length",
        description="Target edge length in scene units (used if target quads == 0)",
        default=0.0,
        min=0.0,
    )

    adaptive_density: FloatProperty(
        name="Adaptive Density",
        description="Adapt quad sizes to local surface curvature (0.0 = uniform, 1.0 = highly adaptive)",
        default=0.5,
        min=0.0,
        max=1.0,
    )

    max_surface_deviation: FloatProperty(
        name="Max Deviation (%)",
        description="Maximum allowed surface deviation from original mesh (% of bounding box diagonal)",
        default=0.20,
        min=0.001,
        max=10.0,
    )

    preserve_sharp_edges: BoolProperty(
        name="Preserve Sharp Edges",
        description="Detect and retain hard geometric features as quad edges",
        default=True,
    )

    sharp_angle_threshold: FloatProperty(
        name="Sharp Angle",
        description="Dihedral angle threshold for automatic sharp edge detection (in degrees)",
        default=45.0,
        min=0.0,
        max=180.0,
    )

    preserve_uv_seams: BoolProperty(
        name="Preserve UV Seams",
        description="Enforce quad edges along existing UV seam lines",
        default=True,
    )

    preserve_material_borders: BoolProperty(
        name="Preserve Material Borders",
        description="Keep edges aligned with boundaries between material assignments",
        default=True,
    )

    preserve_open_borders: BoolProperty(
        name="Preserve Open Borders",
        description="Accurately preserve open mesh boundary edges without hole filling",
        default=True,
    )

    symmetry_axis: EnumProperty(
        name="Symmetry",
        description="Mirror remeshed topology along global symmetry axis",
        items=[
            ('-1', "None", "Disable symmetry"),
            ('0', "X Axis", "Symmetry across X = 0"),
            ('1', "Y Axis", "Symmetry across Y = 0"),
            ('2', "Z Axis", "Symmetry across Z = 0"),
            ('3', "Auto-Detect", "Automatically detect symmetry axis (X, Y, or Z)"),
        ],
        default='-1',
    )

    symmetry_tolerance: FloatProperty(
        name="Symmetry Tolerance",
        description="Tolerance for welding vertices on symmetry plane (% of bbox)",
        default=0.05,
        min=0.0001,
        max=5.0,
    )

    enforce_closed_loops: BoolProperty(
        name="Anti-Spiral Loops",
        description="Prevent spiral edge loops on cylindrical sections (arms, fingers, pipes)",
        default=True,
    )

    exact_quad_count: BoolProperty(
        name="Exact Quad Count",
        description="Strictly match target quad budget within ±1%",
        default=False,
    )

    allow_triangles: BoolProperty(
        name="Allow Triangles",
        description="Allow rare transition triangles in high density gradient regions",
        default=True,
    )

    auto_transfer_uv: BoolProperty(
        name="Auto-Transfer UV",
        description="Automatically project and interpolate UV maps from source mesh to remeshed mesh",
        default=True,
    )

    field_smooth_iters: IntProperty(
        name="Field Smooth Iterations",
        description="Number of iterations for 4-RoSy cross-field smoothing",
        default=50,
        min=0,
        max=500,
    )

    vertex_relax_iters: IntProperty(
        name="Relax Iterations",
        description="Conservative tangential relaxation iterations (lower values preserve small details)",
        default=6,
        min=0,
        max=100,
    )

    shape_preservation: FloatProperty(
        name="Shape Preservation",
        description="Weight of surface projection onto original high-poly mesh (0.0 .. 1.0)",
        default=0.90,
        min=0.0,
        max=1.0,
    )

    proxy_tri_threshold: IntProperty(
        name="Proxy Threshold",
        description="Triangle count threshold to activate fast QEM proxy decimation",
        default=300000,
        min=10000,
    )

    # Status / Report display fields
    last_report_quads: IntProperty(name="Quads", default=0)
    last_report_tris: IntProperty(name="Tris", default=0)
    last_report_jacobian_med: FloatProperty(name="Median Jacobian", default=0.0)
    last_report_rms: FloatProperty(name="RMS Deviation", default=0.0)
    last_report_time: FloatProperty(name="Time (s)", default=0.0)
