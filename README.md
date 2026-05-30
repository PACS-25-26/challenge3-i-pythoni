# Challenge 3 — Matrix-free Parallel Solver for the Laplace Equation

This repository contains a C++17 implementation of a matrix-free solver for

$$
-\Delta u = f \quad \text{in } \Omega = (0,1)^2
$$

with Dirichlet boundary conditions on the unit square.

The project includes:

- a serial Jacobi solver in `laplace_serial`
- an MPI/OpenMP parallel solver in `laplace_solver`
- manufactured-solution verification cases
- VTK output readable by ParaView
- unit tests and a small reproducible benchmark workflow

## Project Goal

The required solver for the assignment is the standard matrix-free Jacobi method on a structured Cartesian grid:

- no global matrix is assembled
- the update is applied directly through the five-point stencil
- the MPI decomposition is row-wise and easy to explain
- OpenMP is used only on local compute loops

## Numerical Method

The domain is discretized with a uniform `n x n` Cartesian grid on `(0,1)^2`. The unknown is stored as dense grid values at the mesh points. Interior nodes are updated with the matrix-free Jacobi stencil

$$
U_{i,j}^{k+1} =
\frac{1}{4}\left(
U_{i-1,j}^{k}
+ U_{i+1,j}^{k}
+ U_{i,j-1}^{k}
+ U_{i,j+1}^{k}
+ h^2 f_{i,j}
\right).
$$

No global matrix is assembled anywhere in the code. Boundary values are set directly from the selected manufactured solution, and only interior nodes are updated.

The serial solver performs one Jacobi sweep per iteration. The MPI solver supports the same point Jacobi method and an additional simple Schwarz / block Jacobi variant based on repeated local Jacobi sweeps.

## Implemented Problem Cases

The documented command-line cases are:

- `--case sine`
- `--case poly`
- `--case constant`

### `--case sine`

This is the standard manufactured verification case:

$$
u(x,y) = \sin(2\pi x)\sin(2\pi y)
$$

$$
f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y)
$$

The exact solution vanishes on the boundary, so this case has homogeneous Dirichlet data.

### `--case poly`

This is a non-homogeneous Dirichlet manufactured case:

$$
u(x,y) = x^2 + y^2
$$

$$
f(x,y) = -4
$$

Boundary values are taken directly from the exact solution and are therefore nonzero on most of the boundary.

### `--case constant`

This is another non-homogeneous Dirichlet manufactured case:

$$
u(x,y) = \frac{1}{4}\left(x(1-x) + y(1-y)\right)
$$

$$
f(x,y) = 1
$$

Again, boundary values are taken directly from the exact solution.

## Parallel Strategy

The MPI solver splits the global rows into contiguous blocks. The distribution is balanced with the standard quotient/remainder rule:

- every rank receives `global_rows / size` rows
- the first `global_rows % size` ranks receive one additional row

Each MPI rank owns one contiguous block of physical rows. Two ghost rows are stored locally:

- one above the owned block
- one below the owned block

Adjacent ranks exchange the boundary rows of their owned blocks. The ghost rows are then used in the local stencil update. Rank 0 gathers the full solution at the end of the solve for error evaluation and optional VTK output.

OpenMP is used on the local compute loops, including:

- interior stencil updates
- local increment accumulation
- local `L^2` error accumulation

## Convergence

The serial solver uses the standard Jacobi increment norm

$$
h \sqrt{\sum (U^{k+1} - U^k)^2}.
$$

The MPI solver follows the assignment wording:

- each rank computes its own local increment norm
- each rank checks a local convergence flag
- local flags are reduced across all ranks with MPI
- global convergence is reached only if all ranks are locally converged
- the reported `final_increment` is the maximum local increment over all ranks

This is the active convergence logic in the current code. The MPI stopping criterion is therefore not described as a single globally accumulated norm.

The two MPI solver modes are:

- `--solver jacobi`: one global Jacobi sweep per outer iteration
- `--solver schwarz`: one outer Schwarz iteration consisting of ghost exchange followed by `--local-iter` local Jacobi sweeps with fixed ghost rows

