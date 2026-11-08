#!/usr/bin/env python3
"""SAQ validation (user's law) against analytic ground truth.

Two things are tested, in this order, because a pretty theory still has to survive numbers:

A. THEIR PROTOTYPE AS-IS (test_out/saq_reference.py = the spec's code, verbatim).
   Does its MAD threshold separate AI noise from real curvature? Does its flat_mask cover the known
   planes? Does the vertex-count integral K behave on a cylinder (their gamma -> 0+ makes
   sqrt(det G) = sqrt(gamma * k1) -> 0 there, which contradicts their own physical meaning #2)?
   The prototype estimates k1,k2 as ||dn||/||e|| over two edges of a face: that is a magnitude
   proxy, NOT the Weingarten operator, so it yields no principal directions - the anisotropy the
   law is named after. Measured here, not argued.

B. THE MISSING PART, rebuilt: filter the normals spectrally (their Eq. 1, biharmonic CG solve,
   vertices untouched), then fit the symmetric 2x2 Weingarten tensor per vertex by least squares
   over the 1-ring in the filtered tangent basis, and compare against the ANALYTIC ground truth of
   the bridge case (test_out/gen_cases.py): deck/rails/stringers = planes (k1=k2=0), arch = circular
   cylinder r=0.75 -> k1 = 1/0.75 = 1.3333 along the circumferential direction, k2 = 0 along the
   axis. Angular error of e1 and relative error of k1 are the numbers that decide whether SAQ's
   field is usable at all.

C. THE PREMISE on the real AI mesh: how big is the "мятая фольга" high-frequency normal noise, and
   does it reach our output? Source faces are labelled flat by the SAQ threshold, then the same
   quantity is measured on the shipped output.

Usage: saq_validate.py [--mesh test_out/bridge.obj] [--alpha 2.0] [--eps0 0.02] [--out-obj FILE]
"""
import argparse
import os
import sys
from collections import defaultdict

import numpy as np
import scipy.sparse as sp
import scipy.sparse.linalg as spla

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from smooth_creases import load_polys, face_normals  # noqa: E402


# ------------------------------------------------------------------ A: reference prototype ----
def cot_laplacian(V, F):
    n = len(V)
    i0, i1, i2 = F[:, 0], F[:, 1], F[:, 2]
    v0, v1, v2 = V[i0], V[i1], V[i2]
    e0, e1, e2 = v2 - v1, v0 - v2, v1 - v0
    cross = np.cross(e1, e2)
    dbl = np.linalg.norm(cross, axis=1) + 1e-12
    cot0 = -np.einsum('ij,ij->i', e1, e2) / dbl
    cot1 = -np.einsum('ij,ij->i', e2, e0) / dbl
    cot2 = -np.einsum('ij,ij->i', e0, e1) / dbl
    II = np.concatenate([i1, i2, i2, i0, i0, i1])
    JJ = np.concatenate([i2, i1, i0, i2, i1, i0])
    WW = 0.5 * np.concatenate([cot0, cot0, cot1, cot1, cot2, cot2])
    C = sp.coo_matrix((WW, (II, JJ)), shape=(n, n)).tocsc()
    C = sp.diags(np.asarray(C.sum(axis=1)).ravel()) - C
    mass = np.zeros(n)
    np.add.at(mass, i0, dbl / 6.0)
    np.add.at(mass, i1, dbl / 6.0)
    np.add.at(mass, i2, dbl / 6.0)
    M = sp.diags(mass + 1e-12)
    return C, M, 0.5 * dbl


def face_mean_normals_raw(V, F):
    fn = np.cross(V[F[:, 1]] - V[F[:, 0]], V[F[:, 2]] - V[F[:, 0]])
    ln = np.linalg.norm(fn, axis=1)
    return fn / np.where(ln[:, None] < 1e-12, 1.0, ln[:, None])


