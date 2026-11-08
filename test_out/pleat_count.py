#!/usr/bin/env python3
"""Pleat counter (T8 refined metric): adjacent faces folded to the SAME side of their shared edge.

The coverage-multiplicity test (overlap_cover.py) disproved the "duplicate band" theory: only
1/127 BUG-01 target faces sits in a double-covered cell, and the AI-mesh rate (1.98%) is at the
noise floor of a clean control (2.18%). What remains is an effect between ADJACENT faces: two
quads sharing an edge but both extending to the same side of it (a fold, not a duplication).
Usually the folded one is a thin sliver lying on top of a much larger neighbour.

This script counts exactly that, winding-independently (no normals involved):
  for every interior edge with two incident faces:
    project each face's first free vertex against the edge line in the plane perpendicular to it
    same sign  -> PLEAT (fold); record also the area ratio of the two faces
    other sign -> normal flat continuation

Usage:
  pleat_count.py OUT.obj [SRC.obj] [--out-dih 120] [--src-gate 30] [--rel 0.01] [--json]
"""
import argparse
import json
import os
import sys
from collections import defaultdict

import numpy as np
from scipy.spatial import cKDTree

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from smooth_creases import load_polys, face_normals, edge_dihedrals  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("src", nargs="?", default="")
    ap.add_argument("--out-dih", type=float, default=120.0)
    ap.add_argument("--src-gate", type=float, default=30.0)
    ap.add_argument("--rel", type=float, default=0.01)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    V, po = load_polys(a.out)
    N = face_normals(V, po)
    maxd, _ = edge_dihedrals(V, po, N)
    C = np.array([V[idx].mean(axis=0) for idx in po])

    def area(idx):
        p = V[idx]
        if len(p) == 4:
            return 0.5 * float(np.linalg.norm(np.cross(p[2] - p[0], p[3] - p[1])))
        return 0.5 * float(np.linalg.norm(np.cross(p[1] - p[0], p[2] - p[0])))
    ar = np.array([area(i) for i in po])

    e2f = defaultdict(list)
    for fi, idx in enumerate(po):
        n = len(idx)
        for i in range(n):
            x, y = idx[i], idx[(i + 1) % n]
            e2f[(min(x, y), max(x, y))].append((fi, (x, y)))

    pleat, flat, ratio_small = 0, 0, 0
    pleat_faces = set()
    for k, fs in e2f.items():
        if len(fs) != 2:
            continue
        (f0, d0), (f1, d1) = fs
        x, y = k
        px, py = V[x], V[y]
        e = py - px
        L = np.linalg.norm(e)
        if L < 1e-12:
            continue
        e = e / L
        free0 = [v for v in po[f0] if v != x and v != y]
        free1 = [v for v in po[f1] if v != x and v != y]
        if not free0 or not free1:
            continue
        mid = 0.5 * (px + py)
        u0 = V[free0[0]] - mid
        u0 = u0 - np.dot(u0, e) * e
        u1 = V[free1[0]] - mid
        u1 = u1 - np.dot(u1, e) * e
        if np.dot(u0, u1) > 0:
            pleat += 1
            pleat_faces.update((f0, f1))
            if min(ar[f0], ar[f1]) / max(ar[f0], ar[f1]) < 0.25:
                ratio_small += 1
        else:
            flat += 1

    res = {
        "out": a.out, "polys": len(po), "faces": len(V),
        "interior_edges_checked": pleat + flat,
        "pleat_edges_same_side": pleat,
        "flat_opposite_side": flat,
        "pleat_pct": 100.0 * pleat / max(1, pleat + flat),
        "pleat_with_area_ratio_lt_0.25": ratio_small,
        "pleat_faces": len(pleat_faces),
    }

    if a.src:
        Vs, ps = load_polys(a.src)
        Ns = face_normals(Vs, ps)
        maxd_s, _ = edge_dihedrals(Vs, ps, Ns)
        Cs = np.array([Vs[idx].mean(axis=0) for idx in ps])
        kd = cKDTree(Cs)
        diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))
        R = a.rel * diag
        isq = np.array([len(p) == 4 for p in po])
        cre = np.where((maxd > a.out_dih) & isq)[0]
        tset = set()
        for fi in cre:
            nn = kd.query_ball_point(C[fi], r=R)
            if nn and maxd_s[nn].max() < a.src_gate:
                tset.add(int(fi))
        res["bug01_target_faces"] = len(tset)
        res["targets_that_are_pleat_faces"] = len(tset & pleat_faces)
        res["targets_in_pleat_pct"] = 100.0 * len(tset & pleat_faces) / max(1, len(tset))

    print(json.dumps(res) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))


if __name__ == "__main__":
    sys.exit(main())
