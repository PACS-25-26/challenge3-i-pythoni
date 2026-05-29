# Test And Reproducibility Material

This folder collects both the small unit tests used during development and the reproducibility material requested by the assignment.

## Contents

- `test_*.cpp`: small C++ unit tests for the grid, manufactured functions, Jacobi solver, row decomposition, error computation, and VTK writer.
- `run_scalability.sh`: benchmark script for the required serial and MPI Jacobi runs; it also writes the CSV file and the SVG plots.
- `RESULT.md`: short discussion of the benchmark results.
- `hw.info`: hardware report generated with `lscpu` on this machine.
- `data/`: benchmark CSV files and generated plots.

## Build The Project

From the project root:

```bash
make clean
make
```

This builds:

- `laplace_serial`
- `laplace_solver`

## Run The Unit Tests

From the project root:

```bash
make test
```

## Run The Scalability Study

From the project root:

```bash
./test/run_scalability.sh
```

Default benchmark settings:

- problem case: `sine`
- grid sizes: `16 32 64 128 256`
- serial runs
- MPI runs with `np = 1, 2, 4`
- `OMP_NUM_THREADS=1` by default
- tolerance `1e-6`
- maximum iterations `2000`

The script writes:

- `test/data/performance.csv`

Representative benchmark outputs are intentionally kept under `test/data/` as small reproducible sample results. Re-running the script overwrites them with the results from the current machine.

The default settings are intentionally modest because Jacobi converges slowly on the finest grids. You can adjust them without editing the script by setting environment variables before running it, for example:

```bash
MAX_ITER=4000 TOL=1e-7 THREADS=1 ./test/run_scalability.sh
```

## Generated Plot Files

The benchmark script reads and writes `test/data/performance.csv` and also creates SVG plots in `test/data/`.

Typical output files are:

- `test/data/performance.csv`
- `test/data/time_vs_n.svg`
- `test/data/speedup_vs_n.svg`
- `test/data/l2_error_vs_n.svg`

The benchmark script uses standard shell tools and writes CSV and SVG files directly.

## Hardware Used

The benchmark in this repository was run on the machine described in `test/hw.info`. A copy is also available at the project root as `hw.info`.

On this machine the hardware report was generated with:

```bash
lscpu > test/hw.info
```

If `lscpu` is unavailable, the fallback command is:

```bash
cat /proc/cpuinfo > test/hw.info
```

## Notes

- The required performance study uses the standard Jacobi solver, not the Schwarz / block-Jacobi variant.
- The completed extra is the non-homogeneous Dirichlet manufactured cases. The repository also includes a simple one-level Schwarz / block-Jacobi variant that stays matrix-free by using repeated local Jacobi sweeps instead of LU.
- In Schwarz mode, `--local-iter` is the number of local Jacobi sweeps per outer iteration, and convergence is checked on the outer update after those sweeps.
- The manufactured-function unit tests cover the non-homogeneous Dirichlet cases, while the benchmark study uses the sine manufactured solution for a clean serial/MPI comparison.
- Results on a normal laptop may not show ideal parallel speedup. This is consistent with the assignment note: small cases are overhead-dominated, and more meaningful parallel behavior should be expected on a proper cluster.

## Small Schwarz Smoke Tests

These are intentionally small checks for the additional Schwarz / block-Jacobi mode:

```bash
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver jacobi --max-iter 10
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 1 --max-iter 10
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 5 --max-iter 20
mpirun -np 2 ./laplace_solver --n 16 --case sine --solver schwarz --local-iter 500 --max-iter 5 --tol 1e-6
```

The last command is meant to check that large local iteration counts do not falsely report convergence from the internal local solver alone. The convergence test is based on the outer Schwarz update.
