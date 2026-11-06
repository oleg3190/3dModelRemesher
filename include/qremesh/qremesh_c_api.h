#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum QrmStatus {
    QRM_OK = 0,
    QRM_ERR_INVALID_INPUT = 1,       // Пустой меш, < 4 граней, NaN в координатах
    QRM_ERR_TOPOLOGY_UNFIXABLE = 2,  // Критический non-manifold, не подлежащий авто-ремонту
    QRM_ERR_OUT_OF_MEMORY = 3,       // Нехватка ОЗУ
    QRM_ERR_CANCELLED = 4,           // Остановлено пользователем через callback
    QRM_ERR_FIELD_DIVERGED = 5,      // Не сошёлся решатель кросс-поля
    QRM_ERR_PARAM_FAILED = 6,        // Ошибка параметризации / извлечения квадов
    QRM_ERR_INTERNAL = 99
} QrmStatus;

typedef enum QrmCurveRole {
    QRM_CURVE_HARD_LOOP = 0,   // Обязательная линия рёбер (шарп, граница)
    QRM_CURVE_QUAD_STRIP = 1,  // Ровно один ряд квадов вдоль линии (фаска)
    QRM_CURVE_SEAM_SPLIT = 2,  // Разрез патчей (UV-шов или граница материала)
    QRM_CURVE_FLOW_GUIDE = 3   // Мягкий направляющий вектор (штрих Grease Pencil)
} QrmCurveRole;

typedef struct QrmCurve {
    const float* points;       // Кординаты полилинии: point_count * 3
    uint32_t     point_count;
    QrmCurveRole role;
    float        weight;       // Сила влияния (0.0 .. 1.0) для FLOW_GUIDE
} QrmCurve;

typedef struct QrmMeshIn {
    const float*    positions;             // nv * 3
    const uint32_t* tri_indices;           // nt * 3 (триангулированный буфер)
    const uint32_t* sharp_edge_pairs;      // num_sharp_edges * 2 (индексы вершин v0, v1)
    const float*    sharp_edge_creases;    // num_sharp_edges (значения 0.0 .. 1.0)
    const uint32_t* seam_edge_pairs;       // num_seam_edges * 2 (UV-швы v0, v1)
    const float*    vertex_density_weights;// NULL или nv (множитель плотности 0.25 .. 4.0 из Vertex Paint)
    const uint16_t* tri_material_ids;      // NULL или nt (индекс материала на треугольник)
    const QrmCurve* user_curves;           // NULL или num_curves
    
    uint32_t nv;
    uint32_t nt;
    uint32_t num_sharp_edges;
    uint32_t num_seam_edges;
    uint32_t num_curves;
} QrmMeshIn;

