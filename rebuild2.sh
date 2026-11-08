#!/usr/bin/env bash
# Manual rebuild of qremesh core+CLI without cmake (cmake binary is missing on this host).
set -euo pipefail
R=/home/s/my/APPS/llama/flux/3dModelRemesher
B=$R/build2
D=$R/.deps/root/usr
OBJ=$B/CMakeFiles/qremesh_core.dir
CLIOBJ=$B/CMakeFiles/qremesh_cli.dir

INC="-I$R/include -I$R/src -isystem $D/include/eigen3 -isystem $D/include"
FLAGS="-O3 -DNDEBUG -std=c++20 -fPIC -Wall -Wextra -Werror"

srcs=(src/common/half_edge_mesh.cpp src/common/bvh.cpp src/remesh_context.cpp
      src/stages/stage0_prep.cpp src/stages/stage1_cut.cpp src/stages/stage2_size.cpp
      src/stages/stage3_field.cpp src/stages/stage4_patch.cpp src/stages/stage5_param.cpp
      src/stages/stage6_opt.cpp src/qremesh_c_api.cpp)

for s in "${srcs[@]}"; do
  o="$OBJ/${s}.o"
  if [ ! -f "$o" ] || [ "$R/$s" -nt "$o" ]; then
    echo "CXX $s"
    c++ -Dqremesh_core_EXPORTS $INC $FLAGS -c "$R/$s" -o "$o"
  fi
done

echo "LINK libqremesh_core.so"
objs=()
for s in "${srcs[@]}"; do objs+=("$OBJ/${s}.o"); done
c++ -fPIC -O3 -DNDEBUG -shared -Wl,-soname,libqremesh_core.so -o "$B/libqremesh_core.so" \
  "${objs[@]}" \
  -Wl,-rpath,$D/lib/x86_64-linux-gnu "$D/lib/x86_64-linux-gnu/libtbb.so.12.11"

if [ ! -f "$CLIOBJ/cli/main.cpp.o" ] || [ "$R/cli/main.cpp" -nt "$CLIOBJ/cli/main.cpp.o" ]; then
  echo "CXX cli/main.cpp"
  mkdir -p "$CLIOBJ/cli"
  c++ $INC -O3 -DNDEBUG -std=c++20 -fPIE -Wall -Wextra -Werror -c "$R/cli/main.cpp" -o "$CLIOBJ/cli/main.cpp.o"
fi
echo "LINK qremesh_cli"
c++ "$CLIOBJ/cli/main.cpp.o" -o "$B/qremesh_cli" -Wl,-rpath,"$B" -L"$B" -lqremesh_core
echo OK
