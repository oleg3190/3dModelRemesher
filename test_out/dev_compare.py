#!/usr/bin/env python3
"""Compare deviation of several meshes against the source mesh.

Usage: dev_compare.py <source.obj> <mesh1.obj> [mesh2.obj ...]

For each mesh reports:
  - vertex deviation (mesh vertices -> source surface): mean/RMS/p95/p99/max
  - surface-sample deviation both ways (chamfer / coverage)
  - area ratio
All values in absolute units and % of bbox diagonal.
"""
import sys
import numpy as np
import trimesh

N_SAMPLES = 60000


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


def stats(d, diag, tag):
    print(f"    {tag}: mean={100*d.mean()/diag:.4f}% rms={100*np.sqrt((d**2).mean())/diag:.4f}% "
          f"p95={100*np.percentile(d,95)/diag:.4f}% p99={100*np.percentile(d,99)/diag:.4f}% "
          f"max={100*d.max()/diag:.4f}% (abs max={d.max():.5f})")


def main():
    src_path = sys.argv[1]
    src = load(src_path)
    diag = float(np.linalg.norm(src.extents))
    src_q = trimesh.proximity.ProximityQuery(src)
    print(f"SOURCE {src_path}: area={src.area:.4f} diag={diag:.4f}")

    for path in sys.argv[2:]:
        m = load(path)
        print(f"\n{path}: {len(m.vertices)}v {len(m.faces)}f area={m.area:.4f} "
              f"({100*m.area/src.area:.1f}% of src)")
        # 1) deviation of the mesh's own vertices from the source surface
        _, d, _ = src_q.on_surface(m.vertices)
        stats(d, diag, "verts->src")
        # 2) sampled both ways
        pts, _ = trimesh.sample.sample_surface(m, N_SAMPLES)
        _, d1, _ = src_q.on_surface(pts)
        stats(d1, diag, "surface->src")
        q2 = trimesh.proximity.ProximityQuery(m)
        pts2, _ = trimesh.sample.sample_surface(src, N_SAMPLES)
        _, d2, _ = q2.on_surface(pts2)
        stats(d2, diag, "src->surface")
        # worst vertices for localisation
        worst = np.argsort(d)[-5:][::-1]
        print("    worst verts (x,y,z,dev): " +
              ", ".join(f"({m.vertices[i][0]:.3f},{m.vertices[i][1]:.3f},"
                        f"{m.vertices[i][2]:.3f},{d[i]:.4f})" for i in worst))


if __name__ == "__main__":
    main()
