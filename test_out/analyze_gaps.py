#!/usr/bin/env python3
"""Gap analysis for open (boundary) edges of a polygon soup OBJ.

For every boundary edge we find the nearest boundary edge that does not share a vertex
and report the distance distribution. Small distances => borders are geometrically
coincident but not index-welded; large distances => real holes / missing surface.
Also reports how many vertices are not referenced by any polygon.
"""
import sys
import numpy as np
from collections import defaultdict
from scipy.spatial import cKDTree

path = sys.argv[1]
verts, faces = [], []
with open(path) as fh:
    for line in fh:
        if line.startswith("v "):
            p = line.split()
            verts.append((float(p[1]), float(p[2]), float(p[3])))
        elif line.startswith("f "):
            faces.append([int(t.split("/")[0]) - 1 for t in line.split()[1:]])
V = np.asarray(verts)
print(f"v lines={len(V)} polys={len(faces)}")

used = set()
edge_faces = defaultdict(list)
for fi, f in enumerate(faces):
    used.update(f)
    for i in range(len(f)):
        a, b = f[i], f[(i + 1) % len(f)]
        if a != b:
            edge_faces[(min(a, b), max(a, b))].append(fi)
print(f"referenced vertices={len(used)}  unreferenced={len(V) - len(used)}")

bnd = [e for e, fl in edge_faces.items() if len(fl) == 1]
print(f"boundary edges={len(bnd)}")

mid = np.array([(V[a] + V[b]) * 0.5 for a, b in bnd])
tree = cKDTree(mid)
# nearest boundary edge not sharing a vertex with the query
d1, i1 = tree.query(mid, k=8)
res = []
for k in range(len(bnd)):
    a, b = bnd[k]
    for j in range(1, 8):
        idx = i1[k, j]
        c, d = bnd[idx]
        if a not in (c, d) and b not in (c, d):
            res.append(d1[k, j])
            break
res = np.array(res)
if len(res):
    print("gap to nearest non-adjacent boundary edge (absolute / %diag=2.1567):")
    for q in (10, 25, 50, 75, 90, 99):
        print(f"  p{q}: {np.percentile(res, q):.6f}  ({100*np.percentile(res, q)/2.1567:.4f}%)")
    print(f"  max: {res.max():.6f}  mean={res.mean():.6f}")
    print(f"  fraction < 1e-4: {(res < 1e-4).mean()*100:.1f}%   < 1e-3: {(res < 1e-3).mean()*100:.1f}%"
          f"   < 0.01: {(res < 0.01).mean()*100:.1f}%   >= 0.01: {(res >= 0.01).mean()*100:.1f}%")