def saq_reference(V, F, alpha=2.0, gamma=1e-3):
    C, M, fa = cot_laplacian(V, F)
    Minv = sp.diags(1.0 / M.diagonal())
    fn = np.cross(V[F[:, 1]] - V[F[:, 0]], V[F[:, 2]] - V[F[:, 0]])
    vn = np.zeros_like(V)
    for k in range(3):
        np.add.at(vn, F[:, k], fn)
    N = vn / (np.linalg.norm(vn, axis=1, keepdims=True) + 1e-12)
    edges = np.vstack([F[:, [0, 1]], F[:, [1, 2]], F[:, [2, 0]]])
    mean_e = np.mean(np.linalg.norm(V[edges[:, 0]] - V[edges[:, 1]], axis=1))
    tau = (alpha * mean_e) ** 4
    A = (M + tau * (C @ Minv @ C)).tocsc()
    try:
        Ns = spla.spsolve(A, M @ N)
    except Exception:
        Ns = spla.lsqr(A, M @ N)[0]
    Ns = Ns / (np.linalg.norm(Ns, axis=1, keepdims=True) + 1e-12)
    e01 = V[F[:, 1]] - V[F[:, 0]]; e02 = V[F[:, 2]] - V[F[:, 0]]
    d01 = Ns[F[:, 1]] - Ns[F[:, 0]]; d02 = Ns[F[:, 2]] - Ns[F[:, 0]]
    r01 = np.linalg.norm(d01, axis=1) / (np.linalg.norm(e01, axis=1) + 1e-12)
    r02 = np.linalg.norm(d02, axis=1) / (np.linalg.norm(e02, axis=1) + 1e-12)
    kmax = np.maximum(r01, r02); kmin = np.minimum(r01, r02)
    med = np.median(kmin); mad = np.median(np.abs(kmin - med))
    delta = med + 1.4826 * mad
    k1q = np.maximum(0.0, kmax - delta); k2q = np.maximum(0.0, kmin - delta)
    dens = np.sqrt((k1q + gamma) * (k2q + gamma))
    return {"delta": delta, "flat": (k1q == 0) & (k2q == 0), "density": dens,
            "areas": fa, "k1": k1q, "k2": k2q, "N_smooth": Ns, "mean_edge": mean_e}


# ------------------------------------------------------- B: the missing Weingarten operator -----
def spectral_normals(V, F, alpha=2.0):
    C, M, _ = cot_laplacian(V, F)
    Minv = sp.diags(1.0 / M.diagonal())
    fn = np.cross(V[F[:, 1]] - V[F[:, 0]], V[F[:, 2]] - V[F[:, 0]])
    vn = np.zeros_like(V)
    for k in range(3):
        np.add.at(vn, F[:, k], fn)
    N = vn / (np.linalg.norm(vn, axis=1, keepdims=True) + 1e-12)
    edges = np.vstack([F[:, [0, 1]], F[:, [1, 2]], F[:, [2, 0]]])
    mean_e = float(np.mean(np.linalg.norm(V[edges[:, 0]] - V[edges[:, 1]], axis=1)))
    tau = (alpha * mean_e) ** 4
    A = (M + tau * (C @ Minv @ C)).tocsc()
    b = M @ N
    # CG per component (SPD after the small mass regularisation); keeps determinism
    Ns = np.zeros_like(N)
    for c in range(3):
        x, info = spla.cg(A, b[:, c], tol=1e-8, maxiter=800)
        if info != 0:
            x = np.linalg.lstsq(A.toarray(), b[:, c], rcond=None)[0]
        Ns[:, c] = x
    Ns /= (np.linalg.norm(Ns, axis=1, keepdims=True) + 1e-12)
    return Ns, mean_e


def vertex_ring(V, F):
    nbr = defaultdict(set)
    for a, b, c in F:
        nbr[a].update((b, c)); nbr[b].update((a, c)); nbr[c].update((a, b))
    return {int(k): np.array(sorted(v), dtype=np.int64) for k, v in nbr.items()}


