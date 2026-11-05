import ctypes
import os
import sys
import platform
import numpy as np

# Load native library
_lib_instance = None

def get_lib_path():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    system = platform.system()
    machine = platform.machine().lower()

    if system == "Windows":
        lib_name = "qremesh_core.dll"
    elif system == "Darwin":
        lib_name = "libqremesh_core.dylib"
    else:
        lib_name = "libqremesh_core.so"

    search_paths = [
        os.path.join(base_dir, "wheels", lib_name),
        os.path.join(base_dir, "..", "..", "build", lib_name),
        os.path.join(base_dir, lib_name),
    ]

    for p in search_paths:
        if os.path.exists(p):
            return os.path.abspath(p)
    return lib_name

def get_library():
    global _lib_instance
    if _lib_instance is not None:
        return _lib_instance

    lib_path = get_lib_path()
    try:
        _lib_instance = ctypes.CDLL(lib_path)
    except Exception as e:
        print(f"[QRemesh] Warning: Could not load native library from {lib_path}: {e}")
        return None

    # Setup C ABI signatures
    _setup_abi(_lib_instance)
    return _lib_instance

# C Structs matching qremesh_c_api.h

class CQrmStatus:
    QRM_OK = 0
    QRM_ERR_INVALID_INPUT = 1
    QRM_ERR_TOPOLOGY_UNFIXABLE = 2
    QRM_ERR_OUT_OF_MEMORY = 3
    QRM_ERR_CANCELLED = 4
    QRM_ERR_FIELD_DIVERGED = 5
    QRM_ERR_PARAM_FAILED = 6
    QRM_ERR_INTERNAL = 99

class CQrmCurve(ctypes.Structure):
    _fields_ = [
        ("points", ctypes.POINTER(ctypes.c_float)),
        ("point_count", ctypes.c_uint32),
        ("role", ctypes.c_int),
        ("weight", ctypes.c_float),
    ]

class CQrmMeshIn(ctypes.Structure):
    _fields_ = [
        ("positions", ctypes.POINTER(ctypes.c_float)),
        ("tri_indices", ctypes.POINTER(ctypes.c_uint32)),
        ("sharp_edge_pairs", ctypes.POINTER(ctypes.c_uint32)),
        ("sharp_edge_creases", ctypes.POINTER(ctypes.c_float)),
        ("seam_edge_pairs", ctypes.POINTER(ctypes.c_uint32)),
        ("vertex_density_weights", ctypes.POINTER(ctypes.c_float)),
        ("tri_material_ids", ctypes.POINTER(ctypes.c_uint16)),
        ("user_curves", ctypes.POINTER(CQrmCurve)),
        ("nv", ctypes.c_uint32),
        ("nt", ctypes.c_uint32),
        ("num_sharp_edges", ctypes.c_uint32),
        ("num_seam_edges", ctypes.c_uint32),
        ("num_curves", ctypes.c_uint32),
    ]

class CQrmParams(ctypes.Structure):
    _fields_ = [
        ("target_quad_count", ctypes.c_int32),
        ("target_edge_length", ctypes.c_float),
        ("adaptive_density", ctypes.c_float),
        ("max_surface_deviation", ctypes.c_float),
        ("preserve_sharp_edges", ctypes.c_bool),
        ("sharp_angle_threshold", ctypes.c_float),
        ("preserve_uv_seams", ctypes.c_bool),
        ("preserve_material_borders", ctypes.c_bool),
        ("preserve_open_borders", ctypes.c_bool),
        ("symmetry_axis", ctypes.c_int8),
        ("symmetry_tolerance", ctypes.c_float),
        ("enforce_closed_loops", ctypes.c_bool),
        ("exact_quad_count", ctypes.c_bool),
        ("allow_triangles", ctypes.c_bool),
        ("field_smooth_iters", ctypes.c_int32),
        ("vertex_relax_iters", ctypes.c_int32),
        ("shape_preservation", ctypes.c_float),
        ("proxy_tri_threshold", ctypes.c_uint32),
        ("num_threads", ctypes.c_uint32),
        ("random_seed", ctypes.c_uint32),
        ("debug_dump_dir", ctypes.c_char_p),
    ]

class CQrmQualityReport(ctypes.Structure):
    _fields_ = [
        ("quad_count", ctypes.c_uint32),
        ("tri_count", ctypes.c_uint32),
        ("ngon_count", ctypes.c_uint32),
        ("singularity_count", ctypes.c_uint32),
        ("hausdorff_max_rel", ctypes.c_float),
        ("rms_deviation_rel", ctypes.c_float),
        ("scaled_jacobian_min", ctypes.c_float),
        ("scaled_jacobian_median", ctypes.c_float),
        ("max_warp_angle_deg", ctypes.c_float),
        ("total_time_sec", ctypes.c_float),
    ]

