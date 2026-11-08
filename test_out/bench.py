#!/usr/bin/env python3
"""T19 - comparison harness. The rule it enforces: no single-point "improvements".

Every claim in this thread that survived was checked against 5 budgets; the ones that did not
(S45 twice, S47, and the crease half of S48) came from one run. So this harness makes the protocol
mechanical:

  - a frozen grid: N configs x M budgets (x model), one CLI call per cell, results cached on disk;
  - mandatory metrics per cell: polygons, quads, triangles, min/median Scaled Jacobian, RMS,
    Hausdorff, boundary %, non-manifold, components, wall time, plus the two fold metrics computed
    here (creases on locally-smooth source per 1k faces, same-side pleat %) and a winding audit;
  - a paired comparison per budget between the reference config and each candidate;
  - a verdict per metric, and it says INCONCLUSIVE whenever the spread covers the delta:
      CONFIRMED   all paired deltas share the sign AND |median delta| >= EFFECT_REL x |median base|
      LIKELY      >=4/5 same sign AND the same effect-size test
      otherwise   INCONCLUSIVE  (and a run with < MIN_POINTS budgets can never be CONFIRMED)
  - determinism is checked separately (repeat the same cell, compare md5).

Usage:
  bench.py run  [--configs base,dn20,stack] [--budgets 4000,5500,7000,8500,10000]
                [--model ai|sphere|rail] [--quality] [--force]
  bench.py cmp  --ref base --cand dn20 [--model ai] [--budgets ...]
  bench.py det  --config dn20 [--budget 6000]
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
import time
from collections import defaultdict

import numpy as np
from scipy.spatial import cKDTree

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from smooth_creases import load_polys, face_normals, edge_dihedrals, scaled_jacobian  # noqa: E402

CACHE_DIR = os.path.join(ROOT, "test_out", ".bench")
CLI = os.environ.get("QREMESH_CLI", os.path.join(ROOT, "build2", "qremesh_cli"))
MIN_POINTS = 5
EFFECT_REL = 0.05          # |median delta| must be at least 5% of the reference median

MODELS = {
    "ai": {"src": "test_out/input.obj", "quality": True},
    "sphere": {"src": "test_out/sphere20k.obj", "quality": False},
    "rail": {"src": "test_out/rail10.obj", "quality": False},
}

CONFIGS = {
    "base": {},
    "dn20": {"QREMESH_DN_DEG": "20", "QREMESH_DN_MIN": "0.25", "QREMESH_DN_MAX": "3.0"},
    "dn20_grow": {"QREMESH_DN_DEG": "20", "QREMESH_DN_MIN": "1.0", "QREMESH_DN_MAX": "3.0"},
    "t16": {"QREMESH_T16": "1"},
    "collapse": {"QREMESH_COLLAPSE": "0.5"},
    "stack": {"QREMESH_COLLAPSE": "0.5", "QREMESH_T7": "1", "QREMESH_T7_LOOSE": "1",
              "QREMESH_T7_CAP": "2.5", "QREMESH_T7_ITERS": "20"},
    "dn20+stack": {"QREMESH_DN_DEG": "20", "QREMESH_DN_MIN": "0.25", "QREMESH_DN_MAX": "3.0",
                   "QREMESH_COLLAPSE": "0.5", "QREMESH_T7": "1", "QREMESH_T7_LOOSE": "1",
                   "QREMESH_T7_CAP": "2.5", "QREMESH_T7_ITERS": "20"},
}

# lower is better for these; the sign convention for verdicts
METRICS = {
    "polys": "size", "quads": "size", "triangles": "size",
    "min_sj": "max", "median_sj": "max", "rms_pct": "min", "hausdorff_pct": "min",
    "boundary_pct": "min", "nonmanifold": "min", "components_dist": "min",
    "crease_per_1k": "min", "pleat_pct": "min", "winding_conflicts": "min",
    "time_s": "time",
}
LOWER_BETTER = {"rms_pct", "hausdorff_pct", "boundary_pct", "nonmanifold", "components_dist",
                "crease_per_1k", "pleat_pct", "winding_conflicts", "time_s"}
HIGHER_BETTER = {"min_sj", "median_sj"}


# --------------------------------------------------------------------- source-side memo ---------
_SRC_MEMO = {}


def src_side(src_path, rel=0.01, gate=30.0):
    key = (src_path, rel, gate)
    if key in _SRC_MEMO:
        return _SRC_MEMO[key]
    Vs, ps = load_polys(src_path)
    Ns = face_normals(Vs, ps)
    maxd_s, _ = edge_dihedrals(Vs, ps, Ns)
    Cs = np.array([Vs[idx].mean(axis=0) for idx in ps])
    diag = float(np.linalg.norm(Vs.max(0) - Vs.min(0)))
    kd = cKDTree(Cs)
    _SRC_MEMO[key] = (kd, maxd_s, rel * diag)
    return _SRC_MEMO[key]


def fold_metrics(out_path, src_path):
    """Creases on locally-smooth source (per 1k faces) + same-side pleat rate of interior edges."""
    from collections import defaultdict
    V, po = load_polys(out_path)
    N = face_normals(V, po)
    maxd, _ = edge_dihedrals(V, po, N)
    C = np.array([V[idx].mean(axis=0) for idx in po])
    isq = np.array([len(idx) == 4 for idx in po])
    kd, maxd_s, R = src_side(src_path)

    creased = np.where((maxd > 120.0) & isq)[0]
    n_target = 0
    for fi in creased:
        nn = kd.query_ball_point(C[fi], r=R)
        if nn and maxd_s[nn].max() < 30.0:
            n_target += 1

    e2f = defaultdict(list)
    for fi, idx in enumerate(po):
        pn = len(idx)
        for i in range(pn):
            a, b = idx[i], idx[(i + 1) % pn]
            free = [v for v in idx if v != a and v != b]
            e2f[(min(a, b), max(a, b))].append((fi, free[0] if free else None))
    pleat = two = 0
    for k, fs in e2f.items():
        if len(fs) != 2:
            continue
        (f0, w0), (f1, w1) = fs
        if w0 is None or w1 is None:
            continue
        two += 1
        a, b = k
        e = V[b] - V[a]
        L = np.linalg.norm(e)
        if L < 1e-12:
            continue
        e = e / L
        mid = 0.5 * (V[a] + V[b])
        u0 = V[w0] - mid; u0 = u0 - np.dot(u0, e) * e
        u1 = V[w1] - mid; u1 = u1 - np.dot(u1, e) * e
        if np.dot(u0, u1) > 0:
            pleat += 1
    return {
        "crease_per_1k": 1000.0 * n_target / max(1, len(po)),
        "pleat_pct": 100.0 * pleat / max(1, two),
    }


def winding_conflicts_of(path):
    V, po = load_polys(path)
    from smooth_creases import winding_conflicts
    return winding_conflicts(po)


# ------------------------------------------------------------------------ runner ----------------
def run_cell(model, config, budget, force=False):
    m = MODELS[model]
    cfg = CONFIGS[config]
    os.makedirs(CACHE_DIR, exist_ok=True)
    tag = hashlib.sha256(json.dumps({"m": model, "c": config, "b": budget, "cli": _cli_stamp()},
                                    sort_keys=True).encode()).hexdigest()[:16]
    cf = os.path.join(CACHE_DIR, f"{tag}.json")
    if os.path.exists(cf) and not force:
        return json.load(open(cf))

    out = os.path.join(CACHE_DIR, f"{tag}.obj")
    log = os.path.join(CACHE_DIR, f"{tag}.log")
    cmd = [CLI, os.path.join(ROOT, m["src"]), "-o", out, "--target", str(budget)]
    if m.get("quality"):
        cmd.append("--quality")
    env = dict(os.environ)
    env.update(cfg)
    t0 = time.time()
    with open(log, "w") as fh:
        rc = subprocess.call(cmd, cwd=ROOT, env=env, stdout=fh, stderr=subprocess.STDOUT)
    if rc != 0 or not os.path.exists(out):
        raise SystemExit(f"CLI failed for {model}/{config}/{budget}: see {log}")
    txt = open(log).read()

    def grab(pattern, cast=float, default=None):
        mm = re.search(pattern, txt)
        return cast(mm.group(1)) if mm else default

    polys = grab(r"Polys:\s*(\d+)")
    res = {
        "model": model, "config": config, "budget": budget,
        "polys": polys or 0,
        "quads": grab(r"Quads:\s*(\d+)", int, 0),
        "triangles": grab(r"Triangles:\s*(\d+)", int, 0),
        "min_sj": grab(r"Min Scaled Jacobian:\s*(-?[\d.]+)", float, 0.0),
        "median_sj": grab(r"Median Scaled Jacobian:\s*(-?[\d.]+)", float, 0.0),
        "rms_pct": grab(r"RMS Deviation:\s*([\d.]+)", float, 0.0),
        "hausdorff_pct": grab(r"Hausdorff Max:\s*([\d.]+)", float, 0.0),
        "boundary_pct": grab(r"Boundary edges:\s*\d+\s*/\s*\d+\s*\(([\d.]+)", float, 0.0),
        "nonmanifold": grab(r"Non-manifold edges:\s*(\d+)", int, 0),
        "components_dist": abs((grab(r"Connected components:\s*(\d+)", int, 1) or 1) - 1),
        "time_s": round(time.time() - t0, 2),
        "md5": _md5(out),
    }
    fm = fold_metrics(out, os.path.join(ROOT, m["src"]))
    res.update(fm)
    res["winding_conflicts"] = winding_conflicts_of(out)
    json.dump(res, open(cf, "w"), indent=1)
    return res


def _cli_stamp():
    p = CLI
    try:
        st = os.stat(p)
        return f"{int(st.st_mtime)}-{st.st_size}"
    except OSError:
        return "missing"


def _md5(path):
    h = hashlib.md5()
    with open(path, "rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


# ---------------------------------------------------------------------- statistics --------------
NEUTRAL = {"polys", "quads", "triangles"}   # size metrics: reported, never judged good/bad


def verdict(ref_rows, cand_rows):
    """Paired per-budget comparison. Returns {metric: (median_delta, sign_ok, rel_ok, verdict)}"""
    by_b_ref = {r["budget"]: r for r in ref_rows}
    by_b_cand = {r["budget"]: r for r in cand_rows}
    common = sorted(set(by_b_ref) & set(by_b_cand))
    out = {}
    for mt in METRICS:
        d = [by_b_cand[b][mt] - by_b_ref[b][mt] for b in common]
        if not d:
            continue
        med = float(np.median(d))
        base_med = float(np.median([by_b_ref[b][mt] for b in common]))
        n = len(d)
        if mt in NEUTRAL:
            out[mt] = {"n": n, "median_delta": round(med, 4), "base_median": round(base_med, 4),
                       "spread": [round(min(d), 4), round(max(d), 4)], "same_sign": "-",
                       "verdict": "SIZE (not judged)"}
            continue
        want_neg = mt in LOWER_BETTER
        # count how many budgets moved in the wanted direction and how many against it: a run that
        # is consistently WORSE must be reported as REGRESSED, not swallowed by "INCONCLUSIVE"
        n_want = sum(1 for x in d if ((x < 0) if want_neg else (x > 0)))
        n_anti = sum(1 for x in d if ((x > 0) if want_neg else (x < 0)))
        denom = abs(base_med) if abs(base_med) > 1e-12 else 1.0
        rel_ok = abs(med) >= EFFECT_REL * denom
        enough = n >= MIN_POINTS
        if not enough or (n_want < n - 1 and n_anti < n - 1):
            v = "INCONCLUSIVE"
        elif not rel_ok:
            v = "NOISE"                       # consistent but below the effect floor
        elif n_want == n:
            v = "IMPROVED"
        elif n_want == n - 1:
            v = "LIKELY-IMPROVED"
        elif n_anti == n:
            v = "REGRESSED"
        else:
            v = "LIKELY-REGRESSED"
        out[mt] = {"n": n, "median_delta": round(med, 4), "base_median": round(base_med, 4),
                   "spread": [round(min(d), 4), round(max(d), 4)],
                   "same_sign": f"{n_want}/{n} for, {n_anti}/{n} against", "verdict": v}
    return out


def table(rows):
    cols = ["budget", "config", "polys", "triangles", "median_sj", "min_sj", "hausdorff_pct",
            "rms_pct", "boundary_pct", "crease_per_1k", "pleat_pct", "winding_conflicts",
            "components_dist", "time_s"]
    print("  " + " ".join(f"{c:>15}" for c in cols))
    for r in sorted(rows, key=lambda x: (x["config"], x["budget"])):
        print("  " + " ".join(f"{r[c]:>15.4g}" if isinstance(r[c], float) else f"{r[c]:>15}"
                              for c in cols))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("action", choices=["run", "cmp", "det"])
    ap.add_argument("--configs", default="base,dn20")
    ap.add_argument("--ref", default="base")
    ap.add_argument("--cand", default="dn20")
    ap.add_argument("--model", default="ai")
    ap.add_argument("--budgets", default="4000,5500,7000,8500,10000")
    ap.add_argument("--budget", type=int, default=6000)
    ap.add_argument("--force", action="store_true")
    a = ap.parse_args()
    cfgs = [c for c in a.configs.split(",") if c]
    budgets = [int(b) for b in a.budgets.split(",") if b]
    for c in cfgs + [a.ref, a.cand]:
        if c not in CONFIGS:
            raise SystemExit(f"unknown config {c}; known: {sorted(CONFIGS)}")

    if a.action == "det":
        hashes = []
        for i in range(3):
            r = run_cell(a.model, a.configs if a.configs in CONFIGS else a.cand, a.budget, force=True)
            hashes.append(r["md5"])
        print(f"determinism {a.model}/{a.cand}@{a.budget}: {hashes} -> "
              + ("OK" if len(set(hashes)) == 1 else "NONDETERMINISTIC"))
        return

    rows = defaultdict(list)
    for c in (cfgs if a.action == "run" else [a.ref, a.cand]):
        for b in budgets:
            rows[c].append(run_cell(a.model, c, b, force=a.force))
    print(f"model={a.model}  budgets={budgets}  cli={_cli_stamp()}")
    table([r for rr in rows.values() for r in rr])

    if a.action == "cmp":
        print(f"\npaired comparison  ref={a.ref}  cand={a.cand}  n={len(budgets)} budgets"
              + ("" if len(budgets) >= MIN_POINTS else "  [< MIN_POINTS -> nothing can be CONFIRMED]"))
        print(f"  {'metric':>18} {'base_median':>12} {'median_delta':>13} {'delta_range':>18} "
              f"{'signs':>22}  verdict")
        v = verdict(rows[a.ref], rows[a.cand])
        order = ["crease_per_1k", "pleat_pct", "median_sj", "min_sj", "hausdorff_pct", "rms_pct",
                 "boundary_pct", "winding_conflicts", "components_dist", "nonmanifold",
                 "polys", "triangles", "time_s"]
        incon = 0
        for mt in order:
            if mt not in v:
                continue
            d = v[mt]
            if d["verdict"] in ("INCONCLUSIVE", "NOISE"):
                incon += 1
            print(f"  {mt:>18} {d['base_median']:>12g} {d['median_delta']:>13g} "
                  f"{str(d['spread']):>18} {str(d['same_sign']):>22}  {d['verdict']}")
        judged = [m for m in order if m in v and m not in NEUTRAL]
        incon = sum(1 for m in judged if v[m]["verdict"] in ("INCONCLUSIVE", "NOISE"))
        print(f"\n  {incon} of {len(judged)} judged metrics are statistically indistinguishable "
              f"at n={len(budgets)} (rule: >=4/{len(budgets)} budgets in one direction AND "
              f"|median delta| >= {EFFECT_REL:.0%} of the reference median; size metrics are never "
              f"judged good/bad)")


if __name__ == "__main__":
    main()
