#!/usr/bin/env python3
"""Per-face deviation analysis: how far do FACE INTERIORS drift from the source surface.

Usage: face_dev.py <source.obj> <mesh.obj> [...]

Reports distribution of centroid deviation and worst-face diagnostics
(edge length, aspect, whether the face bridges distant source regions).
"""
import sys
import numpy as np
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


def main():
    src = load(sys.argv[1])
    diag = float(np.linalg.norm(src.extents))
    q = trimesh.proximity.ProximityQuery(src)
    print(f"SOURCE area={src.area:.4f} diag={diag:.4f}")

    for path in sys.argv[2:]:
        m = load(path)
        cent = m.triangles.mean(axis=1)
        _, d, _ = q.on_surface(cent)
        dp = 100 * d / diag
        areas = m.area_faces
        # edge lengths per triangle
        e0 = np.linalg.norm(m.triangles[:, 1] - m.triangles[:, 0], axis=1)
        e1 = np.linalg.norm(m.triangles[:, 2] - m.triangles[:, 1], axis=1)
        e2 = np.linalg.norm(m.triangles[:, 0] - m.triangles[:, 2], axis=1)
        emax = np.maximum(np.maximum(e0, e1), e2)
        print(f"\n{path}: tris={len(m.faces)} area={m.area:.4f} ({100*m.area/src.area:.1f}% src)")
        print(f"  centroid dev %diag: mean={dp.mean():.4f} rms={np.sqrt((dp**2).mean()):.4f} "
              f"p50={np.percentile(dp,50):.4f} p90={np.percentile(dp,90):.4f} "
              f"p99={np.percentile(dp,99):.4f} max={dp.max():.4f}")
        for thr in (0.25, 0.5, 1.0, 2.0):
            sel = dp > thr
            print(f"  faces with centroid dev > {thr}% diag: {sel.sum()} "
                  f"({100*sel.mean():.2f}%)  their area share="
                  f"{100*areas[sel].sum()/max(areas.sum(),1e-12):.2f}%")
        worst = np.argsort(dp)[-5:][::-1]
        print("  worst faces (max_edge, dev%): " +
              ", ".join(f"({emax[i]:.4f},{dp[i]:.2f})" for i in worst))
        print(f"  edge length: p50={np.percentile(emax,50):.4f} p95={np.percentile(emax,95):.4f} "
              f"max={emax.max():.4f}")


if __name__ == "__main__":
    main()