class CQrmMeshOut(ctypes.Structure):
    _fields_ = [
        ("positions", ctypes.POINTER(ctypes.c_float)),
        ("poly_indices", ctypes.POINTER(ctypes.c_uint32)),
        ("poly_offsets", ctypes.POINTER(ctypes.c_uint32)),
        ("sharp_edge_out", ctypes.POINTER(ctypes.c_uint32)),
        ("crease_edge_out", ctypes.POINTER(ctypes.c_float)),
        ("seam_edge_out", ctypes.POINTER(ctypes.c_uint32)),
        ("poly_material_ids", ctypes.POINTER(ctypes.c_uint16)),
        ("nv_out", ctypes.c_uint32),
        ("np_out", ctypes.c_uint32),
        ("total_corners", ctypes.c_uint32),
        ("num_sharp_out", ctypes.c_uint32),
        ("num_seam_out", ctypes.c_uint32),
        ("report", CQrmQualityReport),
    ]

PROGRESS_CB_TYPE = ctypes.CFUNCTYPE(ctypes.c_bool, ctypes.c_float, ctypes.c_char_p, ctypes.c_void_p)

def _setup_abi(lib):
    lib.qrm_default_params.argtypes = [ctypes.POINTER(CQrmParams)]
    lib.qrm_default_params.restype = None

    lib.qrm_remesh.argtypes = [
        ctypes.POINTER(CQrmMeshIn),
        ctypes.POINTER(CQrmParams),
        PROGRESS_CB_TYPE,
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.POINTER(CQrmMeshOut)),
    ]
    lib.qrm_remesh.restype = ctypes.c_int

    lib.qrm_free_mesh_out.argtypes = [ctypes.POINTER(CQrmMeshOut)]
    lib.qrm_free_mesh_out.restype = None

