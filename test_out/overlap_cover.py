#!/usr/bin/env python3
"""T8 metric: COVERAGE MULTIPLICITY - does the source surface get claimed twice?

Why this metric and not a boolean intersection test nor 2D polygon clipping (both were tried and
rejected by a control): a raw tri-tri predicate fires on quads that merely touch at a pinch (44
false hits on a clean icosphere remesh), and Sutherland-Hodgman needs convexity, which projected
warped quads do not have (it reported overlap area LARGER than the face, ratio 1.7, on the same
control). Neither can be trusted as evidence.

Robust version: every output face is assigned to the source cell (a grid of side = mean output
edge) that contains the closest point of the SOURCE surface. A singly-covered sheet puts ~1 face
per cell; two faces of DIFFERENT, non-adjacent bands landing in the same cell = double coverage.
Calibration is on a clean watertight icosphere remesh, which must stay near zero.

Also correlates the double-covered faces with the BUG-01 target mask (creased output x smooth
source), which is the T8 confirmation test.

Usage:
  overlap_cover.py OUT.obj [SRC.obj] [--cell 1.0] [--json]
"""
import argparse
import json
import os
import sys
from collections import Counter, defaultdict

import numpy as np
from scipy.spatial import cKDTree

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from smooth_creases import load_polys, face_normals, edge_dihedrals, winding_conflicts  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("src", nargs="?", default="")
    ap.add_argument("--cell", type=float, default=1.0, help="cell size in mean output edges")
    ap.add_argument("--srccell", type=float, default=1.0, help="cell size in mean source edges")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    V, po = load_polys(a.out)
    N = face_normals(V, po)
    C = np.array([V[idx].mean(axis=0) for idx in po])
    el = np.array([np.mean([np.linalg.norm(V[idx[(i + 1) % len(idx)]] - V[idx[i]])
                           for i in range(len(idx))]) for idx in po])
    mean_e = float(el.mean())
    verts_of = [set(int(v) for v in idx) for idx in po]

    if not a.src:
        # self-cell multiplicity on the output alone (cheap, no source needed)
        cell = a.cell * mean_e
        keys = np.floor(C / cell).astype(np.int64)
        cnt = Counter(map(tuple, keys))
        multi = sum(1 for k, v in cnt.items() if v >= 2)
        res = {"out": a.out, "polys": len(po), "mean_edge": mean_e,
               "output_cell_multiplicity_cells_ge2": multi,
               "winding_conflicts": winding_conflicts(po)}
        print(json.dumps(res) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))
        return

    Vs, ps = load_polys(a.src)
    Ns = face_normals(Vs, ps)
    Cs = np.array([Vs[idx].mean(axis=0) for idx in ps])
    sel = np.array([np.mean([np.linalg.norm(Vs[idx[(i + 1) % len(idx)]] - Vs[idx[i]])
                             for i in range(len(idx))]) for idx in ps])
    kd = cKDTree(Cs)
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))

    # each output face -> nearest source point -> source cell
    src_cell = a.srccell * float(np.median(sel))
    d, nn = kd.query(C)
    sc = np.floor(Cs[nn] / src_cell).astype(np.int64)
    buckets = defaultdict(list)
    for i, k in enumerate(map(tuple, sc)):
        buckets[k].append(i)

    double_pairs, multi_cells = [], 0
    for k, fl in buckets.items():
        if len(fl) < 2:
            continue
        # count only pairs from DIFFERENT bands (no shared vertex)
        flagged = False
        for x in range(len(fl)):
            for y in range(x + 1, len(fl)):
                i, j = fl[x], fl[y]
                if verts_of[i] & verts_of[j]:
                    continue
                if abs(N[i] @ N[j]) < 0.6:
                    continue      # crossing sheets, not a doubled band: rarer, still counted below
                double_pairs.append((int(i), int(j), float(d[i]), float(d[j])))
                flagged = True
        if flagged:
            multi_cells += 1

    faces = sorted({p for i, j, _, _ in double_pairs for p in (i, j)})

    res = {
        "out": a.out, "src": a.src, "polys": len(po),
        "mean_out_edge": mean_e, "src_cell": src_cell,
        "double_covered_cells": multi_cells,
        "double_covered_pairs": len(double_pairs),
        "double_covered_faces": len(faces),
        "double_covered_pct_of_faces": 100.0 * len(faces) / max(1, len(po)),
        "winding_conflicts": winding_conflicts(po),
        "src_winding_conflicts": winding_conflicts(ps),
    }

    # BUG-01 correlation: target mask = creased output (quads) x locally smooth source
    maxd_o, _ = edge_dihedrals(V, po, N)
    maxd_s, _ = edge_dihedrals(Vs, ps, Ns)
    R = 0.01 * diag
    isq = np.array([len(p) == 4 for p in po])
    cre = np.where((maxd_o > 120.0) & isq)[0]
    tset = set()
    for fi in cre:
        n = kd.query_ball_point(C[fi], r=R)
        if n and maxd_s[n].max() < 30.0:
            tset.add(int(fi))
    inter = len(tset & set(faces))
    res["bug01_target_faces"] = len(tset)
    res["targets_double_covered"] = inter
    res["targets_double_covered_pct"] = 100.0 * inter / max(1, len(tset))

    print(json.dumps(res) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))
    if not a.json and double_pairs:
        print(f"  sample (faceA,faceB,out2src A,B): {double_pairs[:12]}")


if __name__ == "__main__":
    sys.exit(main())
