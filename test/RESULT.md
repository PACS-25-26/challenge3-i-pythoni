# Benchmark Results

This file summarizes the small scalability study requested by the assignment. The detailed benchmark data are stored in `test/data/performance.csv`, and the plots generated from those data are stored in `test/data/`.

The benchmark script uses the standard Jacobi solver on the sine manufactured problem with:

- grid sizes `n = 16, 32, 64, 128, 256`
- tolerance `1e-6`
- maximum iterations `2000`
- `OMP_NUM_THREADS=1`
- serial runs
- MPI runs with `np = 1, 2, 4`

The study is intentionally small and reproducible on a laptop. Jacobi converges slowly, so the iteration cap matters on the finest grids.

The benchmark outputs in `test/data/` are representative sample results kept in the repository as reproducible reference files. Re-running `test/run_scalability.sh` and `test/plot_results.py` overwrites them.

## Main Observations

- The serial solver and the MPI solver give the same iteration counts, convergence status, final increment, and L2 error for the same `n`, as expected from the same Jacobi method.
- The runs with `n = 16, 32, 64` reached the requested tolerance before the iteration cap.
- The runs with `n = 128, 256` did not converge within `2000` iterations. This is not a bug in the implementation; it reflects the slow convergence of plain Jacobi on finer grids.
- The parallel solver shows a clear benefit for the larger converged cases and still gives useful speedups on the capped runs.

## Timing Summary

Measured wall-clock times in seconds:

| n | serial | MPI 1 proc | MPI 2 proc | MPI 4 proc |
|---|---:|---:|---:|---:|
| 16  | 0.000831 | 0.000849 | 0.000644 | 0.000772 |
| 32  | 0.012894 | 0.011679 | 0.006945 | 0.005007 |
| 64  | 0.169033 | 0.160294 | 0.092948 | 0.067369 |
| 128 | 0.854454 | 0.835581 | 0.508124 | 0.322894 |
| 256 | 3.398378 | 3.376387 | 1.980032 | 1.496186 |

Corresponding speedups with respect to the serial run:

| n | MPI 1 proc | MPI 2 proc | MPI 4 proc |
|---|---:|---:|---:|
| 16  | 0.98 | 1.29 | 1.08 |
| 32  | 1.10 | 1.86 | 2.58 |
| 64  | 1.05 | 1.82 | 2.51 |
| 128 | 1.02 | 1.68 | 2.65 |
| 256 | 1.01 | 1.72 | 2.27 |

The very small `n = 16` case is dominated by overhead, so its timings are not especially meaningful. From `n = 32` onward, the benefit of the parallel solver becomes much clearer, and in the current sample results the best speedup is the `np = 4` run at `n = 128`, at about `2.65`.

These measurements come from a normal laptop environment. They are useful for reproducibility, but they should not be interpreted as ideal parallel scaling results.

## Accuracy Notes

- For `n = 16, 32, 64`, the L2 error decreases as the mesh is refined, reaching about `2.15e-4` at `n = 64`.
- For `n = 128, 256`, the error is larger because the iteration cap is reached before convergence.
- This benchmark therefore reflects both performance and the practical cost of Jacobi convergence on finer grids.

## Discussion

- Jacobi converges slowly, especially on the finer grids, so the largest cases are sensitive to the `MAX_ITER` cap.
- Small grids are dominated by MPI startup and communication overhead, so their speedup numbers are not very informative.
- Speedup on a laptop may be limited by shared resources, process-launch overhead, and the fact that this is not a cluster environment.
- More meaningful parallel behavior should be expected from cluster runs or from a machine designed for parallel workloads.
- The L2 error should decrease as `n` increases when the solver is allowed to converge adequately; if the iteration cap is reached first, the error trend can worsen even though the discretization itself is correct.

## Included Files

- `test/data/performance.csv`: raw benchmark table
- `test/data/time_vs_n.svg`: runtime plot
- `test/data/speedup_vs_n.svg`: speedup plot
- `test/data/l2_error_vs_n.svg`: L2-error plot

The plotting script uses only the Python standard library and writes SVG files.
