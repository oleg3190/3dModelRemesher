#!/usr/bin/env python3
"""Clean artificial cases for the fold (pleat) investigation - S44.

Why these three: the fold mechanism has to be separated from AI-mesh noise, and the T12 theory
said "super-elongated patches fold". On a generated rail 10:1:1 (perfectly watertight, zero
noise) the pleat rate is 0.00%, on a cube 0.13%, on a sphere 0.73% and on the AI mesh 5.09% -
so the trigger is curvature versus cell size, not patch elongation.

Usage: gen_cases.py [outdir]     -> writes rail10.obj, cube.obj
"""
import os
import sys

import numpy as np


def box(L, W, H, nl, nw, nh):
    V, F = [], []

    def grid(a, b, fn):
        idx = {}
        for i in range(a + 1):
            for j in range(b + 1):
                idx[(i, j)] = len(V)
                V.append(fn(i / a, j / b))
        for i in range(a):
            for j in range(b):
                F.append([idx[(i, j)], idx[(i + 1, j)], idx[(i + 1, j + 1)], idx[(i, j + 1)]])

    grid(nl, nw, lambda u, v: (u * L, v * W, 0.0))
    grid(nl, nw, lambda u, v: (u * L, (1 - v) * W, H))
    grid(nl, nh, lambda u, v: (u * L, 0.0, v * H))
    grid(nl, nh, lambda u, v: (u * L, W, (1 - v) * H))
    grid(nw, nh, lambda u, v: (0.0, u * W, v * H))
    grid(nw, nh, lambda u, v: (L, (1 - u) * W, v * H))

    V = np.asarray(V, dtype=float)
    key = np.round(V, 7)
    uniq, remap = {}, []
    for k in map(tuple, key):
        if k not in uniq:
            uniq[k] = len(uniq)
        remap.append(uniq[k])
    NV = np.zeros((len(uniq), 3))
    for i, k in enumerate(map(tuple, key)):
        NV[uniq[k]] = V[i]
    return NV, [[remap[x] for x in f] for f in F]


def write(path, V, F):
    with open(path, "w") as fh:
        for p in V:
            fh.write("v %.7f %.7f %.7f\n" % tuple(p))
        for f in F:
            fh.write("f %d %d %d %d\n" % tuple(x + 1 for x in f))


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else "test_out"
    os.makedirs(out, exist_ok=True)
    for name, (L, W, H, n) in {
        "rail10": (1.0, 0.1, 0.1, 40),   # 10:1:1 elongation, flat faces
        "cube": (0.5, 0.5, 0.5, 10),     # control: same face count scale, no elongation
    }.items():
        a = int(n)
        b = max(2, round(n * W / L))
        c = max(2, round(n * H / L))
        V, F = box(L, W, H, a, b, c)
        p = os.path.join(out, f"{name}.obj")
        write(p, V, F)
        print(f"{p}: verts={len(V)} quads={len(F)}")


if __name__ == "__main__":
    main()
