#!/usr/bin/env python3
"""Diagnose WHAT a smooth-region crease is (BUG-01 / T7) before choosing a repair primitive.

For each target face (creased quad x locally smooth source) report:
  fold       : max dihedral over its edges (deg)
  own_sj     : scaled jacobian of that quad (<0 => inverted)
  float/edge : distance of the quad centroid to the nearest SOURCE point / its edge length
               (~0 => the face already lies on the surface: the defect is tangential,
                not an out-of-plane bump)
  cover      : count of NON-adjacent output faces within 0.5*edge with |n.dot|>0.9
               (>0 => double-covered / folded-on-top-of-itself region)
  nbr_dih    : the quad's neighbour max dihedral - is the fold shared with a partner?
"""
import sys
import numpy as np
from collections import defaultdict
from scipy.spatial import cKDTree

sys.path.insert(0, "test_out")
from smooth_creases import load_polys, face_normals, edge_dihedrals, scaled_jacobian

OUT_DIH, SRC_GATE, REL = 120.0, 30.0, 0.01


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else "test_out/t7_base.obj"
    src = sys.argv[2] if len(sys.argv) > 2 else "test_out/input.obj"

    Vo, po = load_polys(out)
    No = face_normals(Vo, po)
    maxd_o, _ = edge_dihedrals(Vo, po, No)
    isq = np.array([len(p) == 4 for p in po])
    SJV = scaled_jacobian(Vo, po)
    sj = np.full(len(po), np.nan)
    qi = 0
    for fi, p in enumerate(po):
        if len(p) == 4:
            sj[fi] = SJV[qi]
            qi += 1

    e2f = defaultdict(list)
    for fi, idx in enumerate(po):
        n = len(idx)
        for i in range(n):
            a, b = idx[i], idx[(i + 1) % n]
            e2f[(min(a, b), max(a, b))].append(fi)
    fadj = defaultdict(set)
    for fs in e2f.values():
        for a in fs:
            for b in fs:
                if a != b:
                    fadj[a].add(b)

    Co = np.array([Vo[idx].mean(axis=0) for idx in po])
    el = np.array([np.mean([np.linalg.norm(Vo[idx[(i + 1) % len(idx)]] - Vo[idx[i]])
                            for i in range(len(idx))]) for idx in po])
    val = np.zeros(len(Vo), dtype=int)
    for idx in po:
        for v in idx:
            val[v] += 1

    Vs, ps = load_polys(src)
    Ns = face_normals(Vs, ps)
    maxd_s, _ = edge_dihedrals(Vs, ps, Ns)
    Cs = np.array([Vs[idx].mean(axis=0) for idx in ps])
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))
    R = REL * diag
    kd = cKDTree(Cs)
    kto = cKDTree(Co)

    creased = np.where((maxd_o > OUT_DIH) & isq)[0]
    rough = np.array([maxd_s[kd.query_ball_point(Co[fi], r=R)].max()
                      if len(kd.query_ball_point(Co[fi], r=R)) else 360.0 for fi in creased])
    targets = creased[rough < SRC_GATE]
    print(f"{out}: quads={int(isq.sum())} creased={len(creased)} targets={len(targets)} "
          f"R={R:.5f} mean_edge={el[isq].mean():.5f}")

    rows = []
    for fi in targets:
        d, _ = kd.query(Co[fi])
        cand = kto.query_ball_point(Co[fi], r=0.5 * el[fi])
        cov = sum(1 for c in cand if c != fi and c not in fadj[fi] and abs(No[c] @ No[fi]) > 0.9)
        partner = any(maxd_o[nb] > OUT_DIH for nb in fadj[fi])
        vs = po[fi]
        rows.append((fi, maxd_o[fi], sj[fi], d / el[fi], cov, partner,
                     tuple(int(val[v]) for v in vs)))
    rows.sort(key=lambda r: -r[1])

    arr = np.array([[r[1], r[2], r[3], r[4]] for r in rows]) if rows else np.zeros((0, 4))
    if len(arr):
        print(f"  fold>150: {(arr[:,0]>150).sum()}  own SJ<0: {(arr[:,1]<0).sum()}  "
              f"float>0.3edge: {(arr[:,2]>0.3).sum()}  double-covered: {(arr[:,3]>0).sum()}")
        print(f"  medians: fold={np.median(arr[:,0]):.1f} sj={np.median(arr[:,1]):.3f} "
              f"float/edge={np.median(arr[:,2]):.3f}")
    print("  face  fold   sj  float/e cover partner_valences")
    for r in rows[:20]:
        print(f"  {r[0]:5d} {r[1]:5.1f} {r[2]:6.2f} {r[3]:8.2f} {r[4]:5d}   {str(r[5]):5s} {r[6]}")
    # how many target faces have ALL corners valence 4? (the movability precondition)
    allv4 = sum(1 for r in rows if all(x == 4 for x in r[6]))
    print(f"  targets with all-4-valence corners: {allv4}/{len(rows)}")


if __name__ == "__main__":
    main()
