# Test And Reproducibility Material


This folder contains:

- small unit tests used during development
- the reproducible scalability script for the required Jacobi study
- the benchmark CSV file and generated SVG plots
- a short benchmark summary
- the hardware report for the current machine

## Contents

- `test_*.cpp`: unit tests for the grid, manufactured functions, serial solver, local solver parsing, row decomposition, error computation, VTK writer, and one MPI-aware parallel solver regression
- `run_scalability.sh`: shell benchmark script for the standard Jacobi solver
- `RESULT.md`: concise benchmark summary
- `hw.info`: hardware information generated on this machine
- `data/`: benchmark CSV file and SVG plots

## Build And Unit Tests

From the project root:

```bash
make clean
make
make test
```

The `make test` target builds all unit tests and runs them immediately. The MPI-aware test executable is launched through `mpirun -np 1` so that the parallel solver path is checked in a simple reproducible way.

## Scalability Script

Run the reproducibility script from the project root with:

```bash
bash test/run_scalability.sh
```

Default settings:

- problem case: `sine`
- grid sizes: `16 32 64 128 256`
- serial runs
- MPI runs with `np = 1, 2, 4`
- `OMP_NUM_THREADS=1`
- tolerance `1e-6`
- maximum iterations `2000`

The script is intentionally modest because plain Jacobi converges slowly on fine grids.

## Generated Files

The scalability script overwrites:

- `test/data/performance.csv`
- `test/data/time_vs_n.svg`
- `test/data/speedup_vs_n.svg`
- `test/data/l2_error_vs_n.svg`

The plots are generated directly by shell tools and `awk`. No extra plotting package is required.

## Reading `performance.csv`

The CSV columns are:

- `mode`: `serial` or `mpi`
- `n`: grid size
- `processes`: MPI process count
- `threads`: OpenMP thread count used during the run
- `iterations`: executed iterations
- `converged`: `yes` or `no`
- `final_increment`: reported stopping metric
- `l2_error`: discrete `L^2` error against the exact manufactured solution
- `time_seconds`: measured wall-clock time

For MPI runs, `final_increment` is the maximum local increment over all ranks. Global convergence requires all ranks to satisfy their local tolerance check.

## Hardware File

The benchmark in this repository was run on the machine described in `test/hw.info`. A copy is also available at the project root as `hw.info`.

Typical command used to generate the file:

```bash
lscpu > test/hw.info
```

## Notes On Runtime And Interpretation

- The required performance study uses the standard Jacobi solver, not the Schwarz mode.
- Small grids are strongly affected by launch overhead and operating-system noise.
- The finest cases are slower because Jacobi converges slowly and may hit the iteration cap.
- Results from another laptop or workstation can differ noticeably even when the implementation is unchanged.

## Small Schwarz Smoke Checks

The additional Schwarz / block Jacobi mode is validated with small smoke commands rather than a full benchmark campaign:

```bash
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver jacobi --max-iter 10
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 1 --max-iter 10
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 5 --max-iter 20
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 500 --max-iter 5 --tol 1e-6
```

These runs check that:

- `local-iter 1` behaves like standard Jacobi
- moderate local relaxation improves the solution on a small case
- very large `local-iter` values do not produce false convergence from the internal local sweeps alone
