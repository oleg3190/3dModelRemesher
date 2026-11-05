# QRemesh 2.0 (Clean-Room Anti-Spiral Quad Remesher)

Кроссплатформенная библиотека (C++20 ядро с C99 ABI) и расширение для Blender (3.6 LTS … 4.x+), реализующие автоматическую ретопологизацию произвольных полигональных сеток в квад-сетку (Quad-Dominant / All-Quad).

## Структура репозитория

```
├── CMakeLists.txt              # Сборочный сценарий CMake
├── LICENSE                     # Двухуровневая лицензия (MPL-2.0 для ядра, GPL-3.0 для аддона)
├── THIRD_PARTY_NOTICES.md      # Лицензии сторонних математических библиотек
├── include/
│   └── qremesh/
│       └── qremesh_c_api.h     # C99 ABI спецификация (структуры, коллбэки, функции)
├── src/
│   ├── common/
│   │   ├── types.h             # Базовые типы, Eigen векторы, матрицы, BoundingBox
│   │   ├── half_edge_mesh.h    # HalfEdge структура топологии на плоских std::vector
│   │   ├── half_edge_mesh.cpp
│   │   ├── bvh.h               # BVH пространственный индекс для проекции на High-Poly
│   │   └── bvh.cpp
│   ├── stages/
│   │   ├── stage0_prep.h       # Сварка, починка manifold, QEM-прокси, симметрия
│   │   ├── stage0_prep.cpp
│   │   ├── stage1_cut.h        # Встраивание шарпов, UV-швов, проекция кривых
│   │   ├── stage1_cut.cpp
│   │   ├── stage2_size.h       # Оценка кривизны, поле плотности, Липшиц-лимитер
│   │   ├── stage2_size.cpp
│   │   ├── stage3_field.h      # 4-RoSy кросс-поле, Леви-Чивита связность, сингулярности
│   │   ├── stage3_field.cpp
│   │   ├── stage4_patch.h      # Motorcycle Graph, анти-спиральные лупы, квад-патчи
│   │   ├── stage4_patch.cpp
│   │   ├── stage5_param.h      # Квантование, параметризация и сборка квадов
│   │   ├── stage5_param.cpp
│   │   ├── stage6_opt.h        # Таубин-релаксация, BVH-проекция, QualityReport
│   │   └── stage6_opt.cpp
│   ├── remesh_context.h        # Контекст выполнения и отладочные дампы
│   ├── remesh_context.cpp
│   └── qremesh_c_api.cpp       # Реализация экспортируемых C функций
├── cli/
│   └── main.cpp                # Автономная консольная утилита qremesh_cli
└── addon/
    └── qremesh/                # Аддон для Blender (формат Blender Extensions 4.2+ и 3.6 LTS)
        ├── blender_manifest.toml
        ├── __init__.py
        ├── props.py            # Настройки ремешера
        ├── bridge.py           # CTypes мост с Zero-Copy через numpy
        ├── extractors.py       # foreach_get извлечение буферов, цвета и штрихов
        ├── operators.py        # Неблокирующий модальный оператор с отменой (ESC)
        ├── ui_panel.py         # Панель во View3D > Sidebar > QRemesh
        ├── uv_transfer.py      # Авто-перенос UV через Data Transfer
        ├── gpu_overlay.py      # Viewport GPU отрисовка
        ├── presets/            # Пресеты: Organic_Rigging, HardSurface_CAD, Fast_Sculpt
        └── wheels/             # Скомпилированные библиотеки ядра
```

## Сборка C++ Ядра

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

В результате собираются:
- `libqremesh_core.so` (общая C-библиотека, автоматически копируется в `addon/qremesh/wheels/`)
- `libqremesh_core_static.a` (статическая библиотека для SDK)
- `qremesh_cli` (консольная утилита)

## Использование CLI

```bash
./build/qremesh_cli input.obj output.obj 5000
```
# 3dModelRemesher