def invoke_remesh(mesh_in_dict, params_dict, progress_callback=None):
    lib = get_library()
    if lib is None:
        raise RuntimeError("qremesh native library is not available.")

    # Populate parameters
    params = CQrmParams()
    lib.qrm_default_params(ctypes.byref(params))

    _encoded_strings = []
    for k, v in params_dict.items():
        if hasattr(params, k):
            if k == "debug_dump_dir" and v is not None:
                enc = v.encode("utf-8")
                _encoded_strings.append(enc)
                setattr(params, k, enc)
            else:
                setattr(params, k, v)

    # Prepare inputs using numpy pointer extraction (zero-copy)
    pos_np = np.ascontiguousarray(mesh_in_dict["positions"], dtype=np.float32)
    tris_np = np.ascontiguousarray(mesh_in_dict["tri_indices"], dtype=np.uint32)

    c_mesh_in = CQrmMeshIn()
    c_mesh_in.positions = pos_np.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
    c_mesh_in.tri_indices = tris_np.ctypes.data_as(ctypes.POINTER(ctypes.c_uint32))
    c_mesh_in.nv = len(pos_np) // 3
    c_mesh_in.nt = len(tris_np) // 3

    # Optional sharp edges
    sharp_np = None
    crease_np = None
    if "sharp_edge_pairs" in mesh_in_dict and mesh_in_dict["sharp_edge_pairs"] is not None:
        sharp_np = np.ascontiguousarray(mesh_in_dict["sharp_edge_pairs"], dtype=np.uint32)
        c_mesh_in.sharp_edge_pairs = sharp_np.ctypes.data_as(ctypes.POINTER(ctypes.c_uint32))
        c_mesh_in.num_sharp_edges = len(sharp_np) // 2

        if "sharp_edge_creases" in mesh_in_dict and mesh_in_dict["sharp_edge_creases"] is not None:
            crease_np = np.ascontiguousarray(mesh_in_dict["sharp_edge_creases"], dtype=np.float32)
            c_mesh_in.sharp_edge_creases = crease_np.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

    # Optional seam edges
    seam_np = None
    if "seam_edge_pairs" in mesh_in_dict and mesh_in_dict["seam_edge_pairs"] is not None:
        seam_np = np.ascontiguousarray(mesh_in_dict["seam_edge_pairs"], dtype=np.uint32)
        c_mesh_in.seam_edge_pairs = seam_np.ctypes.data_as(ctypes.POINTER(ctypes.c_uint32))
        c_mesh_in.num_seam_edges = len(seam_np) // 2

    # Optional density weights
    density_np = None
    if "vertex_density_weights" in mesh_in_dict and mesh_in_dict["vertex_density_weights"] is not None:
        density_np = np.ascontiguousarray(mesh_in_dict["vertex_density_weights"], dtype=np.float32)
        c_mesh_in.vertex_density_weights = density_np.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

    # Optional material ids
    mat_np = None
    if "tri_material_ids" in mesh_in_dict and mesh_in_dict["tri_material_ids"] is not None:
        mat_np = np.ascontiguousarray(mesh_in_dict["tri_material_ids"], dtype=np.uint16)
        c_mesh_in.tri_material_ids = mat_np.ctypes.data_as(ctypes.POINTER(ctypes.c_uint16))

    # Optional user curves
    curve_structs = []
    curve_points_np = []
    if "user_curves" in mesh_in_dict and mesh_in_dict["user_curves"]:
        curves_list = mesh_in_dict["user_curves"]
        for c in curves_list:
            pts = np.ascontiguousarray(c["points"], dtype=np.float32)
            curve_points_np.append(pts)
            st = CQrmCurve()
            st.points = pts.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
            st.point_count = len(pts) // 3
            st.role = c.get("role", 3)
            st.weight = c.get("weight", 1.0)
            curve_structs.append(st)

        curves_array = (CQrmCurve * len(curve_structs))(*curve_structs)
        c_mesh_in.user_curves = curves_array
        c_mesh_in.num_curves = len(curve_structs)

    # Callback wrapper
    def cb_wrapper(p, label, _):
        if progress_callback:
            return bool(progress_callback(p, label.decode("utf-8") if label else ""))
        return True

    c_cb = PROGRESS_CB_TYPE(cb_wrapper)

    out_ptr = ctypes.POINTER(CQrmMeshOut)()
    status = lib.qrm_remesh(
        ctypes.byref(c_mesh_in),
        ctypes.byref(params),
        c_cb,
        None,
        ctypes.byref(out_ptr)
    )

    if status != CQrmStatus.QRM_OK or not out_ptr:
        raise RuntimeError(f"Remesh failed with status code: {status}")

    out = out_ptr.contents

    # Extract output buffers back to Python / numpy
    nv = out.nv_out
    np_count = out.np_out
    tot_corners = out.total_corners

    out_positions = np.ctypeslib.as_array(out.positions, shape=(nv, 3)).copy()
    out_offsets = np.ctypeslib.as_array(out.poly_offsets, shape=(np_count + 1,)).copy()
    out_indices = np.ctypeslib.as_array(out.poly_indices, shape=(tot_corners,)).copy()
    out_mats = np.ctypeslib.as_array(out.poly_material_ids, shape=(np_count,)).copy()

    out_sharp = None
    out_crease = None
    if out.num_sharp_out > 0:
        out_sharp = np.ctypeslib.as_array(out.sharp_edge_out, shape=(out.num_sharp_out, 2)).copy()
        out_crease = np.ctypeslib.as_array(out.crease_edge_out, shape=(out.num_sharp_out,)).copy()

    out_seams = None
    if out.num_seam_out > 0:
        out_seams = np.ctypeslib.as_array(out.seam_edge_out, shape=(out.num_seam_out, 2)).copy()

    report = {
        "quad_count": out.report.quad_count,
        "tri_count": out.report.tri_count,
        "ngon_count": out.report.ngon_count,
        "singularity_count": out.report.singularity_count,
        "hausdorff_max_rel": out.report.hausdorff_max_rel,
        "rms_deviation_rel": out.report.rms_deviation_rel,
        "scaled_jacobian_min": out.report.scaled_jacobian_min,
        "scaled_jacobian_median": out.report.scaled_jacobian_median,
        "max_warp_angle_deg": out.report.max_warp_angle_deg,
        "total_time_sec": out.report.total_time_sec,
    }

    lib.qrm_free_mesh_out(out_ptr)

    # Format polygons list of index arrays
    polygons = []
    for i in range(np_count):
        start = out_offsets[i]
        end = out_offsets[i + 1]
        polygons.append(out_indices[start:end].tolist())

    return {
        "positions": out_positions,
        "polygons": polygons,
        "material_ids": out_mats,
        "sharp_edges": out_sharp,
        "crease_values": out_crease,
        "seam_edges": out_seams,
        "report": report,
    }
