# Vehicle / High Detail preset

The Blender addon includes a **Vehicle / High Detail** preset intended for detailed hard-surface assets and AI-generated vehicles.

## Settings

| Parameter | Value | Reason |
|---|---:|---|
| Target quads | 20,000 | Allocates a larger polygon budget than the general 5,000-quad default. |
| Adaptive density | 0.0 | Uses an even base sizing field. The Stage 2 implementation notes that its current per-vertex curvature signal is noisy on the AI-mesh probe and that increasing adaptive density raised the measured fold rate. |
| Max surface deviation | 0.08% | Tightens the permitted deviation from the source surface. |
| Sharp angle | 35° | Detects more hard-surface transitions than the general 45° default. |
| Relax iterations | 2 | Limits post-process movement so the result does not wash out small body details. Stage 6 now executes the requested count instead of forcing at least eight passes. |
| Shape preservation | 0.98 | Strengthens the pull toward the original surface. |
| QEM proxy threshold | 1,000,000 triangles | Delays early proxy decimation on dense input meshes; this increases memory and processing requirements. |

The preset keeps **Preserve Open Borders** enabled. This is safer for models with intentional openings (windows, wheel arches, underside openings). Do not disable it indiscriminately: filling every opening can close geometry that is meant to remain open.

## Recommended workflow

1. Duplicate the source object so the original remains available.
2. Apply **Vehicle / High Detail** and remesh the duplicate.
3. Compare the result against the source from the same front, side, and three-quarter views.
4. If the source itself contains weak or missing details, a higher quad count cannot reconstruct information that is absent from the input.
5. If the output is still too coarse and the source has clean geometry, test 30,000 quads on a duplicate and compare the quality report and processing time.

## Validation metrics

For each run, record target and actual polygon count, RMS deviation, Hausdorff maximum, median/minimum Scaled Jacobian, boundary edges, non-manifold edges, connected components, and processing time. Visually inspect the grille, wheel rims/spokes, wheel arches, window frames, door seams, roof line, and front/rear silhouette.

The preset is a starting point, not a guarantee of reconstruction quality. In particular, it cannot recover detail missing from the input mesh. The current environment does not include the user's source vehicle mesh, so this preset has not been benchmarked against that exact asset.
