#!/usr/bin/env python3
"""Analytic detector + DoD metric for BUG-01 / T7 (smooth-source x creased-output).

A remeshed output face is a REPAIR TARGET when BOTH hold:
  1. it is creased: max dihedral over its edges > --out-dih (default 120 deg,
     dihedral = angle between neighbouring face normals, winding-independent);
  2. the ORIGINAL source surface under it is smooth: max dihedral among source
     faces with centroid within R = --rel * bbox-diag (default 0.01) < --src-rough
     (default 30 deg).

Gate (2) is essential: on the AI-mesh source most creased output faces are real
relief, not defects. "Smooth everything" would delete detail.

Usage:
  smooth_creases.py OUT.obj SRC.obj [--out-dih 120] [--src-rough 30] [--rel 0.01]
                        [--json] [--targets targets.obj]

Prints: creased_out, on_smooth_source (the metric that must go ~0), plus
Min/median Scaled Jacobian of the whole output (stage6 convention).
"""
import argparse
import json
import sys
import numpy as np
from scipy.spatial import cKDTree


def load_polys(path):
    verts, polys = [], []
    with open(path) as fh:
        for line in fh:
            if line.startswith("v "):
                p = line.split()
                verts.append((float(p[1]), float(p[2]), float(p[3])))
            elif line.startswith("f "):
                idx = [int(t.split("/")[0]) - (1 if int(t.split("/")[0]) > 0 else 0)
                       for t in line.split()[1:]]
                idx = [i if i >= 0 else len(verts) + i for i in idx]
                if len(idx) >= 3:
                    polys.append(idx)
    return np.asarray(verts, dtype=np.float64), polys


def winding_conflicts(polys):
    """Edges whose two incident faces traverse them in the SAME direction.

    The detector's dihedral is a signed dot of face normals, so a ~180 deg reading means a
    geometric fold ONLY while the mesh is consistently oriented; otherwise it is flipped
    winding. Returns the number of conflicting edges (0 = signed dihedral is trustworthy).
    """
    seen = {}
    conflicts = 0
    for fi, idx in enumerate(polys):
        n = len(idx)
        for i in range(n):
            a, b = idx[i], idx[(i + 1) % n]
            if a == b:
                continue
            key = (min(a, b), max(a, b))
            d = 1 if a < b else -1
            if key not in seen:
                seen[key] = (d, fi)
            else:
                d0, f0 = seen[key]
                if f0 != fi:
                    if d0 == d:
                        conflicts += 1
                    seen[key] = (d0, f0)  # keep first; >2 faces handled by the caller's gate
    return conflicts


def face_normals(V, polys):
    """Newell normals per polygon (robust for non-planar quads)."""
    N = np.zeros((len(polys), 3))
    for k, idx in enumerate(polys):
        p = V[idx]
        n = np.zeros(3)
        for i in range(len(p)):
            a, b = p[i], p[(i + 1) % len(p)]
            n += np.cross(a, b)
        N[k] = n
    ln = np.linalg.norm(N, axis=1)
    N[ln > 1e-12] /= ln[ln > 1e-12][:, None]
    return N


def edge_adjacency(polys):
    """map undirected edge -> [face ids] (manifold-ish: keep first two)."""
    adj = {}
    for fi, idx in enumerate(polys):
        n = len(idx)
        for i in range(n):
            a, b = idx[i], idx[(i + 1) % n]
            key = (a, b) if a < b else (b, a)
            adj.setdefault(key, []).append(fi)
    return adj


def edge_dihedrals(V, polys, N):
    """Return (face_max_dihedral_deg, edge_list_with_dihedral)."""
    adj = edge_adjacency(polys)
    maxd = np.zeros(len(polys))
    edges = []
    for (a, b), fs in adj.items():
        if len(fs) != 2:
            continue
        f0, f1 = fs
        dot = float(np.clip(np.dot(N[f0], N[f1]), -1.0, 1.0))
        d = np.degrees(np.arccos(dot))          # 0 = coplanar, 180 = folded back
        edges.append((a, b, f0, f1, d))
        if d > maxd[f0]:
            maxd[f0] = d
        if d > maxd[f1]:
            maxd[f1] = d
    return maxd, edges


