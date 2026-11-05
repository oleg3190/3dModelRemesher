# Разбор машинного кода: meshoptimizer 1.3.0 (WASM)

Артефакты обратной инженерии пакета `node_modules/meshoptimizer` —
компилята C++ → WebAssembly (clang 22.1.0-wasi-sdk), которая и есть
«машинный код» в составе AstraForge.

## Файлы

| Файл | Что это |
|---|---|
| `meshopt_simplifier.wasm` | распакованный бинарник модуля, 40 919 байт, магия `00 61 73 6d` |
| `meshopt_simplifier.wat` | полный дизайн сборки в текстовый формат WAT, 20 511 строк, 58 функций (wabt `wasm2wat`, с `generateNames`/`applyNames`) |
| `f1.wat` | только основная функция `$f1` (~9 900 строк) — ядро `meshopt_simplify*` |
| `extract.mjs` | скрипт: вытаскивает и распаковывает embedded-WASM из `meshopt_simplifier.js` (перехватывает `WebAssembly.instantiate(unpack(wasm))`) |
| `to-wat.mjs` | скрипт: `.wasm` → `.wat` через npm-пакет `wabt` |

## Повторное воспроизведение

```bash
cd analysis/machine-code
node extract.mjs        # перечитает node_modules/meshoptimizer/meshopt_simplifier.js
npm install wabt        # одноразово (или использовать уже поставленный)
node to-wat.mjs
```

## Ключевые находки (см. полный разбор в сессии)

- Экспортов 12: `meshopt_simplify`, `simplifyWithAttributes`, `simplifyWithUpdate`,
  `simplifySloppy`, `simplifyPrune`, `simplifyPoints`, `simplifyScale`, `remesh`,
  `optimizeVertexFetchRemap`, `generatePositionRemap`, `sbrk`, `__wasm_call_ctors`.
- Флаги-маски подтверждены в коде: 1 LockBorder, 2 Sparse, 4 ErrorAbsolute,
  8 Prune, 16 Regularize, 32 Permissive, 64 RegularizeLight, 128 PreserveFolds,
  256 ErrorClamped, 1<<30 debug.
- Нормировка порога: `l59 = target_error² / (flags&4 ? 1 : scale)²` —
  относительная ошибка относительно масштаба сетки, если не задан ErrorAbsolute.
- Только `f32`-арифметика (ни одного `f64`) — квадрики в одинарной точности.
- Много раундовый основной цикл схлопываний: порог растёт `×1.5` за раунд
  (`f32.const 0x1.8p+0`), не глобальная очередь сортировки (нет qsort/i32.trunc).
- PreserveFolds (бит 128): отдельный блок обхода отсортированных пар рёбер
  (записи 12 и 44 байт, по 3 соседних треугольника).
- Проверка площади треугольника: `sqrt(x²+y²+z²)*0.25`, эпсилонами 1e-7/1e-6/0.0001.

## Связь с AstraForge

`server/optimize.mjs` → `POST /api/optimize` → `MeshoptSimplifier.simplify(...)`
с `target_error = 0.002` и флагом `['PreserveFolds']`, затем `compactMesh`
и построение обратного mapping вершин (`remap[i] !== 0xffffffff`).
