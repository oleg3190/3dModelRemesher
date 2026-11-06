#!/usr/bin/env python3
"""Heatmap of per-face deviation from the source surface (3 views).

Usage: heatmap.py <source.obj> <mesh.obj> <out.png>
"""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import trimesh


def load(path):
    verts, faces = [], []
    with open(path) as fh:
        for line in fh:
            if line.startswith("v "):
                p = line.split()
                verts.append((float(p[1]), float(p[2]), float(p[3])))
            elif line.startswith("f "):
                idx = [int(t.split("/")[0]) - 1 for t in line.split()[1:]]
                if len(idx) >= 3:
                    for i in range(1, len(idx) - 1):
                        faces.append((idx[0], idx[i], idx[i + 1]))
    return trimesh.Trimesh(np.asarray(verts), np.asarray(faces), process=False)


src = load(sys.argv[1])
mesh = load(sys.argv[2])
diag = float(np.linalg.norm(src.extents))
q = trimesh.proximity.ProximityQuery(src)
cent = mesh.triangles.mean(axis=1)
_, d, _ = q.on_surface(cent)
dp = 100 * d / diag

cmap = plt.get_cmap("inferno")
norm = plt.Normalize(vmin=0.0, vmax=max(1.0, float(np.percentile(dp, 99))))
cols = cmap(norm(dp))[:, :3]
# shade by normal for readability
n = mesh.face_normals
L = np.array([0.4, -0.6, 0.7]); L /= np.linalg.norm(L)
shade = (np.clip(n @ L, 0, 1) * 0.4 + 0.6)
cols = np.clip(cols * shade[:, None], 0, 1)

VIEWS = [(20, -60, "front-iso"), (20, 30, "side-iso"), (89, -90, "top")]
fig = plt.figure(figsize=(6 * 3, 6), dpi=130)
for r, (el, az, name) in enumerate(VIEWS):
    ax = fig.add_subplot(1, 3, r + 1, projection="3d")
    ax.add_collection3d(Poly3DCollection(mesh.triangles, facecolors=cols, edgecolors="none"))
    b = mesh.bounds
    c = b.mean(0); rr = (b[1] - b[0]).max() / 2
    ax.set_xlim(c[0] - rr, c[0] + rr); ax.set_ylim(c[1] - rr, c[1] + rr); ax.set_zlim(c[2] - rr, c[2] + rr)
    ax.view_init(elev=el, azim=az)
    ax.set_axis_off()
    ax.set_title(f"{name}\nfaces dev %: p50={np.percentile(dp,50):.3f} "
                 f"p90={np.percentile(dp,90):.3f} p99={np.percentile(dp,99):.3f}", fontsize=10)
fig.suptitle(f"{sys.argv[2]}  |  colour = face centroid deviation, max(0..1% of diag)", fontsize=11)
plt.tight_layout()
plt.savefig(sys.argv[3])
print("saved", sys.argv[3], "dev stats:", {f"p{p}": round(float(np.percentile(dp, p)), 4)
                                           for p in (50, 90, 99, 100)})
