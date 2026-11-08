#!/usr/bin/env python3
"""Can a broken band be deleted and re-filled with a CLEAN QUAD grid? (user's idea, measured)

The refill is legal all-quad only if the hole border is a single even-length cycle whose four
sides can be paired (g0 == g2, g1 == g3) and whose corners are real mesh corners (otherwise the
new grid is a twisted fan, not a plane). So measure, per broken component:
  - diskness (one closed cycle, chi = 1)
  - border parity (even/odd)
  - whether corners exist at all (turn angle > 35 deg)
  - what happens after growing the hole by 1..3 face rings (does parity/diskness become OK?)
and report how many BUG-01 faces such a repair could actually cover.
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


def border_cycle(faces, po, e2f):
    """Return the ordered boundary vertex cycle of a face set, or None if not a single cycle."""
    fs = set(faces)
    directed = []
    for fi in fs:
        idx = po[fi]
        n = len(idx)
        for i in range(n):
            x, y = idx[i], idx[(i + 1) % n]
            neigh = [f for f, _ in e2f[(min(x, y), max(x, y))] if f in fs and f != fi]
            if not neigh:
                directed.append((x, y))
    nxt = {}
    for x, y in directed:
        if x in nxt:
            return None
        nxt[x] = y
    if len(nxt) != len(directed):
        return None
    start = directed[0][0]
    path, cur = [], start
    for _ in range(len(directed) + 1):
        path.append(cur)
        cur = nxt.get(cur)
        if cur is None:
            return None
        if cur == start:
            return path
    return None


def chi(faces, po):
    """Euler characteristic of a face set (V - E + F); 1 == disk."""
    vs, es = set(), set()
    for fi in faces:
        idx = po[fi]
        n = len(idx)
        for i in range(n):
            vs.add(idx[i])
            x, y = idx[i], idx[(i + 1) % n]
            es.add((min(x, y), max(x, y)))
    return len(vs) - len(es) + len(faces)


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
    Cs = np.array([Vs[idx].mean(axis=0) for idx in ps])
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))
    R = a.rel * diag
    kd = cKDTree(Cs)

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
    for fi in list(broken):
        for g in adj[fi]:
            if maxd[g] > a.out_dih and isq[g]:
                broken.add(int(g))

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

    def grow(comp, rings):
        cur = set(comp)
        for _ in range(rings):
            nxt = set(cur)
            for f in cur:
                nxt |= adj[f]
            cur = nxt
        return sorted(cur)

    rows = []
    for comp in comps:
        for rings in (0, 1, 2, 3):
            faces = comp if rings == 0 else grow(comp, rings)
            cyc = border_cycle(faces, po, e2f)
            if cyc is None:
                rows.append((rings, len(faces), -1, False, False))
                continue
            L = len(cyc)
            disk = (chi(faces, po) == 1)
            rows.append((rings, len(faces), L, L % 2 == 0, disk))

    # per-component verdict: smallest ring growth that gives disk + even cycle
    fixable = {}
    for comp in comps:
        n0 = len(comp)
        best = None
        for rings in (0, 1, 2, 3):
            faces = comp if rings == 0 else grow(comp, rings)
            cyc = border_cycle(faces, po, e2f)
            if cyc is None:
                continue
            if len(cyc) % 2 == 0 and chi(faces, po) == 1 and len(cyc) >= 8:
                best = (rings, len(faces), len(cyc))
                break
        fixable[n0] = best

    total_broken = len(broken)
    can = [b for b in fixable.values() if b]
    res = {
        "out": a.out,
        "broken_faces": total_broken,
        "components": len(comps),
        "components_refillable": len(can),
        "faces_covered_by_refillable": sum(k for k, v in fixable.items() if v),
        "pct_broken_covered": 100.0 * sum(k for k, v in fixable.items() if v) / max(1, total_broken),
        "growth_needed": sorted(v[0] for v in can),
        "hole_face_counts": sorted(v[1] for v in can),
        "border_lengths": sorted(v[2] for v in can),
        "median_cell": med_e,
    }
    print(json.dumps(res, indent=1) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))


if __name__ == "__main__":
    sys.exit(main())
