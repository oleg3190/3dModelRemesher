#!/usr/bin/env python3
"""Structure-first probe (user's idea): build OUR OWN grid from the model's faces/creases instead
of repairing the AI mesh. Before touching the pipeline, measure the three things that decide
whether this is worth it and how many polygons it saves:

  1. PLANAR STRUCTURE - does the source decompose into few planar regions? Region growing on face
     normals (BFS over face adjacency, merge while the normal deviates less than --theta and the
     face centroid stays within --sag of the growing plane). Report region count, area share of
     the big regions, whether a region is a disk (chi = 1, so it CAN be one quad patch), and the
     mean in-plane residual.
  2. CREASE GRAPH - the edges between regions: count + dihedral histogram = the feature lines a
     feature-aligned grid would follow (what stage1 tries to do from noisy curvature).
  3. OPTIMAL BUDGET - if cell size were bounded by normal deviation (h <= 2*r*sin(dn/2), r from a
     DENOISED local curvature estimate) instead of a uniform target, how many quads does THIS
     model actually need? Compare with the current 9184 and with the resolution sweep.

Usage: structure_probe.py SRC.obj [--theta 3] [--sag 0.0005] [--dn 15] [--json]
"""
import argparse
import json
import os
import sys
from collections import Counter, defaultdict

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from smooth_creases import load_polys, face_normals, edge_dihedrals  # noqa: E402