In Schwarz mode the convergence test is measured on the outer update, not on the internal local sweeps.

## Output

The serial solver prints:

- `n`
- `iterations`
- `converged`
- `final_increment`
- `l2_error`
- `time_seconds`

The MPI solver prints:

- `n`
- `processes`
- `solver`
- `local_iterations`
- `iterations`
- `converged`
- `final_increment`
- `l2_error`
- `time_seconds`

If `--output FILE` is provided, the solution is written to a legacy ASCII VTK file readable by ParaView. The writer uses `DATASET STRUCTURED_POINTS` with:

- `DIMENSIONS n n 1`
- `ORIGIN 0 0 0`
- `SPACING h h 1`

The option `--print-decomposition` prints the row interval owned by each rank and exits.

## Build

The project is built with `mpic++` and uses C++17.

```bash
make clean
make
make test
```

This builds:

- `laplace_serial`
- `laplace_solver`
- the unit test executables used by `make test`

For MPI runs, the number of processes must not exceed `n`.

## Run Examples

Serial examples:

```bash
./laplace_serial --case sine --n 32 --max-iter 500 --tol 1e-6
./laplace_serial --case poly --n 32 --max-iter 500 --tol 1e-6
./laplace_serial --case constant --n 32 --max-iter 500 --tol 1e-6
```

Parallel Jacobi examples:

```bash
mpirun -np 4 ./laplace_solver --case sine --n 32 --max-iter 500 --tol 1e-6
mpirun -np 4 ./laplace_solver --case constant --n 32 --max-iter 500 --tol 1e-6
```

Parallel Schwarz / block Jacobi example:

```bash
mpirun -np 2 ./laplace_solver --case sine --n 32 --solver schwarz --local-iter 5 --max-iter 100 --tol 1e-6
```

VTK output:

```bash
./laplace_serial --case sine --n 32 --max-iter 500 --tol 1e-6 --output output/serial.vtk
mpirun -np 4 ./laplace_solver --case sine --n 32 --max-iter 500 --tol 1e-6 --output output/parallel.vtk
```

Decomposition print:

```bash
mpirun -np 4 ./laplace_solver --n 11 --print-decomposition
```

## Tests And Benchmark Workflow

Unit tests are run with:

```bash
make test
```

The benchmark workflow is a small shell-based scalability script:

```bash
bash test/run_scalability.sh
```

The default benchmark settings are:

- `n = 16, 32, 64, 128, 256`
- standard Jacobi solver on the `sine` case
- serial runs
- MPI runs with `np = 1, 2, 4`
- `OMP_NUM_THREADS=1`
- `tol = 1e-6`
- `max-iter = 2000`

The script writes:

- `test/data/performance.csv`
- `test/data/time_vs_n.svg`
- `test/data/speedup_vs_n.svg`
- `test/data/l2_error_vs_n.svg`

The CSV and SVG plots are generated directly by shell tools and `awk`. Results are machine-dependent, especially on a laptop or desktop system where MPI startup and shared-resource effects can dominate small runs.

## Extras

Completed extras:

- non-homogeneous Dirichlet boundary handling through function-based boundary values
- the additional `poly` and `constant` manufactured cases
- a simple one-level Schwarz / block Jacobi variant in the MPI solver

The Schwarz variant is intentionally modest:

- enabled with `--solver schwarz --local-iter K`
- still matrix-free
- uses repeated local Jacobi sweeps, not LU
- keeps ghost rows fixed during each local relaxation phase
- measures convergence on the outer Schwarz update
- is not part of the main scalability benchmark unless explicitly stated

## Repository Structure

```text
include/               Public headers
src/                   Source files and executable entry points
test/                  Unit tests and reproducibility material
test/data/             Benchmark CSV file and generated SVG plots
output/                Optional VTK output directory
Makefile               Build rules
RESULT.md              Short final report
hw.info                Hardware report for the current machine
Challenge25-26-3.pdf   Assignment specification
```

## Supporting Files

- `RESULT.md` contains the short final report
- `test/README.md` contains reproducibility instructions
- `test/RESULT.md` contains the benchmark discussion
