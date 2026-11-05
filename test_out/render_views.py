#!/usr/bin/env python3
"""Render source + remeshed meshes (3 views) with matplotlib for visual QA."""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import trimesh

paths = sys.argv[1:-1]
out_png = sys.argv[-1]

VIEWS = [(20, -60, "front-iso"), (20, 30, "side-iso"), (89, -90, "top")]
fig = plt.figure(figsize=(6 * len(paths), 5.5 * 3), dpi=110)

for col, path in enumerate(paths):
    m = trimesh.load(path, process=True)
    if isinstance(m, trimesh.Scene):
        m = m.to_mesh()
    tris = m.vertices[m.faces]
    # lambert shading from a fixed light
    n = m.face_normals
    L = np.array([0.4, -0.6, 0.7]); L = L / np.linalg.norm(L)
    lam = np.clip(np.abs(n @ L), 0, 1) * 0.75 + 0.25
    base = np.array([0.45, 0.62, 0.85])
    colors = np.clip(base[None, :] * lam[:, None], 0, 1)
    # component colouring for the first (source) column? -> separate handling below
    comps = m.split(only_watertight=False)
    comp_info = f"components={len(comps)}"
    for row, (el, az, name) in enumerate(VIEWS):
        ax = fig.add_subplot(len(paths), 3, row * len(paths) + col + 1, projection="3d")
        pc = Poly3DCollection(tris, facecolors=colors, edgecolors="none")
        ax.add_collection3d(pc)
        b = m.bounds
        c = b.mean(0); r = (b[1] - b[0]).max() / 2
        ax.set_xlim(c[0] - r, c[0] + r); ax.set_ylim(c[1] - r, c[1] + r); ax.set_zlim(c[2] - r, c[2] + r)
        ax.view_init(elev=el, azim=az)
        ax.set_axis_off()
        ax.set_title(f"{path.split('/')[-1]} | {name}\n{len(m.vertices)}v {len(m.faces)}f {comp_info}",
                     fontsize=9)
plt.tight_layout()
plt.savefig(out_png)
print("saved", out_png)
