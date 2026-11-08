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


def add_bridge(out):
    """A panel-dominant 'bridge': deck plate, pylon pair, arch (torus segment), thin rails.

    Overlapping shells are left as-is (no CSG): an AI mesh is self-intersecting too, so this is
    the same input regime. The point is the SHAPE STATISTICS - a genuinely planar-dominated model,
    which is where the shape-aware spec (T20) predicts our pipeline fails. Our current model has
    only 38% of area in trusted flat regions, so it cannot test that claim.
    """
    V, F = [], []

    def box(cx, cy, cz, sx, sy, sz, n=4):
        base = len(V)
        ix, iy, iz = n, n, n
        # six faces, each a grid
        def face(p, u, v, nu, nv):
            idx = {}
            for i in range(nu + 1):
                for j in range(nv + 1):
                    a, b = i / nu, j / nv
                    pt = [p[k] + u[k] * a + v[k] * b for k in range(3)]
                    idx[(i, j)] = len(V); V.append(pt)
            for i in range(nu):
                for j in range(nv):
                    F.append([idx[(i, j)], idx[(i + 1, j)], idx[(i + 1, j + 1)], idx[(i, j + 1)]])
        p = [cx - sx / 2, cy - sy / 2, cz - sz / 2]
        u = [sx, 0, 0]; v = [0, sy, 0]
        face(p, u, v, ix, iy)                              # bottom
        face([p[0], p[1], p[2] + sz], u, v, ix, iy)        # top
        face(p, [0, sy, 0], [0, 0, sz], iy, iz)            # -x side
        face([p[0] + sx, p[1], p[2]], [0, sy, 0], [0, 0, sz], iy, iz)
        face(p, [sx, 0, 0], [0, 0, sz], ix, iz)            # -y side
        face([p[0], p[1] + sy, p[2]], [sx, 0, 0], [0, 0, sz], ix, iz)

    def tube(cx, cz, r, a0, a1, ny=3, na=28, wall=0.02):
        """Curved band (arch) in the x-z plane, extruded along y, with a real wall thickness."""
        rings = []
        for i in range(ny + 1):
            y = -0.5 + i * (1.0 / ny)
            row_o, row_i = [], []
            for j in range(na + 1):
                t = a0 + (a1 - a0) * j / na
                row_o.append(len(V)); V.append([cx + r * np.cos(t), y, cz + r * np.sin(t)])
                row_i.append(len(V)); V.append([cx + (r - wall) * np.cos(t), y, cz + (r - wall) * np.sin(t)])
            rings.append((row_o, row_i))
        for i in range(ny):
            o0, i0 = rings[i]; o1, i1 = rings[i + 1]
            for j in range(na):
                # outer skin, inner skin, and both end caps of the wall
                F.append([o0[j], o1[j], o1[j + 1], o0[j + 1]])
                F.append([i1[j + 1], i1[j], i0[j], i0[j + 1]])
                F.append([o0[j + 1], o1[j + 1], i1[j + 1], i0[j + 1]])
                F.append([i1[j], o1[j], o0[j], i0[j]])

    box(0.0, 0.0, 0.55, 2.2, 0.5, 0.08, n=8)          # deck slab
    box(-0.9, 0.0, 0.0, 0.22, 0.44, 1.02, n=2)        # pylon left
    box(0.9, 0.0, 0.0, 0.22, 0.44, 1.02, n=2)         # pylon right
    for s in (-1, 1):                                   # two thin rails on the deck edges
        box(0.0, s * 0.24, 0.78, 2.2, 0.03, 0.36, n=8)
    for z in (0.62, 0.70, 0.78):                       # three thin deck-stringer plates
        box(0.0, 0.0, z, 2.1, 0.02, 0.03, n=8)
    tube(0.0, 0.15, 0.75, 0.25, np.pi - 0.25, ny=2, na=26)   # arch
    V = np.asarray(V, dtype=float)
    # weld coincident vertices, otherwise every box is six disconnected open sheets and the
    # case tests our handling of a broken input instead of a panel-dominant shape
    key = np.round(V, 7)
    uniq, remap = {}, []
    for k in map(tuple, key):
        if k not in uniq:
            uniq[k] = len(uniq)
        remap.append(uniq[k])
    NV = np.zeros((len(uniq), 3))
    for i, k in enumerate(map(tuple, key)):
        NV[uniq[k]] = V[i]
    NF = []
    for f in F:
        g = [remap[x] for x in f]
        while len(g) > 1 and g[0] == g[-1]:
            g.pop()
        g2 = []
        for x in g:
            if not g2 or g2[-1] != x:
                g2.append(x)
        if len(g2) >= 3 and len(set(g2)) == len(g2):
            NF.append(g2)
    write(out, NV, NF)


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else "test_out"
    os.makedirs(out, exist_ok=True)
    add_bridge(os.path.join(out, "bridge.obj"))
    print(f"{os.path.join(out,'bridge.obj')}: generated (panel-dominant test case, T20)")
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
