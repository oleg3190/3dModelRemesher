#!/usr/bin/env python3
"""Shape-aware retopology: premise test BEFORE architecture (the user's T20 spec).

The spec claims our output turns flat structures into a lumpy mass ("мост превращается в
кракозябру") and that a silhouette/planarity-driven grid fixes it. It also puts semantic shape
ABOVE geometric deviation, which contradicts invariant #7 (snap on-surface) - so it must be
measured, not implemented on faith. Three numbers decide it:

  1. FLATNESS OF THE SOURCE  - does the model actually contain large planar regions? (S48 already
     showed planar areas cover 32-54% of the AI model and the edges between them have a median
     dihedral of 13-19 deg, i.e. "plane segmentation by normals" is noisy - so regions must pass an
     area AND compactness AND residual filter before they are trusted.)
  2. HOW MUCH OUR OUTPUT BULGES IN THOSE REGIONS  - p50/p95/max distance of output face centroids
     to the fitted plane, in % of the model diagonal. The spec's target is <0.1%.
  3. SILHOUETTE IoU vs the source over N views - the spec's own primary metric (target >0.95).

Plus a CEILING experiment that costs no C++: snap the output onto the trusted planes and re-measure
everything. If the bulge is not what the user actually sees as "кракозябра", or if the ceiling
gain is small, the whole architecture is not worth its cost. If the ceiling is big, the number
tells us what a shape-aware pipeline could buy before we build it.

Usage:
  shape_probe.py OUT.obj SRC.obj [--views 12] [--res 384] [--theta 6] [--sag 0.002]
                 [--minarea 0.005] [--snap-out /tmp/snap.obj] [--json]
"""
import argparse
import io
import json
import os
import sys
from collections import defaultdict

import numpy as np
from PIL import Image, ImageDraw
from scipy.spatial import cKDTree

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from smooth_creases import load_polys, face_normals, edge_dihedrals, scaled_jacobian  # noqa: E402


# ------------------------------------------------------------------ geometry helpers ----------
def fan_tris(po):
    tris = []
    for idx in po:
        for i in range(1, len(idx) - 1):
            tris.append((idx[0], idx[i], idx[i + 1]))
    return np.asarray(tris, dtype=np.int64) if tris else np.zeros((0, 3), np.int64)


def face_adjacency(po):
    e2f = defaultdict(list)
    for fi, idx in enumerate(po):
        n = len(idx)
        for i in range(n):
            a, b = idx[i], idx[(i + 1) % n]
            e2f[(min(a, b), max(a, b))].append(fi)
    adj = defaultdict(set)
    for fs in e2f.values():
        for a in fs:
            for b in fs:
                if a != b:
                    adj[a].add(b)
    return adj


def planar_regions(V, po, theta_deg, sag, min_area_frac, max_aspect):
    """Region-grow planar faces, then keep only trustworthy ones (area, compactness, residual).

    S48 lesson: raw normal region growing on an AI mesh returns 21k 'regions' - useless. The
    filters below are what turns it into a semantic cue at all.
    """
    N = face_normals(V, po)
    C = np.array([V[idx].mean(axis=0) for idx in po])
    AR = np.array([_poly_area(V[idx]) for idx in po])
    total = float(AR.sum())
    adj = face_adjacency(po)
    cos_t = np.cos(np.radians(theta_deg))

    label = np.full(len(po), -1, dtype=np.int64)
    regions = []
    for seed in range(len(po)):
        if label[seed] >= 0:
            continue
        rid = len(regions)
        label[seed] = rid
        n0, o0 = N[seed], C[seed]
        stack, members = [seed], [seed]
        while stack:
            f = stack.pop()
            for g in adj[f]:
                if label[g] >= 0 or N[g] @ n0 < cos_t or abs((C[g] - o0) @ n0) > sag:
                    continue
                label[g] = rid
                members.append(g)
                stack.append(g)
        regions.append(members)

    out = []
    for r, m in enumerate(regions):
        area = float(AR[m].sum())
        if area < min_area_frac * total:
            continue
        vs = set(v for fi in m for v in po[fi])
        P = V[np.array(sorted(vs))]
        ext = P.max(axis=0) - P.min(axis=0)
        srt = np.sort(ext)[::-1]
        if srt[0] <= 1e-9 or srt[2] / srt[0] > max_aspect:
            continue                       # not a plate: it is a curve or a blob in 3D
        cen = P.mean(axis=0)
        u, s, vt = np.linalg.svd(P - cen, full_matrices=False)
        n = vt[2]
        resid = float(np.sqrt(np.mean(((P - cen) @ n) ** 2)))
        if resid > sag:
            continue
        d = float(cen @ n)
        out.append({"faces": m, "area": area, "n": n.tolist(), "d": d,
                    "resid": resid, "extent": srt.tolist(), "verts": sorted(vs)})
    return out, AR, total


