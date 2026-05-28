# Test And Reproducibility Material

This folder collects both the small unit tests used during development and the reproducibility material requested by the assignment.

## Contents

- `test_*.cpp`: small C++ unit tests for the grid, manufactured functions, Jacobi solver, row decomposition, error computation, and VTK writer.
- `run_scalability.sh`: benchmark script for the required serial and MPI Jacobi runs.
- `plot_results.py`: plotting script for the benchmark CSV file.
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

## Generate The Plots

From the project root:

```bash
python3 test/plot_results.py
```

The plotting script reads `test/data/performance.csv`.

It writes SVG plots in `test/data/`.

Typical output files are:

- `test/data/performance.csv`
- `test/data/time_vs_n.svg`
- `test/data/speedup_vs_n.svg`
- `test/data/l2_error_vs_n.svg`

The plotting script uses only the Python standard library and writes SVG files.

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

- The required performance study uses the standard Jacobi solver, not the experimental Schwarz mode.
- The non-homogeneous Dirichlet polynomial case is covered by the unit tests, while the scalability study uses the sine manufactured solution for a clean serial/MPI comparison.
- Results on a normal laptop may not show ideal parallel speedup. This is consistent with the assignment note: small cases are overhead-dominated, and more meaningful parallel behavior should be expected on a proper cluster.