typedef struct QrmParams {
    int32_t  target_quad_count;      // Желаемое число квадов (0 = авто по target_edge_length)
    float    target_edge_length;     // Базовый шаг сетки (если target_quad_count == 0)
    float    adaptive_density;       // 0.0 (равномерная) .. 1.0 (мельче на высокой кривизне)
    float    max_surface_deviation;  // Допустимое отклонение от исходника (% от диагонали BBox)
    
    bool     preserve_sharp_edges;   // Удерживать острые рёбра
    float    sharp_angle_threshold;  // Авто-детект шарпов по углу в градусах (по умолч. 45.0)
    bool     preserve_uv_seams;      // Удерживать UV-швы как рёбра новой сетки
    bool     preserve_material_borders; // Удерживать границы материалов
    bool     preserve_open_borders;  // Точное удержание открытых границ меша
    
    int8_t   symmetry_axis;          // -1 = выкл, 0 = X, 1 = Y, 2 = Z, 3 = авто-детект
    float    symmetry_tolerance;     // Порог слияния по центральной плоскости (% от BBox)
    
    bool     enforce_closed_loops;   // Анти-спиральный фильтр на цилиндрических участках
    bool     exact_quad_count;       // Строгая подгонка под target_quad_count (±1%)
    bool     allow_triangles;        // Разрешать единичные треугольники в зонах перехода плотности
    
    int32_t  field_smooth_iters;     // Итерации сглаживания 4-RoSy поля (по умолч. 50)
    int32_t  vertex_relax_iters;     // Итерации финальной релаксации (по умолч. 20)
    float    shape_preservation;     // Вес привязки к исходной поверхности (0.0 .. 1.0)
    
    uint32_t proxy_tri_threshold;    // Порог включения QEM-прокси (по умолч. 300000)
    uint32_t num_threads;            // 0 = все доступные ядра
    uint32_t random_seed;            // Фиксированный seed для 100% детерминизма (по умолч. 42)
    const char* debug_dump_dir;      // NULL или путь к папке для сохранения OBJ по стадиям

    // --- Input repair / quality preset (S40) -----------------------------------------
    // Tri-state: -1 = auto (fall back to the QREMESH_* env override, then built-in
    // default OFF), 0 = explicitly off, 1 = explicitly on. Priority for the CLI path is
    // explicit flag > --quality preset > env > built-in default, which the CLI resolves
    // into these fields BEFORE calling qrm_remesh; the library treats a set (>=0) field as
    // authoritative and only consults env when the field is left at -1. Keeping the
    // defaults at -1 means qrm_default_params reproduces the pre-preset behaviour exactly
    // (bit-identical baseline for callers that never touch these fields).
    int8_t nm_keep_best;             // non-manifold keep-best-2 repair (safe on clean input)
    int8_t drop_debris;              // drop disconnected dust components (aggressive)
    int8_t drop_islands;             // drop final island shells (single-shell policy)
    float  weld_scale;               // weld-tolerance multiplier; 0 = auto/none
} QrmParams;

typedef struct QrmQualityReport {
    uint32_t quad_count;
    uint32_t tri_count;
    uint32_t ngon_count;
    uint32_t singularity_count;      // Число полюсов (внутренних вершин с valence != 4)
    float    hausdorff_max_rel;      // Макс. отклонение от оригинала (% от диагонали BBox)
    float    rms_deviation_rel;      // Среднеквадратичное отклонение (% от диагонали BBox)
    float    scaled_jacobian_min;    // Минимальный якобиан (-1..1, норма > 0.2)
    float    scaled_jacobian_median; // Медианный якобиан (норма > 0.80)
    float    max_warp_angle_deg;     // Макс. угол излома непланарного квада (в градусах)
    float    stage_times_sec[7];     // Время выполнения по стадиям 0..6
    float    total_time_sec;
} QrmQualityReport;

typedef struct QrmMeshOut {
    float*    positions;             // nv_out * 3
    uint32_t* poly_indices;          // Плоский массив индексов вершин полигонов (размер total_corners)
    uint32_t* poly_offsets;          // Смещения начал полигонов (размер np_out + 1)
    uint32_t* sharp_edge_out;        // num_sharp_out * 2
    float*    crease_edge_out;       // num_sharp_out
    uint32_t* seam_edge_out;         // num_seam_out * 2
    uint16_t* poly_material_ids;     // np_out
    
    uint32_t  nv_out;
    uint32_t  np_out;
    uint32_t  total_corners;
    uint32_t  num_sharp_out;
    uint32_t  num_seam_out;
    
    QrmQualityReport report;
} QrmMeshOut;

// Коллбэк прогресса. Возврат false инициирует безопасную остановку алгоритма.
typedef bool (*QrmProgressFn)(float progress_0_to_1, const char* stage_label, void* user_data);

// Заполнение структуры параметров значениями по умолчанию ("одна кнопка")
void qrm_default_params(QrmParams* out_params);

// Основной вызов ремешера
QrmStatus qrm_remesh(
    const QrmMeshIn*  in_mesh,
    const QrmParams*  params,
    QrmProgressFn     progress_cb,
    void*             user_data,
    QrmMeshOut**      out_mesh
);

// Обязательное освобождение памяти, выделенной ядром под QrmMeshOut
void qrm_free_mesh_out(QrmMeshOut* out_mesh);

#ifdef __cplusplus
}
#endif
