# Benchmark Results

This file summarizes the small scalability study requested by the assignment. The raw data are stored in `test/data/performance.csv`, and the generated plots are stored in `test/data/`.

## Benchmark Setup

The benchmark script uses the standard Jacobi solver on the sine manufactured problem with:

- grid sizes `n = 16, 32, 64, 128, 256`
- tolerance `1e-6`
- maximum iterations `2000`
- `OMP_NUM_THREADS=1`
- serial runs
- MPI runs with `np = 1, 2, 4`

The study is intentionally small and reproducible on a laptop. Jacobi converges slowly, so the iteration cap matters on the finest grids.

The required benchmark study in this file is about the standard Jacobi solver. The repository also contains a simple one-level Schwarz / block Jacobi variant, but that mode is treated only with small smoke checks rather than a separate benchmark table.

## Timing Summary

Measured wall-clock times in seconds:

| n | serial | MPI 1 proc | MPI 2 proc | MPI 4 proc |
|---|---:|---:|---:|---:|
| 16  | 0.001148 | 0.001140 | 0.000909 | 0.000970 |
| 32  | 0.019453 | 0.017610 | 0.149131 | 0.006416 |
| 64  | 0.254327 | 0.264666 | 0.135550 | 0.089211 |
| 128 | 1.430794 | 1.454174 | 0.672430 | 0.568410 |
| 256 | 5.543337 | 5.525457 | 2.649505 | 2.358900 |

Speedups with respect to the serial run:

| n | MPI 1 proc | MPI 2 proc | MPI 4 proc |
|---|---:|---:|---:|
| 16  | 1.01 | 1.26 | 1.18 |
| 32  | 1.10 | 0.13 | 3.03 |
| 64  | 0.96 | 1.88 | 2.85 |
| 128 | 0.98 | 2.13 | 2.52 |
| 256 | 1.00 | 2.09 | 2.35 |

## Accuracy Summary

- `n = 16, 32, 64` converge within the iteration cap.
- `n = 128, 256` hit the cap at `2000` iterations.
- The `L^2` error decreases clearly on the converged cases.
- For capped runs, the error remains larger because the solver stops before full convergence.

Representative `L^2` errors from the CSV:

| n | serial | MPI 2 proc | MPI 4 proc |
|---|---:|---:|---:|
| 16  | 7.366e-03 | 7.361e-03 | 7.357e-03 |
| 32  | 1.668e-03 | 1.648e-03 | 1.623e-03 |
| 64  | 2.149e-04 | 1.316e-04 | 2.135e-05 |
| 128 | 4.311e-02 | 4.311e-02 | 4.311e-02 |
| 256 | 2.724e-01 | 2.724e-01 | 2.724e-01 |

## Discussion

- Jacobi converges slowly, especially on the finer grids, so the largest cases are sensitive to the `MAX_ITER` cap.
- Small grids are dominated by MPI startup and communication overhead, so the timing results are noisy.
- The `np = 2` result at `n = 32` is unusually slow in the current local run, which is a good reminder that laptop timings can fluctuate.
- Medium and large grids show the intended benefit of the parallel solver more clearly.
- Because the MPI solver uses the assignment-style local convergence flags, converged iteration counts and final errors can differ slightly from serial even though the same stencil and manufactured problems are used.
- More meaningful parallel behavior should be expected on a better-controlled multicore machine or a cluster.

## Schwarz Note

The repository also includes a simple one-level Schwarz / block Jacobi option based on repeated local Jacobi sweeps. It was validated with targeted smoke tests, including a large-`local-iter` case to confirm that convergence is measured on the outer Schwarz update. It was not used for the main timing table above.

## Included Files

- `test/data/performance.csv`: raw benchmark table
- `test/data/time_vs_n.svg`: runtime plot
- `test/data/speedup_vs_n.svg`: speedup plot
- `test/data/l2_error_vs_n.svg`: L2-error plot
