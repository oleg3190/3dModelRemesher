#!/usr/bin/env bash
# T18 step 1: calibrate the Delta-n law BEFORE adding T5.
# Decoupled targets (user, 2026-11-08):
#   Primary   median SJ >= 0.74
#   Secondary polygons <= 6.0k
#   Tertiary  DoD < 15 per 1k faces, Hausdorff <= 1.9%
# Also prints the fraction of patches whose grid is too coarse to orthoganalize
# (min(nu,nv) < 3, the S26 ceiling) - the quantity T5 would have to move.
set -uo pipefail
cd "$(dirname "$0")/.." || exit 1
SRC=${1:-test_out/input.obj}
T=${TARGET:-6000}
extra=${EXTRA:-}
printf "%-22s %7s %7s %7s %8s %7s %7s %7s\n" "config" "polys" "medSJ" "minSJ" "DoD" "DoD/1k" "Haus%" "coarse%"
row() {
  local name=$1 envs=$2 deg=$3 mn=$4 mx=$5
  local o=/tmp/dncal_"${name//[^A-Za-z0-9]/_}".obj
  local l=/tmp/dncal_"${name//[^A-Za-z0-9]/_}".log
  if [ "$deg" = "off" ]; then
    env $extra ./build2/qremesh_cli "$SRC" -o $o --target $T --quality >$l 2>&1
  else
    env QREMESH_DN_DEG=$deg QREMESH_DN_MIN=$mn QREMESH_DN_MAX=$mx $extra \
      ./build2/qremesh_cli "$SRC" -o $o --target $T --quality >$l 2>&1
  fi
  local polys med minj dod haus coarse
  polys=$(grep -oE "Polys: [0-9]+" $l | head -1 | grep -oE "[0-9]+")
  med=$(grep -oE "Median Scaled Jacobian: [-0-9.]+" $l | grep -oE "[-0-9.]+")
  minj=$(grep -oE "Min Scaled Jacobian: [-0-9.]+" $l | grep -oE "[-0-9.]+")
  haus=$(grep -oE "Hausdorff Max: [0-9.]+" $l | grep -oE "[0-9.]+")
  coarse=$(grep -oE "coarse_min_dim_lt3=[0-9]+/[0-9]+ \([0-9.]+%\)" $l | tail -1 | grep -oE "\([0-9.]+%" | tr -d "(%")
  dod=$(python3 test_out/smooth_creases.py $o "$SRC" --json 2>/dev/null | python3 -c "import json,sys;d=json.load(sys.stdin);print('%d %.2f'%(d['smooth_creases'],d['smooth_creases_per_kfaces']))" 2>/dev/null)
  printf "%-22s %7s %7s %7s %8s %7s %7s %7s\n" "$name" "$polys" "$med" "$minj" \
    "$(echo $dod | cut -d' ' -f1)" "$(echo $dod | cut -d' ' -f2)" "$haus" "$coarse"
}
row "off.baseline" "" off 0 0
for deg in 10 15 20 25; do
  for mn in 0.25 0.30 0.40; do
    for mx in 2.0 3.0; do
      row "dn${deg}_min${mn}_max${mx}" "$deg" "$deg" "$mn" "$mx"
    done
  done
done
