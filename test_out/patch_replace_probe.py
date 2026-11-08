#!/usr/bin/env python3
"""Feasibility probe for the user's idea: DELETE the broken band and RE-FILL it as a clean grid.

A quad-only refill of a hole is legal ONLY if the hole boundary is a single simple cycle that is a
"grid rectangle": 4 sides, with opposite sides holding the same number of vertices (that is the
same parity/equal-side condition the extractor already demands, invariants #4/#5). Otherwise the
refill would need T-junctions or triangles.

So before writing any C++ pass, measure:
  - what fraction of the broken regions (smooth-source creases / same-side pleats) form such a
    rectangle, how big they are, and how many of the BUG-01 faces they cover;
  - how planar the source really is under each candidate (the user's premise: take the plane,
    orientation and positions from the source);
  - the best-fit plane residual (RMS of border vertices to the plane) in units of the cell size.

Usage: patch_replace_probe.py OUT.obj SRC.obj [--out-dih 120] [--src-gate 30] [--rel 0.01] [--json]
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
    ap.add_argument("src")
    ap.add_argument("--out-dih", type=float, default=120.0)
    ap.add_argument("--src-gate", type=float, default=30.0)
    ap.add_argument("--rel", type=float, default=0.01)
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    V, po = load_polys(a.out)
    N = face_normals(V, po)
    maxd, _ = edge_dihedrals(V, po, N)
    C = np.array([V[i].mean(axis=0) for i in po])

    Vs, ps = load_polys(a.src)
    Ns = face_normals(Vs, ps)
    maxd_s, _ = edge_dihedrals(Vs, ps, Ns)
    Cs = np.array([Vs[i].mean(axis=0) for i in ps])
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))
    R = a.rel * diag
    kd = cKDTree(Cs)

    # adjacency + shared-edge geometry
    e2f = defaultdict(list)
    for fi, idx in enumerate(po):
        n = len(idx)
        for i in range(n):
            x, y = idx[i], idx[(i + 1) % n]
            e2f[(min(x, y), max(x, y))].append((fi, (x, y)))
    adj = defaultdict(set)
    for k, fs in e2f.items():
        for f, _ in fs:
            for g, _ in fs:
                if g != f:
                    adj[f].add(g)

    isq = np.array([len(i) == 4 for i in po])
    broken = set()
    for fi in np.where((maxd > a.out_dih) & isq)[0]:
        nn = kd.query_ball_point(C[fi], r=R)
        if nn and maxd_s[nn].max() < a.src_gate:
            broken.add(int(fi))
    # also pull in the fold partners (the neighbour the flap lies on top of)
    for fi in list(broken):
        for g in adj[fi]:
            if maxd[g] > a.out_dih and isq[g]:
                broken.add(int(g))

    # connected components of broken faces
    seen, comps = set(), []
    for s in sorted(broken):
        if s in seen:
            continue
        stack, comp = [s], []
        seen.add(s)
        while stack:
            f = stack.pop()
            comp.append(f)
            for g in adj[f]:
                if g in broken and g not in seen:
                    seen.add(g)
                    stack.append(g)
        comps.append(sorted(comp))

    med_e = float(np.median([np.linalg.norm(V[i[(k + 1) % len(i)]] - V[i[k]])
                             for i in po for k in range(len(i))]))

    stats = []
    for comp in comps:
        cs = set(comp)
        border = []           # directed boundary edges (face-side only)
        for fi in comp:
            idx = po[fi]
            n = len(idx)
            for i in range(n):
                x, y = idx[i], idx[(i + 1) % n]
                fs = [f for f, _ in e2f[(min(x, y), max(x, y))]]
                if not any(g in cs and g != fi for g in fs):
                    border.append((x, y))
        # walk the cycle
        nxt = dict(border)
        ok_cycle = len(border) == len(set(b for b, _ in border)) and len(nxt) == len(border)
        if not ok_cycle:
            stats.append({"faces": len(comp), "border": len(border), "cycle": False})
            continue
        start = border[0][0]
        path, cur, guard = [start], start, 0
        while cur in nxt and guard < len(border) + 2:
            guard += 1
            cur = nxt[cur]
            if cur == start:
                break
            path.append(cur)
        closed = (cur == start)
        n_b = len(path)
        # 4-corner split by turn angle on the plane of the region
        P = V[path]
        cen = P.mean(axis=0)
        u, s, vt = np.linalg.svd(P - cen)
        plane_n = vt[2]
        A2 = (P - cen) @ np.vstack([vt[0], vt[1]]).T
        turn = []
        for i in range(n_b):
            p0, p1, p2 = A2[i], A2[(i + 1) % n_b], A2[(i + 2) % n_b]
            v1, v2 = p1 - p0, p2 - p1
            cr = v1[0] * v2[1] - v1[1] * v2[0]
            dt = v1 @ v2
            turn.append(np.degrees(np.arctan2(cr, dt)) if np.linalg.norm(v1) > 1e-12 and np.linalg.norm(v2) > 1e-12 else 0.0)
        turn = np.abs(np.array(turn))
        corners = [int(i) for i in np.where(turn > 35.0)[0]]
        # side lengths in vertices between corners
        if len(corners) == 4 and closed:
            idxs = corners
            lens = []
            for i in range(4):
                a0, a1 = idxs[i], idxs[(i + 1) % 4]
                lens.append(((a1 - a0) % n_b) + 1)
            rect = (lens[0] == lens[2]) and (lens[1] == lens[3])
        else:
            lens, rect = [], False
        # planarity of the source under the region (user premise)
        rms = float(np.std((P - cen) @ plane_n))
        stats.append({"faces": len(comp), "border": n_b, "cycle": True, "closed": closed,
                      "corners": len(corners), "sides": lens, "rectangle": rect,
                      "plane_rms_over_cell": rms / med_e})

    tot = len(stats)
    cyc = [x for x in stats if x.get("cycle") and x.get("closed")]
    rec = [x for x in cyc if x.get("rectangle")]
    res = {
        "out": a.out, "broken_faces": len(broken), "components": tot,
        "simple_cycles": len(cyc),
        "grid_rectangles": len(rec),
        "faces_in_rectangles": sum(x["faces"] for x in rec),
        "pct_broken_in_rectangles": 100.0 * sum(x["faces"] for x in rec) / max(1, len(broken)),
        "median_cell": med_e,
        "rect_sizes": sorted((x["faces"] for x in rec), reverse=True)[:12],
        "rect_sides": [x["sides"] for x in rec][:8],
        "plane_rms_over_cell_median": (float(np.median([x["plane_rms_over_cell"] for x in rec]))
                                        if rec else None),
    }
    print(json.dumps(res, indent=1) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))


if __name__ == "__main__":
    sys.exit(main())
