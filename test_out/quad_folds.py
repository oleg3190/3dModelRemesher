#!/usr/bin/env python3
"""Fold/quality analysis of QUADS using the same scaled-Jacobian convention as stage6.

Usage: quad_folds.py mesh.obj [mesh2.obj ...]
Also reports edge length distribution of quads.
"""
import sys
import numpy as np
from collections import Counter


def load_polys(path):
    verts, polys = [], []
    with open(path) as fh:
        for line in fh:
            if line.startswith("v "):
                p = line.split()
                verts.append((float(p[1]), float(p[2]), float(p[3])))
            elif line.startswith("f "):
                idx = [int(t.split("/")[0]) - 1 for t in line.split()[1:]]
                polys.append(idx)
    return np.asarray(verts), polys


def main():
    for path in sys.argv[1:]:
        V, polys = load_polys(path)
        quads = [p for p in polys if len(p) == 4]
        if not quads:
            print(f"{path}: no quads")
            continue
        Q = V[np.asarray(quads)]  # (n,4,3)
        p0, p1, p2, p3 = Q[:, 0], Q[:, 1], Q[:, 2], Q[:, 3]
        nvec = np.cross(p2 - p0, p3 - p1)
        ln = np.linalg.norm(nvec, axis=1)
        valid = ln > 1e-8
        nvec = nvec / np.where(ln[:, None] == 0, 1.0, ln[:, None])
        sj = np.full(len(Q), 1.0)   # sentinel 1.0 (like stage6); invalid corners inject -1
        for i in range(4):
            e1 = Q[:, (i + 1) % 4] - Q[:, i]
            e2 = Q[:, (i + 3) % 4] - Q[:, i]
            l1 = np.linalg.norm(e1, axis=1)
            l2 = np.linalg.norm(e2, axis=1)
            ok = valid & (l1 > 1e-8) & (l2 > 1e-8)
            e1n = e1 / np.where(l1[:, None] == 0, 1.0, l1[:, None])
            e2n = e2 / np.where(l2[:, None] == 0, 1.0, l2[:, None])
            j = np.full(len(Q), -1.0)
            vals = np.einsum('ij,ij->i', np.cross(e1n, e2n), nvec)
            j[ok] = vals[ok]
            sj = np.minimum(sj, j)
        # edge lengths of quads
        el = np.stack([np.linalg.norm(Q[:, (i + 1) % 4] - Q[:, i], axis=1) for i in range(4)])
        emax = el.max(0)
        emin = el.min(0)
        ar = emax / np.where(emin == 0, 1e-30, emin)
        print(f"\n{path}: quads={len(quads)}")
        print(f"  scaled jacobian (stage6 convention): min={sj.min():.4f} "
              f"p01={np.percentile(sj,1):.4f} p05={np.percentile(sj,5):.4f} "
              f"p50={np.percentile(sj,50):.4f}")
        for thr in (-0.0, 0.1, 0.2, 0.5):
            c = (sj < thr).sum()
            print(f"  quads with SJ < {thr}: {c} ({100*c/len(quads):.2f}%)")
        print(f"  quad edge: p50={np.percentile(emax,50):.4f} p95={np.percentile(emax,95):.4f} "
              f"max={emax.max():.4f} | aspect p95={np.percentile(ar,95):.2f} "
              f"max={ar.max():.1f}")
        # degenerate: zero area quads
        print(f"  quad normal |n|: p50={np.percentile(ln,50):.6f} min={ln.min():.3e}")
        print(f"  counter(CornerSums): sj<0.2 share={100*(sj<0.2).mean():.2f}%")


if __name__ == "__main__":
    main()
