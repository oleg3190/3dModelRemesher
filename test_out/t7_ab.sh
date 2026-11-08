#!/usr/bin/env bash
# T7 A/B: run the localized smooth-crease repair in each family mode and measure the
# BUG-01 DoD metric (smooth-region creases) + quality regression metrics.
set -uo pipefail
R=/home/s/my/APPS/llama/flux/3dModelRemesher
cd "$R"
SRC=test_out/input.obj
for m in 0 1 2 3; do
  out=test_out/t7_m$m.obj
  if [ "$m" = 0 ]; then
    ./build2/qremesh_cli $SRC -o $out --target 10000 --quality >/dev/null 2>&1
  else
    QREMESH_T7=1 QREMESH_T7_MODE=$m ./build2/qremesh_cli $SRC -o $out --target 10000 --quality >$out.log 2>&1
  fi
  echo "=== mode=$m"
  grep -h "stage6/t7\]" $out.log 2>/dev/null | sed 's/^/  /'
  grep -h -E "Min Scaled Jacobian|Median Scaled Jacobian|RMS Deviation|Hausdorff Max|Boundary edges|Non-manifold edges|Connected components|Polys:" $out.log 2>/dev/null | sed 's/^/  /'
  python3 test_out/smooth_creases.py $out $SRC --json 2>/dev/null | python3 -c "import json,sys;d=json.load(sys.stdin);print('  DETECTOR creased=%d smooth_creases=%d min_sj=%.3f med_sj=%.3f neg_quads=%d'%(d['creased_out'],d['smooth_creases'],d['min_sj'],d['median_sj'],d['sj_negative']))"
done