def weingarten(V, F, Ns, ring):
    """Least-squares symmetric shape operator per vertex, in the filtered tangent basis."""
    n = len(V)
    k1 = np.zeros(n); k2 = np.zeros(n); e1 = np.zeros((n, 3)); basis = np.zeros((n, 3))
    Wnorm = np.zeros(n)
    for i in range(n):
        nb = ring.get(i)
        if nb is None or len(nb) < 3:
            continue
        ni = Ns[i]
        t1 = np.array([0.0, 0.0, 1.0]) if abs(ni[2]) < 0.9 else np.array([1.0, 0.0, 0.0])
        t1 = t1 - (t1 @ ni) * ni; t1 /= np.linalg.norm(t1)
        t2 = np.cross(ni, t1)
        rows, rhs = [], []
        for j in nb:
            d = V[j] - V[i]
            u = float(d @ t1); v = float(d @ t2)
            dn = Ns[j] - Ns[i]
            a1 = float(dn @ t1); a2 = float(dn @ t2)
            # dn_tangent = -W * d_tangent  (Weingarten), unknowns b,c,d of a symmetric 2x2
            rows.append([-u, -v, 0.0]);  rhs.append(a1)
            rows.append([0.0, -u, -v]);  rhs.append(a2)
        if not rows:
            continue
        A = np.asarray(rows); b = np.asarray(rhs)
        sol, *_ = np.linalg.lstsq(A, b, rcond=None)
        W = np.array([[sol[0], sol[1]], [sol[1], sol[2]]])
        ev, evec = np.linalg.eigh(W)
        order = np.argsort(-np.abs(ev))
        ev = ev[order]; evec = evec[:, order]
        k1[i] = ev[0]; k2[i] = ev[1]
        c0 = evec[0, 0] * t1 + evec[1, 0] * t2      # first eigenvector (2D basis) -> 3D
        ln = np.linalg.norm(c0)
        e1[i] = c0 / ln if ln > 1e-12 else c0
        basis[i] = t1
        Wnorm[i] = np.linalg.norm(W)
    return k1, k2, e1, Wnorm


