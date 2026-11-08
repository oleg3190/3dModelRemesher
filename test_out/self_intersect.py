#!/usr/bin/env python3
"""Self-intersection / double-coverage detector for the quad output (T8 / BUG-01).

The T7 measurement showed the smooth-zone creases are PLEATS: a face folded back over its own
neighbour (both faces on the same side of the shared edge). T8 asks the decisive question:

  is the region ALSO covered twice (non-adjacent faces penetrating each other), or is the fold
  a purely local inversion of an otherwise singly-covered sheet?

Answer decides the fix location:
  - many non-adjacent penetrations   -> layout/assembly produces double coverage (stage5 graph)
  - folds only, no penetration       -> coarse grid over curvature: the sheet pinches, fix is in
                                        relaxation/parametrization, not in the patch graph

Method: fan-triangulate, spatial-hash triangle AABBs, exact Moeller triangle-triangle test.
Adjacent pairs (sharing a vertex) are counted separately - a fold is adjacent by definition.

Usage:
  self_intersect.py OUT.obj [--cell-scale 1.0] [--json] [--faces faces.json]
"""
import argparse
import json
import sys
from collections import defaultdict

import numpy as np

import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from smooth_creases import load_polys  # noqa: E402


def tri_list(V, polys):
    tris, tf = [], []
    for fi, idx in enumerate(polys):
        for i in range(1, len(idx) - 1):
            tris.append((idx[0], idx[i], idx[i + 1]))
            tf.append(fi)
    return np.asarray(tris, dtype=np.int64), np.asarray(tf, dtype=np.int64)


def moeller(p0, q0):
    """True if triangles p0 and q0 (each 3x3 float) intersect (Moeller 1997)."""
    n1 = np.cross(p0[1] - p0[0], p0[2] - p0[0])
    d1 = -np.dot(n1, p0[0])
    n2 = np.cross(q0[1] - q0[0], q0[2] - q0[0])
    d2 = -np.dot(n2, q0[0])

    du = np.array([np.dot(n1, q0[i]) + d1 for i in range(3)])
    dv = np.array([np.dot(n2, p0[i]) + d2 for i in range(3)])

    # reject if one triangle lies strictly on one side of the other's plane
    def all_same(x):
        return (x[0] > 1e-12 and x[1] > 1e-12 and x[2] > 1e-12) or \
               (x[0] < -1e-12 and x[1] < -1e-12 and x[2] < -1e-12)
    if all_same(du) or all_same(dv):
        return False

    # intersection line of the two planes
    u = np.cross(n1, n2)
    ax = abs(u[0]); ay = abs(u[1]); az = abs(u[2])
    if max(ax, ay, az) < 1e-12:
        return False  # coplanar-ish: handled separately
    if ax >= ay and ax >= az:
        k = 0
    elif ay >= az:
        k = 1
    else:
        k = 2

    def project(tri, dd):
        pts, segs = [], []
        for i in range(3):
            j = (i + 1) % 3
            pts.append(tri[i][k])
            a, b = dd[i], dd[j]
            if (a > 0 and b < 0) or (a < 0 and b > 0):
                t = a / (a - b)
                segs.append(tri[i][k] + t * (tri[j][k] - tri[i][k]))
            elif abs(a) <= 1e-12:
                segs.append(tri[i][k])
            elif abs(b) <= 1e-12:
                segs.append(tri[j][k])
        return pts, segs

    pp, sp = project(p0, dv)
    pq, sq = project(q0, du)
    if len(sp) < 2 or len(sq) < 2:
        return False
    lo1, hi1 = min(sp), max(sp)
    lo2, hi2 = min(sq), max(sq)
    if lo1 > hi2 + 1e-12 or lo2 > hi1 + 1e-12:
        return False
    if min(hi1, hi2) - max(lo1, lo2) < 1e-12:
        return False
    return True


def coplanar_overlap(p0, q0, eps=1e-9):
    """Cheap test for near-coplanar overlapping triangles (the pleat case)."""
    n1 = np.cross(p0[1] - p0[0], p0[2] - p0[0])
    ln = np.linalg.norm(n1)
    if ln < eps:
        return False
    n1 = n1 / ln
    # q must lie in p's plane
    if any(abs(np.dot(q0[i] - p0[0], n1)) > 1e-4 * max(1.0, np.linalg.norm(p0[1] - p0[0]))
           for i in range(3)):
        return False
    # 2D projection
    ax = abs(n1[0]); ay = abs(n1[1]); az = abs(n1[2])
    keep = [i for i in range(3) if i != (0 if ax >= ay and ax >= az else (1 if ay >= az else 2))]
    A = p0[:, keep]
    B = q0[:, keep]

    def inside(poly, pt):
        sign = 0
        for i in range(3):
            a, b = poly[i], poly[(i + 1) % 3]
            c = np.cross(b - a, pt - a)
            s = 1 if c > 1e-12 else (-1 if c < -1e-12 else 0)
            if s == 0:
                continue
            if sign == 0:
                sign = s
            elif sign != s:
                return False
        return True
    for pt in B:
        if inside(A, pt):
            return True
    for pt in A:
        if inside(B, pt):
            return True
    return moeller(p0, q0)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("--cell-scale", type=float, default=1.0)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--faces", default="")
    a = ap.parse_args()

    V, polys = load_polys(a.out)
    T, TF = tri_list(V, polys)
    P = V[T]                      # (nt,3,3)
    nt = len(T)
    cen = P.mean(axis=1)
    edges = np.linalg.norm(P[:, 1] - P[:, 0], axis=1)
    cell = max(1e-9, float(np.median(edges)) * a.cell_scale)

    # vertex sets per face, to tell adjacent from non-adjacent
    def vkey(t):
        return set(int(x) for x in T[t])

    # spatial hash over triangle centroids
    grid = defaultdict(list)
    inv = 1.0 / cell
    for i in range(nt):
        k = (int(np.floor(cen[i][0] * inv)), int(np.floor(cen[i][1] * inv)),
             int(np.floor(cen[i][2] * inv)))
        grid[k].append(i)

    nonadj, adj = set(), set()
    tested = 0
    for k, idxs in grid.items():
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                for dz in (-1, 0, 1):
                    for j in grid.get((k[0] + dx, k[1] + dy, k[2] + dz), ()):
                        for i in idxs:
                            if i >= j:
                                continue
                            if np.linalg.norm(cen[i] - cen[j]) > 1.8 * cell:
                                continue
                            tested += 1
                            shared = vkey(i) & vkey(j)
                            if coplanar_overlap(P[i], P[j]) or moeller(P[i], P[j]):
                                (adj if shared else nonadj).add((min(TF[i], TF[j]), max(TF[i], TF[j])))

    res = {
        "out": a.out, "tris": nt, "polys": len(polys), "cell": cell,
        "pairs_tested": tested // 2,
        "nonadjacent_face_pairs_penetrating": len(nonadj),
        "adjacent_face_pairs_penetrating": len(adj),
        "nonadjacent_polys": sorted({p for pr in nonadj for p in pr}),
    }
    if a.faces:
        with open(a.faces, "w") as fh:
            json.dump(res, fh)
    if a.json:
        print(json.dumps({k: v for k, v in res.items() if k != "nonadjacent_polys"}))
    else:
        for k, v in res.items():
            if k == "nonadjacent_polys":
                print(f"  {k}: {len(v)} faces -> {v[:30]}{' ...' if len(v) > 30 else ''}")
            else:
                print(f"  {k}: {v}")


if __name__ == "__main__":
    sys.exit(main())
