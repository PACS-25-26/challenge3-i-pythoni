# Final Report

## Summary Of The Implementation

The repository contains the required matrix-free solver for

$$
-\Delta u = f \quad \text{in } \Omega = (0,1)^2
$$

with Dirichlet boundary conditions on the unit square.

Implemented features:

- serial Jacobi solver in `laplace_serial`
- MPI/OpenMP solver in `laplace_solver`
- row-wise domain decomposition with ghost-row exchange
- matrix-free five-point Jacobi stencil
- discrete `L^2` error against manufactured exact solutions
- VTK output readable by ParaView
- unit tests and a reproducible benchmark script

Completed extras:

- non-homogeneous Dirichlet boundary conditions through function-based boundary values
- additional `poly` and `constant` manufactured cases
- a simple one-level Schwarz / block Jacobi variant based on repeated local Jacobi sweeps

The required benchmark study remains the standard Jacobi solver. The Schwarz variant is included as an additional simple solver mode, not as the main benchmarked method.

## Correctness Discussion

The code uses manufactured solutions to verify both the discretization and the boundary handling:

- `--case sine` is the standard smooth verification case
- `--case poly` and `--case constant` check non-homogeneous Dirichlet data

The implementation remains matrix-free throughout: no global matrix is assembled, and the update is applied directly through the stencil.

Serial and MPI solvers use the same stencil, forcing terms, and boundary values. The MPI solver follows the assignment wording for convergence:

- each rank computes its own local increment norm
- each rank checks a local convergence flag
- MPI combines the flags across ranks
- global convergence is reached only if all ranks are locally converged
- the reported `final_increment` is the maximum local increment over all ranks

Because the MPI stopping rule is based on local flags instead of the serial global increment norm, converged iteration counts can differ slightly between serial and MPI runs even when the computed solutions remain close. This is expected and consistent with the current code.

The `L^2` error decreases clearly on the converged sine-problem runs:

- `n = 16`: about `7.37e-3`
- `n = 32`: about `1.67e-3`
- `n = 64`: about `2.15e-4` in serial and `2.13e-5` for the current `np = 4` run

For `n = 128` and `n = 256`, the iteration cap is reached before convergence, so the error is larger. This reflects the slow convergence of Jacobi rather than an implementation error.

## Performance Discussion

The timing summary below was taken directly from `test/data/performance.csv`.

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

Main observations:

- Jacobi is slow, especially on finer grids.
- Small grids are strongly affected by MPI startup and communication overhead.
- The current local-machine results are hardware-dependent and somewhat irregular, especially for `n = 32`, where the `np = 2` run is slower than expected.
- Larger grids are more meaningful for parallel timing than the smallest case.
- On this machine the more stable medium and large cases show speedups in the `2x` to `2.85x` range.

These local timings are useful for reproducibility, but they should not be interpreted as ideal cluster-quality scaling results. This is consistent with the assignment note that small cases are overhead-dominated and that better scaling should be expected on more suitable hardware.

## Extras Discussion

The completed non-homogeneous Dirichlet extra is implemented through function-based manufactured boundary values:

- `poly` uses `u(x,y) = x^2 + y^2`
- `constant` uses `u(x,y) = 0.25(x(1-x) + y(1-y))`

The additional Schwarz mode is implemented honestly as a simple one-level block Jacobi / Schwarz variant:

- each rank owns a contiguous block of rows
- neighboring ranks exchange ghost rows at each outer iteration
- `--local-iter K` means `K` local Jacobi sweeps inside the local block
- ghost rows stay fixed during those local sweeps
- the local solver is still matrix-free and does not assemble or factor local matrices
- convergence is measured on the outer Schwarz update

This mode is present as a defensible extra, but it is not the primary method of the required benchmark study. It has been checked with unit tests and small smoke runs rather than a dedicated performance campaign.

## Supporting Material

- `README.md`: project overview and usage
- `test/README.md`: reproducibility instructions
- `test/RESULT.md`: benchmark-oriented summary
- `test/data/performance.csv`: raw timing and accuracy data
- `hw.info` and `test/hw.info`: hardware information
