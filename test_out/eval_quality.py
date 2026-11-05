#!/usr/bin/env python3
"""Independent quality evaluation of remeshed outputs vs the source mesh."""
import sys, json, math
import numpy as np
import trimesh

SRC = sys.argv[1] if len(sys.argv) > 1 else "test_out/input.obj"
OUTS = sys.argv[2:] or ["test_out/remesh_1000.obj", "test_out/remesh_3000.obj",
                        "test_out/remesh_5000.obj", "test_out/remesh_20000.obj"]
SAMPLES = 60000


def load(p):
    """Load OBJ keeping polygon faces (trimesh triangulates quads)."""
    verts, faces = [], []
    with open(p) as fh:
        for line in fh:
            if line.startswith("v "):
                parts = line.split()
                verts.append((float(parts[1]), float(parts[2]), float(parts[3])))
            elif line.startswith("f "):
                idx = []
                for tok in line.split()[1:]:
                    i = int(tok.split("/")[0])
                    idx.append(i - 1 if i > 0 else len(verts) + i)
                if len(idx) >= 3:
                    faces.append(idx)
    V = np.asarray(verts, dtype=np.float64)
    # faces kept rectangular, missing corners marked -1 (real polygons only)
    maxn = max(len(f) for f in faces)
    F = np.full((len(faces), maxn), -1, dtype=np.int64)
    for i, f in enumerate(faces):
        F[i, :len(f)] = f
    return V, F