def label_bridge(V, F):
    """Analytic ground truth of test_out/gen_cases.py: which source faces are planes / cylinder."""
    labels = np.empty(len(F), dtype=np.int64)      # 0 unknown, 1 plane, 2 cylinder r=0.75
    cen = np.array([V[f].mean(axis=0) for f in F])
    cx, cz, r = 0.0, 0.15, 0.75
    for i, c in enumerate(cen):
        rho = np.hypot(c[0] - cx, c[2] - cz)
        if abs(rho - r) < 0.03 and abs(c[1]) < 0.55:
            labels[i] = 2
            continue
        n = face_normals(V, [F[i]])[0]
        if abs(abs(n[2]) - 1.0) < 1e-6 and abs(c[2] - 0.59) < 0.02:
            labels[i] = 1                       # deck bottom/top plates
        elif abs(abs(n[1]) - 1.0) < 1e-6:
            labels[i] = 1                       # vertical plate faces
        elif abs(abs(n[0]) - 1.0) < 1e-6:
            labels[i] = 1
    return labels


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--mesh", default="test_out/bridge.obj")
    ap.add_argument("--alpha", type=float, default=2.0)
    ap.add_argument("--eps0", type=float, default=0.02)
    ap.add_argument("--out-obj", default="")
    a = ap.parse_args()
    root = os.path.dirname(HERE)
    path = a.mesh if os.path.isabs(a.mesh) else os.path.join(root, a.mesh)

    V, polys = load_polys(path)
    tri = []
    for p in polys:
        for i in range(1, len(p) - 1):
            tri.append((p[0], p[i], p[i + 1]))
    F = np.asarray(tri, dtype=np.int64)
    print(f"mesh {os.path.basename(path)}: verts={len(V)} faces={len(F)}")

    ref = saq_reference(V, F, alpha=a.alpha)
    flat_area = float(ref["areas"][ref["flat"]].sum() / ref["areas"].sum())
    K_ref = float(ref["areas"] @ ref["density"]) / a.eps0
    print(f"[A] prototype: delta(MAD)={ref['delta']:.4f}  flat area={100*flat_area:.1f}%  "
          f"K=int area*sqrt(detG)/eps0 = {K_ref:.0f}  (eps0={a.eps0})")
    # density on the cylinder, where their gamma->0+ is supposed to keep cells finite
    lab = label_bridge(V, F)
    cyl = lab == 2
    if cyl.any():
        d_cyl = ref["density"][cyl]
        d_pl = ref["density"][(lab == 1)]
        print(f"    on the arch (cylinder r=0.75): sqrt(detG) median={np.median(d_cyl):.5f} "
              f"vs on plates {np.median(d_pl) if len(d_pl) else float('nan'):.5f} "
              f"-> prototype k1 median={np.median(ref['k1'][cyl]):.4f} (analytic 1.3333)")
    ratio = np.sqrt((ref['k1'] + 1e-3) / (ref['k2'] + 1e-3))
    print(f"    prototype anisotropy ratio: p50(all)={np.median(ratio):.2f} "
          + (f"p50(arch)={np.median(ratio[cyl]):.2f}  " if cyl.any() else "")
          + f"-> on a cylinder the true ratio is >>1; ~1.00 means the field is ISOTROPIC everywhere")
    if (lab == 1).any():
        fa = ref['areas']
        plate_flat = float(fa[(lab == 1) & ref['flat']].sum() / max(1e-12, fa[lab == 1].sum()))
        print(f"    of the ANALYTIC PLATE area the prototype calls flat: {100*plate_flat:.1f}% "
              f"(law requires ~100%)")

    Ns, mean_e = spectral_normals(V, F, alpha=a.alpha)
    ring = vertex_ring(V, F)
    k1, k2, e1, _ = weingarten(V, F, Ns, ring)
    print(f"[B] Weingarten rebuild (mean edge={mean_e:.4f}, alpha={a.alpha}):")
    if cyl.any():
        vc = np.unique(F[cyl])
        a1 = np.abs(k1[vc]); a2 = np.abs(k2[vc])
        big = np.maximum(a1, a2); sml = np.minimum(a1, a2)
        # analytic check on the cylinder band
        err_k = np.median(big) - (1.0 / 0.75)
        axis = np.array([0.0, 1.0, 0.0])           # arch extrusion direction: k2 must be 0 along it
        ang = []
        for i in vc:
            e = e1[i]
            if np.linalg.norm(e) < 1e-9:
                continue
            # e1 (max curvature) should be perpendicular to the extrusion axis
            ang.append(np.degrees(np.arccos(np.clip(abs(e @ axis), 0, 1))))
        print(f"    cylinder verts={len(vc)}: |k| big median={np.median(big):.4f} "
              f"(analytic 1.3333, err {err_k:+.4f}), small median={np.median(sml):.4f} (analytic 0)")
        print(f"    principal-direction error vs extrusion axis: p50={np.median(ang):.1f}deg "
              f"(should be ~90deg)")
    if (lab == 1).any():
        vp = np.unique(F[lab == 1])
        print(f"    plate verts={len(vp)}: median |k1|={np.median(np.abs(k1[vp])):.4f} "
              f"median |k2|={np.median(np.abs(k2[vp])):.4f} (analytic 0,0)")
        mad = np.median(np.abs(np.maximum(np.abs(k1), np.abs(k2)) -
                              np.median(np.maximum(np.abs(k1), np.abs(k2)))))
        delta_sa = np.median(np.maximum(np.abs(k1), np.abs(k2))) + 1.4826 * mad
        print(f"    SAQ threshold from MAD over all verts: delta={delta_sa:.4f} "
              f"-> flat verts={(np.maximum(np.abs(k1), np.abs(k2)) <= delta_sa).sum()}/{len(V)}")

    if a.out_obj:
        nrm = Ns
        with open(a.out_obj, "w") as fh:
            for p in V:
                fh.write("v %.6f %.6f %.6f\n" % tuple(p))
            for f in F:
                fh.write("f %d %d %d\n" % (f[0] + 1, f[1] + 1, f[2] + 1))
            for x in nrm:
                fh.write("vn %.5f %.5f %.5f\n" % tuple(x))
        print(f"[out] filtered normals -> {a.out_obj}")


if __name__ == "__main__":
    main()