def _poly_area(p):
    a = 0.0
    for i in range(1, len(p) - 1):
        a += 0.5 * np.linalg.norm(np.cross(p[i] - p[0], p[i + 1] - p[0]))
    return float(a)


# ------------------------------------------------------------------- silhouette IoU --------------
def _basis(dirv):
    d = dirv / np.linalg.norm(dirv)
    ref = np.array([0.0, 0.0, 1.0]) if abs(d[2]) < 0.9 else np.array([1.0, 0.0, 0.0])
    u = np.cross(ref, d); u /= np.linalg.norm(u)
    v = np.cross(d, u)
    return u, v, d


def _views(n):
    # deterministic ring pattern: latitude bands, azimuthal samples (no RNG)
    pts = []
    rings = max(1, int(round(np.sqrt(n / 2))))
    per = max(1, n // rings)
    for i in range(rings):
        lat = -1.2 + 2.4 * (i + 0.5) / rings
        for j in range(per):
            lon = 2 * np.pi * j / per
            pts.append(np.array([np.cos(lat) * np.cos(lon), np.cos(lat) * np.sin(lon), np.sin(lat)]))
    return np.array(pts[:n])


def silhouette_masks(V, tris, views, res, stride, frames):
    """Union of projected triangles == silhouette (valid for closed meshes).

    frames carries the (mn, span) of the REFERENCE mesh per view, so the candidate is never
    allowed to re-normalize itself into agreement (that would hide a scale/extent error).
    """
    imgs = []
    T = tris[::max(1, stride)]
    if len(T) == 0:
        T = tris
    uniq, inv = np.unique(T.ravel(), return_inverse=True)
    Vw = V[uniq]
    T = inv.reshape(-1, 3)
    for dv, (mn, span) in zip(views, frames):
        u, v, d = _basis(dv)
        P = Vw @ np.vstack([u, v, d]).T
        img = Image.new("1", (res, res), 0)
        dr = ImageDraw.Draw(img)
        Q = (P[:, :2] - mn) / span * (res - 1)
        for a, b, c in T:
            dr.polygon([(Q[a][0], Q[a][1]), (Q[b][0], Q[b][1]), (Q[c][0], Q[c][1])], fill=1)
        imgs.append(np.array(img, dtype=bool))
    return imgs


def silhouette_frames(Vs, views, res):
    fr = []
    for dv in views:
        u, v, d = _basis(dv)
        P = Vs @ np.vstack([u, v, d]).T
        mn = P[:, :2].min(axis=0)
        mx = P[:, :2].max(axis=0)
        fr.append((mn, max(mx[0] - mn[0], mx[1] - mn[1]) * 1.02 + 1e-9))
    return fr


def silhouette_iou(Vo, To, Vs, Ts, views=12, res=256, stride_out=1, stride_src=16):
    vs = _views(views)
    fr = silhouette_frames(Vs, vs, res)
    mo = silhouette_masks(Vo, To, vs, res, stride_out, fr)
    ms = silhouette_masks(Vs, Ts, vs, res, stride_src, fr)
    ious = []
    for a, b in zip(mo, ms):
        inter = np.logical_and(a, b).sum()
        union = np.logical_or(a, b).sum()
        ious.append(float(inter / union) if union else 1.0)
    return ious


# ------------------------------------------------------------- planarity + snap ceiling ---------
def planarity_report(V, po, regions, src_V, src_po, kdt_src, reg_of_srcface, diag):
    """Deviation of output face centroids from the fitted planes of trusted flat regions."""
    C = np.array([V[idx].mean(axis=0) for idx in po])
    nearest = kdt_src.query(C)[1]
    dev = []
    for i, sf in enumerate(nearest):
        r = reg_of_srcface.get(int(sf), -1)
        if r < 0:
            continue
        n = np.asarray(regions[r]["n"]); d = regions[r]["d"]
        dev.append(abs(float(C[i] @ n - d)))
    if not dev:
        return {"faces_in_flat_regions": 0}
    dev = np.array(dev)
    return {"faces_in_flat_regions": int(len(dev)),
            "flat_dev_p50_pct": 100.0 * float(np.median(dev)) / diag,
            "flat_dev_p95_pct": 100.0 * float(np.percentile(dev, 95)) / diag,
            "flat_dev_max_pct": 100.0 * float(dev.max()) / diag}


def snap_to_planes(V, po, regions, src_V, src_po, kdt_src, reg_of_srcface, out_path):
    """CEILING experiment: move output vertices onto the trusted plane of their nearest region."""
    Vs = np.array(V, dtype=float)
    nearest = kdt_src.query(np.array([V[idx].mean(axis=0) for idx in po]))[1]
    moved = defaultdict(int)
    face_reg = {}
    for i, sf in enumerate(nearest):
        r = reg_of_srcface.get(int(sf), -1)
        if r >= 0:
            face_reg[i] = r
    # a vertex is snapped only when ALL its faces agree on the same region
    vfaces = defaultdict(list)
    for fi, idx in enumerate(po):
        for v in idx:
            vfaces[v].append(fi)
    snapped = 0
    for v, fs in vfaces.items():
        rs = {face_reg[f] for f in fs if f in face_reg}
        if len(fs) == 0 or len(rs) != 1:
            continue
        r = rs.pop()
        n = np.asarray(regions[r]["n"]); d = regions[r]["d"]
        newp = Vs[v] - (Vs[v] @ n - d) * n
        if np.linalg.norm(newp - Vs[v]) > 1e-12:
            Vs[v] = newp
            snapped += 1
    with open(out_path, "w") as fh:
        for p in Vs:
            fh.write("v %.7f %.7f %.7f\n" % tuple(p))
        for idx in po:
            fh.write("f %s\n" % " ".join(str(x + 1) for x in idx))
    return snapped


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("src")
    ap.add_argument("--views", type=int, default=12)
    ap.add_argument("--res", type=int, default=256)
    ap.add_argument("--theta", type=float, default=6.0)
    ap.add_argument("--sag", type=float, default=0.002)
    ap.add_argument("--minarea", type=float, default=0.005)
    ap.add_argument("--max-aspect", type=float, default=0.45, help="thin extent / long extent")
    ap.add_argument("--snap-out", default="")
    ap.add_argument("--stride-src", type=int, default=1,
                    help="face subsample of the reference mesh; >1 punches holes in its silhouette "
                         "and fakes a low IoU (measured: src-vs-src with stride 16/1 reads 0.939)")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    V, po = load_polys(a.out)
    Vs, ps = load_polys(a.src)
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))

    regions, AR, total = planar_regions(Vs, ps, a.theta, a.sag, a.minarea, a.max_aspect)
    reg_of_srcface = {}
    for r, reg in enumerate(regions):
        for fi in reg["faces"]:
            reg_of_srcface[fi] = r
    kdt_src = cKDTree(np.array([Vs[idx].mean(axis=0) for idx in ps]))

    flat_area = float(sum(reg["area"] for reg in regions))
    res = {
        "out": a.out, "src": a.src, "diag": diag,
        "trusted_flat_regions": len(regions),
        "flat_area_share": flat_area / total,
        "largest_flat_share": (max(reg["area"] for reg in regions) / total) if regions else 0.0,
    }
    res.update(planarity_report(V, po, regions, Vs, ps, kdt_src, reg_of_srcface, diag))

    To = fan_tris(po); Ts = fan_tris(ps)
    ious = silhouette_iou(V, To, Vs, Ts, views=a.views, res=a.res, stride_src=a.stride_src)
    res["silhouette_iou_mean"] = float(np.mean(ious))
    res["silhouette_iou_min"] = float(np.min(ious))
    res["silhouette_iou_p25"] = float(np.percentile(ious, 25))

    if a.snap_out and regions:
        n = snap_to_planes(V, po, regions, Vs, ps, kdt_src, reg_of_srcface, a.snap_out)
        Vs2, ps2 = load_polys(a.snap_out)
        res["snap_vertices"] = n
        res.update({("snap_" + k): v for k, v in
                    planarity_report(Vs2, ps2, regions, Vs, ps, kdt_src, reg_of_srcface, diag).items()})
        Ns2 = face_normals(Vs2, ps2)
        maxd2, _ = edge_dihedrals(Vs2, ps2, Ns2)
        S = scaled_jacobian(Vs2, ps2)
        res["snap_min_sj"] = float(S.min())
        res["snap_median_sj"] = float(np.median(S))
        res["snap_creased_quads"] = int(((maxd2 > 120.0) & np.array([len(i) == 4 for i in ps2])).sum())
    print(json.dumps(res, indent=1) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))


if __name__ == "__main__":
    main()