def topo(V, F):
    """topology stats for polygon soup (V verts, F (n,4) index matrix, -1 pad)."""
    # weld exact-duplicate positions
    key = np.round(V, 7)
    _, inv = np.unique(key, axis=0, return_inverse=True)
    Fw = inv[F]
    n = F.shape[1]
    if n >= 4:
        tri_i = np.where((F[:, 2] >= 0) & (F[:, 3] < 0))[0]
        quad_i = np.where(F[:, 3] >= 0)[0]
    else:
        tri_i = np.where(F[:, 2] >= 0)[0]
        quad_i = np.array([], dtype=np.int64)
    e = []
    if len(tri_i):
        T = Fw[tri_i]
        e.append(np.sort(np.vstack([T[:, [0, 1]], T[:, [1, 2]], T[:, [2, 0]]]), axis=1))
    if len(quad_i):
        Q = Fw[quad_i]
        e.append(np.sort(np.vstack([Q[:, [0, 1]], Q[:, [1, 2]],
                                    Q[:, [2, 3]], Q[:, [3, 0]]]), axis=1))
    e = np.vstack(e)
    uniq, cnt = np.unique(e, axis=0, return_counts=True)
    boundary = int((cnt == 1).sum())
    nonmanifold = int((cnt > 2).sum())
    valid = Fw >= 0
    V_used = np.unique(Fw[valid])
    nV, nE, nF = len(V_used), len(uniq), int(valid.sum())
    # degenerate / zero-area faces (fan triangulation)
    tris = []
    if len(tri_i):
        tris.append(Fw[tri_i[:, None], np.array([0, 1, 2])[None, :]])
    if len(quad_i):
        Q = Fw[quad_i]
        tris.append(Q[:, [0, 1, 2]])
        tris.append(Q[:, [0, 2, 3]])
    T = np.vstack(tris)
    p = V[T]
    area = 0.5 * np.linalg.norm(np.cross(p[:, 1] - p[:, 0], p[:, 2] - p[:, 0]), axis=1)
    zero = area < 1e-14
    dup_key = np.sort(T, axis=1)
    _, dcnt = np.unique(dup_key, axis=0, return_counts=True)
    return dict(boundary_edges=boundary, nonmanifold_edges=nonmanifold,
                euler=int(nV - nE + nF), verts_welded=int(nV),
                watertight=(boundary == 0 and nonmanifold == 0),
                zero_area_faces=int(zero.sum() // 1),
                duplicate_faces=int((dcnt > 1).sum()))


def quad_quality(V, F):
    """Scaled Jacobian / angles for polygonal faces (quads + tris)."""
    v = V
    f = F
    ncols = f.shape[1]
    quad_mask = (f[:, 3] >= 0) if ncols >= 4 else np.zeros(len(f), bool)
    tri_mask = (~quad_mask) & (f[:, 2] >= 0)

    res = {}
    pad = f
    # ---- quads ----
    if quad_mask.any():
        q = pad[quad_mask]
        p = v[q]  # (n,4,3)
        sj = []
        for i in range(4):
            du = p[:, (i + 1) % 4] - p[:, i]
            dv = p[:, (i + 3) % 4] - p[:, i]
            dw = p[:, (i + 2) % 4] - p[:, i]
            nu = np.linalg.norm(du, axis=1)
            nv_ = np.linalg.norm(dv, axis=1)
            nw = np.linalg.norm(dw, axis=1)
            cross = np.cross(du, dv)
            denom = np.where(nu * nv_ * nw == 0, 1e-30, nu * nv_ * nw)
            s = (cross * dw).sum(1) / denom
            sj.append(s)
        sj = np.array(sj)
        sjmin = sj.min(0)
        # angles
        angs = []
        for i in range(4):
            a = p[:, (i + 3) % 4] - p[:, i]
            b = p[:, (i + 1) % 4] - p[:, i]
            la = np.linalg.norm(a, axis=1); lb = np.linalg.norm(b, axis=1)
            d = (a * b).sum(1) / np.where(la * lb == 0, 1e-30, la * lb)
            angs.append(np.degrees(np.arccos(np.clip(d, -1, 1))))
        angs = np.array(angs)
        # edge lengths for aspect ratio
        el = np.stack([np.linalg.norm(p[:, (i + 1) % 4] - p[:, i], axis=1) for i in range(4)])
        ar = el.max(0) / np.where(el.min(0) == 0, 1e-30, el.min(0))
        res.update(
            quads=int(quad_mask.sum()),
            sj_min=float(sjmin.min()), sj_median=float(np.median(sjmin)),
            sj_p05=float(np.percentile(sjmin, 5)),
            sj_mean=float(sjmin.mean()),
            frac_sj_negative=float((sjmin < 0).mean()),
            frac_sj_lt_0_2=float((sjmin < 0.2).mean()),
            frac_sj_lt_0_5=float((sjmin < 0.5).mean()),
            min_angle=float(angs.min()), max_angle=float(angs.max()),
            angle_median_dev=float(np.median(np.abs(angs - 90))),
            aspect_median=float(np.median(ar)), aspect_p95=float(np.percentile(ar, 95)),
            aspect_max=float(ar.max()),
            frac_aspect_gt_4=float((ar > 4).mean()),
        )
    # ---- tris ----
    if tri_mask.any():
        t = pad[tri_mask]
        p = v[t]
        a = np.linalg.norm(np.cross(p[:, 1] - p[:, 0], p[:, 2] - p[:, 0]), axis=1)
        el = np.stack([np.linalg.norm(p[:, 1] - p[:, 0], axis=1),
                       np.linalg.norm(p[:, 2] - p[:, 1], axis=1),
                       np.linalg.norm(p[:, 0] - p[:, 2], axis=1)])
        s = el.sum(0)
        r = np.where(a == 0, 0, 2 * a / np.where(s == 0, 1e-30, s))  # 1 = equilateral ideal-ish

        def ang(u, w):
            lu = np.linalg.norm(u, axis=1); lw = np.linalg.norm(w, axis=1)
            den = np.where(lu * lw == 0, 1e-30, lu * lw)
            return np.degrees(np.arccos(np.clip((u * w).sum(1) / den, -1, 1)))

        tri_ang = np.stack([ang(p[:, 1] - p[:, 0], p[:, 2] - p[:, 0]),
                            ang(p[:, 0] - p[:, 1], p[:, 2] - p[:, 1]),
                            ang(p[:, 0] - p[:, 2], p[:, 1] - p[:, 2])])
        res.update(tris=int(tri_mask.sum()),
                   tri_min_angle=float(tri_ang.min()), tri_max_angle=float(tri_ang.max()),
                   tri_quality_mean=float(r.mean()), frac_tri_degen=float((a < 1e-14).mean()))
    res["poly_count"] = int(len(f))
    res["quad_ratio"] = float(res.get("quads", 0) / max(1, len(f)))
    return res


def accuracy(src, out, diag):
    q = trimesh.proximity.ProximityQuery(src)
    r = {}
    # out -> src (how far the new surface drifts from original)
    pts, _ = trimesh.sample.sample_surface(out, SAMPLES)
    d1 = np.abs(q.signed_distance(pts)) if False else None
    cp, dist, tid = q.on_surface(pts)
    r["out2src_mean"] = float(dist.mean()); r["out2src_rms"] = float(np.sqrt((dist ** 2).mean()))
    r["out2src_p95"] = float(np.percentile(dist, 95)); r["out2src_p99"] = float(np.percentile(dist, 99))
    r["out2src_max"] = float(dist.max())
    # src -> out (coverage: does new mesh cover all of the original?)
    q2 = trimesh.proximity.ProximityQuery(out)
    pts2, _ = trimesh.sample.sample_surface(src, SAMPLES)
    cp2, dist2, tid2 = q2.on_surface(pts2)
    r["src2out_mean"] = float(dist2.mean()); r["src2out_rms"] = float(np.sqrt((dist2 ** 2).mean()))
    r["src2out_p95"] = float(np.percentile(dist2, 95)); r["src2out_p99"] = float(np.percentile(dist2, 99))
    r["src2out_max"] = float(dist2.max())
    for k in list(r):
        r[k + "_pct_diag"] = 100.0 * r[k] / diag
    r["chamfer_rms_pct"] = 0.5 * (r["out2src_rms_pct_diag"] + r["src2out_rms_pct_diag"])
    r["hausdorff_pct"] = max(r["out2src_max_pct_diag"], r["src2out_max_pct_diag"])
    return r


def poly_to_tris(V, F):
    tris = []
    for i in range(1, F.shape[1] - 1):
        ok = (F[:, 0] >= 0) & (F[:, i] >= 0) & (F[:, i + 1] >= 0)
        if ok.any():
            tris.append(F[ok][:, [0, i, i + 1]])
    return trimesh.Trimesh(V, np.vstack(tris), process=False)


def main():
    SV, SF = load(SRC)
    src_tri = poly_to_tris(SV, SF)
    diag = float(np.linalg.norm(src_tri.extents))
    print(f"SOURCE: {len(SV)} v / {len(SF)} f, bbox diag = {diag:.4f}")
    print("  topo:", json.dumps(topo(SV, SF)))
    print(f"  area = {src_tri.area:.6f}")
    res_all = {}
    for p in OUTS:
        V, F = load(p)
        m = poly_to_tris(V, F)
        print(f"\n=== {p} ===")
        print(f"  {len(V)} v / {len(F)} f")
        t = topo(V, F)
        print("  topo:", json.dumps(t))
        q = quad_quality(V, F)
        print("  quad:", json.dumps({k: (round(v, 4) if isinstance(v, float) else v)
                                     for k, v in q.items()}))
        a = accuracy(src_tri, m, diag)
        print("  acc %diag:", json.dumps({k: round(v, 5) for k, v in a.items()
                                          if k.endswith("pct_diag") or k.endswith("_pct")
                                          or k in ("hausdorff_pct", "chamfer_rms_pct")}))
        print("  acc abs  :", json.dumps({k: round(v, 7) for k, v in a.items()
                                          if not k.endswith("pct_diag") and not k.endswith("_pct")
                                          and k not in ("hausdorff_pct", "chamfer_rms_pct")}))
        print(f"  area = {m.area:.6f}  ({100 * m.area / src_tri.area:.2f}% of source)")
        res_all[p] = dict(topo=t, quad=q, acc=a, nv=int(len(V)), nf=int(len(F)),
                          area=float(m.area))
    with open("test_out/eval_report.json", "w") as fh:
        json.dump(res_all, fh, indent=2)
    print("\nSaved test_out/eval_report.json")


if __name__ == "__main__":
    main()
