[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)

# Challenge 3 — Matrix-free parallel solver for the Laplace equation

This repository contains a C++17 implementation of a matrix-free Jacobi solver for the two-dimensional Laplace/Poisson problem on the unit square

\[
-\Delta u = f \quad \text{in } \Omega = (0,1)^2
\]

with Dirichlet boundary conditions. The project includes both a serial solver and a parallel MPI solver based on row-wise domain decomposition, with OpenMP directives on local compute loops.

## Implemented features

- Matrix-free Jacobi iteration on a structured Cartesian grid
- Serial solver executable: `laplace_serial`
- MPI parallel solver executable: `laplace_solver`
- Row-wise MPI domain decomposition with balanced row distribution
- Ghost row exchange between neighboring MPI ranks
- Global convergence check based on `MPI_Allreduce` of the global increment norm
- OpenMP directives on local update and error-computation loops
- Manufactured sine test case
- Manufactured polynomial case with non-homogeneous Dirichlet boundary conditions
- Discrete L2 error computation against the exact solution
- VTK output for ParaView-compatible visualization
- Simple command-line interface for grid size, tolerance, iteration count, problem case, and output file

## Manufactured problems

The main verification case is the sine manufactured solution:

\[
f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y)
\]

\[
u(x,y) = \sin(2\pi x)\sin(2\pi y)
\]

The code also includes a polynomial manufactured case to exercise non-homogeneous Dirichlet boundary conditions.

## Repository structure

```text
include/               Header files for the main project components
src/                   Source files and executable entry points
test/                  Placeholder directory for future tests
test/data/             Placeholder input data directory for tests
output/                Output directory for generated files
Makefile               Build rules for the serial and MPI executables
Challenge25-26-3.pdf   Assignment specification
```

## Build instructions

The project is built with `mpic++` and uses C++17.

```bash
make clean
make
```

This produces:

- `laplace_serial`
- `laplace_solver`

Optional targets:

```bash
make serial
make parallel
```

## Run instructions

Serial help:

```bash
./laplace_serial --help
```

MPI help:

```bash
mpirun -np 2 ./laplace_solver --help
```

Example serial run:

```bash
./laplace_serial --n 32 --max-iter 500 --tol 1e-6 --case sine
```

Example MPI run:

```bash
mpirun -np 4 ./laplace_solver --n 32 --max-iter 500 --tol 1e-6 --case sine
```

Example VTK output:

```bash
./laplace_serial --n 32 --max-iter 500 --tol 1e-6 --case sine --output output/serial.vtk
mpirun -np 4 ./laplace_solver --n 32 --max-iter 500 --tol 1e-6 --case sine --output output/parallel.vtk
```

## Current status

The current code base already includes the core serial and MPI Jacobi solvers, manufactured test cases, error computation, OpenMP local parallelism, and VTK output.

The project is not finished yet. In particular, the repository does not yet include a dedicated testing suite, benchmark scripts, or a complete performance study. Those parts can be added later as separate steps.