def adjacency(polys):
    e2f = defaultdict(list)
    for fi, idx in enumerate(polys):
        n = len(idx)
        for i in range(n):
            a, b = idx[i], idx[(i + 1) % n]
            e2f[(min(a, b), max(a, b))].append(fi)
    adj = defaultdict(list)
    for fs in e2f.values():
        for a in fs:
            for b in fs:
                if a != b:
                    adj[a].append(b)
    return e2f, adj


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--theta", type=float, default=3.0, help="max normal deviation, deg")
    ap.add_argument("--sag", type=float, default=0.0005, help="max off-plane distance, world units")
    ap.add_argument("--dn", type=float, default=15.0, help="target normal deviation per cell, deg")
    ap.add_argument("--json", action="store_true")
    a = ap.parse_args()

    V, polys = load_polys(a.src)
    N = face_normals(V, polys)
    e2f, adj = adjacency(polys)
    C = np.array([V[idx].mean(axis=0) for idx in polys])
    AR = np.array([0.5 * np.linalg.norm(np.cross(V[idx[1]] - V[idx[0]], V[idx[2]] - V[idx[0]]))
                   if len(idx) == 3 else
                   0.5 * np.linalg.norm(np.cross(V[idx[2]] - V[idx[0]], V[idx[3]] - V[idx[1]]))
                   for idx in polys])
    total_area = float(AR.sum())
    nf = len(polys)
    cos_t = np.cos(np.radians(a.theta))

    # ---- 1. region growing on normals + sag ----------------------------------------------------
    label = np.full(nf, -1, dtype=np.int64)
    regions = []
    for seed in range(nf):
        if label[seed] >= 0:
            continue
        rid = len(regions)
        label[seed] = rid
        n0 = N[seed]
        origin = C[seed]
        stack = [seed]
        members = [seed]
        while stack:
            f = stack.pop()
            for g in adj[f]:
                if label[g] >= 0:
                    continue
                # must stay within theta of the region normal AND within sag of its plane
                if N[g] @ n0 < cos_t:
                    continue
                if abs((C[g] - origin) @ n0) > a.sag:
                    continue
                label[g] = rid
                members.append(g)
                stack.append(g)
        regions.append(members)

    areas = np.array([float(AR[m].sum()) for m in regions])
    sizes = np.array([len(m) for m in regions])
    order = np.argsort(-areas)
    big = [r for r in order if areas[r] > 0.002 * total_area]
    cov_big = float(sum(areas[r] for r in big)) / total_area

    # diskness of the big regions (chi = V - E + F == 1)
    disk = 0
    residuals = []
    for r in big:
        m = regions[r]
        vs, es = set(), set()
        for fi in m:
            idx = polys[fi]
            n = len(idx)
            for i in range(n):
                vs.add(idx[i])
                x, y = idx[i], idx[(i + 1) % n]
                es.add((min(x, y), max(x, y)))
        if len(vs) - len(es) + len(m) == 1:
            disk += 1
        P = np.array([V[i] for i in vs])
        cen = P.mean(axis=0)
        u, s, vt = np.linalg.svd(P - cen, full_matrices=False)
        residuals.append(float(np.mean(np.abs((P - cen) @ vt[2]))))

    # ---- 2. crease graph between regions ------------------------------------------------------
    crease_dih = []
    crease_edges = 0
    for k, fs in e2f.items():
        if len(fs) != 2:
            continue
        f0, f1 = fs
        if label[f0] == label[f1]:
            continue
        d = np.degrees(np.arccos(np.clip(N[f0] @ N[f1], -1, 1)))
        crease_dih.append(d)
        crease_edges += 1
    crease_dih = np.array(crease_dih) if crease_dih else np.array([0.0])

    # ---- 3. optimal budget under a Delta-n bound ----------------------------------------------
    # Local curvature is estimated DENOISED (median normal deviation over the 2-ring), because the
    # per-vertex difference on an AI mesh is noise - that is exactly what makes the current
    # adaptive_density hurt (S44). Sampled: 1 face in stride, area weighted back.
    diag = float(np.linalg.norm(V.max(0) - V.min(0)))
    stride = max(1, nf // 12000)
    sample = list(range(0, nf, stride))
    kappa = np.zeros(nf)
    for f in sample:
        nb = set(adj[f])
        for g in adj[f]:
            nb |= set(adj[g])
        nb.discard(f)
        if len(nb) < 3:
            continue
        nl = list(nb)
        nn = N[nl]
        dev = np.degrees(np.arccos(np.clip(nn @ N[f], -1, 1)))
        dist = np.linalg.norm(C[nl] - C[f], axis=1)
        kappa[f] = float(np.median(dev)) / max(1e-9, float(np.median(dist)))
    r_curv = 1.0 / np.maximum(kappa, 1e-6)
    bound = 2.0 * r_curv * np.sin(np.radians(a.dn / 2.0))

    # a cell cannot exceed the planar region it lives in: cap by a fraction of the region extent
    reg_extent = {}
    for r in big:
        m = regions[r]
        P = np.array([V[i] for i in set(v for fi in m for v in polys[fi])])
        reg_extent[r] = float(np.linalg.norm(P.max(0) - P.min(0)))
    cell_cap = np.full(nf, 0.25 * diag)
    for r in big:
        e = max(0.25 * reg_extent.get(r, diag), 1e-6)
        for fi in regions[r]:
            cell_cap[fi] = min(cell_cap[fi], e)
    h = np.minimum(bound, cell_cap)
    h = np.maximum(h, 0.05 * diag)          # do not let perfectly flat zones demand infinite cells
    optimal = float(np.sum(AR[sample] / np.maximum(h[sample], 1e-9) ** 2)) * stride

    res = {
        "src": a.src, "faces": nf, "total_area": total_area,
        "regions": len(regions),
        "regions_gt_0.2%area": len(big),
        "area_share_of_those": cov_big,
        "regions_that_are_disks": disk,
        "mean_in_plane_residual": float(np.mean(residuals)) if residuals else None,
        "median_region_faces": float(np.median(sizes)),
        "largest_region_faces": int(sizes.max()),
        "crease_edges_between_regions": int(crease_edges),
        "crease_dih_p50_p90_max": [float(np.percentile(crease_dih, 50)),
                                   float(np.percentile(crease_dih, 90)),
                                   float(crease_dih.max())],
        "delta_n_deg": a.dn,
        "optimal_quads_under_dnbinding": int(optimal),
        "cell_size_median_over_diag": float(np.median(h) / diag),
        "cell_size_p10_over_diag": float(np.percentile(h, 10) / diag),
    }
    print(json.dumps(res, indent=1) if a.json else "\n".join(f"  {k}: {v}" for k, v in res.items()))


if __name__ == "__main__":
    sys.exit(main())