def scaled_jacobian(V, polys):
    """Same convention as stage6/quad_folds.py (quads only)."""
    quads = [p for p in polys if len(p) == 4]
    if not quads:
        return np.array([1.0])
    Q = V[np.asarray(quads)]
    p0, p1, p2, p3 = Q[:, 0], Q[:, 1], Q[:, 2], Q[:, 3]
    nvec = np.cross(p2 - p0, p3 - p1)
    ln = np.linalg.norm(nvec, axis=1)
    good = ln > 1e-8
    nvec = nvec / np.where(ln[:, None] == 0, 1.0, ln[:, None])
    sj = np.full(len(Q), 1.0)
    for i in range(4):
        e1 = Q[:, (i + 1) % 4] - Q[:, i]
        e2 = Q[:, (i + 3) % 4] - Q[:, i]
        l1 = np.linalg.norm(e1, axis=1)
        l2 = np.linalg.norm(e2, axis=1)
        ok = good & (l1 > 1e-8) & (l2 > 1e-8)
        e1n = e1 / np.where(l1[:, None] == 0, 1.0, l1[:, None])
        e2n = e2 / np.where(l2[:, None] == 0, 1.0, l2[:, None])
        j = np.full(len(Q), -1.0)
        vals = np.einsum('ij,ij->i', np.cross(e1n, e2n), nvec)
        j[ok] = vals[ok]
        sj = np.minimum(sj, j)
    return sj


def triangulate(V, polys):
    """Fan-triangulate polygons; returns tri vertex indices + tri->face id."""
    tris, tf = [], []
    for fi, idx in enumerate(polys):
        for i in range(1, len(idx) - 1):
            tris.append((idx[0], idx[i], idx[i + 1]))
            tf.append(fi)
    return np.asarray(tris, dtype=np.int64), np.asarray(tf, dtype=np.int64)


def source_roughness(Vs, polys, Ns, rel):
    """Per source face: max dihedral among faces whose centroid is within R.

    Returns (centroids, kd, rough_fn) where rough_fn(points) -> max dihedral in
    the neighbourhood of each point.
    """
    maxd, _ = edge_dihedrals(Vs, polys, Ns)
    C = np.array([Vs[idx].mean(axis=0) for idx in polys])
    diag = float(np.linalg.norm(Vs.max(axis=0) - Vs.min(axis=0)))
    R = rel * diag
    kd = cKDTree(C)
    return C, kd, R, maxd, diag


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("src")
    ap.add_argument("--out-dih", type=float, default=120.0)
    ap.add_argument("--src-rough", type=float, default=30.0)
    ap.add_argument("--rel", type=float, default=0.01)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--targets", default="", help="write target face centroids as OBJ points")
    a = ap.parse_args()

    Vo, po = load_polys(a.out)
    No = face_normals(Vo, po)
    maxd_o, _ = edge_dihedrals(Vo, po, No)
    creased = np.where(maxd_o > a.out_dih)[0]

    Vs, ps = load_polys(a.src)
    Ns = face_normals(Vs, ps)
    Cs, kd, R, maxd_s, diag = source_roughness(Vs, ps, Ns, a.rel)

    Co = np.array([Vo[idx].mean(axis=0) for idx in po]) if po else np.zeros((0, 3))
    # roughness of the source neighbourhood under each creased output face
    neigh = kd.query_ball_point(Co[creased], r=R)
    src_rough = np.array([maxd_s[n].max() if len(n) else 360.0 for n in neigh])
    smooth = creased[src_rough < a.src_rough]
    targets = set()
    for fi in smooth:
        targets.update(int(v) for v in po[fi])

    sj = scaled_jacobian(Vo, po)
    res = {
        "out": a.out, "src": a.src,
        "faces": len(po), "verts": len(Vo),
        "bbox_diag": diag, "radius": R,
        "out_dih": a.out_dih, "src_rough_gate": a.src_rough,
        "creased_out": int(len(creased)),
        "smooth_creases": int(len(smooth)),
        "target_verts": int(len(targets)),
        "winding_conflicts": winding_conflicts(po),
        "src_winding_conflicts": winding_conflicts(ps),
        "min_sj": float(sj.min()), "median_sj": float(np.median(sj)),
        "sj_negative": int((sj < 0).sum()),
    }
    if a.json:
        print(json.dumps(res))
    else:
        for k, v in res.items():
            print(f"  {k}: {v}")
    if a.targets:
        with open(a.targets, "w") as fh:
            fh.write("# smooth-region crease target centroids (BUG-01 / T7)\n")
            for fi in smooth:
                fh.write("v %.6f %.6f %.6f\n" % tuple(Co[fi]))
        print(f"  targets -> {a.targets} ({len(smooth)} points)")


if __name__ == "__main__":
    sys.exit(main())
