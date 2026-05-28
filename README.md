[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)

# Challenge 3 — Matrix-free parallel solver for the Laplace equation

This repository contains a C++17 implementation of a matrix-free Jacobi solver for the two-dimensional problem

$$
-\Delta u = f \quad \text{in } \Omega = (0,1)^2
$$

with Dirichlet boundary conditions on the unit square. The project includes both a serial solver and an MPI parallel solver with row-wise domain decomposition and OpenMP directives on the local compute loops.

## Required Features Implemented

- matrix-free Jacobi iteration on a structured Cartesian grid
- serial executable: `laplace_serial`
- MPI executable: `laplace_solver`
- user-selected grid size `n`
- user-selected manufactured problem case
- balanced row-wise MPI decomposition
- ghost-row exchange between neighboring ranks
- global convergence check based on the reduced increment norm
- OpenMP directives on the local update and error loops
- discrete L2 error against the exact manufactured solution
- VTK export readable by ParaView
- reproducibility material for a small scalability study under `test/`

## Manufactured Problems

### Sine verification case

$$
f(x,y) = 8\pi^2 \sin(2\pi x)\sin(2\pi y)
$$

$$
u(x,y) = \sin(2\pi x)\sin(2\pi y)
$$

### Polynomial case

The code also includes the manufactured solution

$$
u(x,y) = x^2 + y^2
$$

with source term

$$
f(x,y) = -4
$$

This case is used to exercise non-homogeneous Dirichlet boundary conditions.

## Extras

### Completed extra

- non-homogeneous Dirichlet boundary conditions through the polynomial manufactured case

### Experimental / partial extra

- Schwarz / block-Jacobi local iteration mode is included for experimentation through `--solver schwarz`
- this mode is not used in the required benchmark study
- it should be considered partial rather than a completed extra
- it is not fully validated, and large `--local-iter` values can produce misleading convergence reports

## Repository Structure

```text
include/               Header files for the main components
src/                   Source files and executable entry points
test/                  Unit tests and reproducibility material
test/data/             Benchmark CSV files and generated plots
output/                Output directory for generated VTK files
Makefile               Build rules for the serial and MPI executables
RESULT.md              Short submission summary
hw.info                Hardware report
Challenge25-26-3.pdf   Assignment specification
```

## Build Instructions

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
make test
```

## Run Instructions

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

## Reproducibility Material

The `test/` folder contains:

- unit tests for the main numerical and utility components
- `test/run_scalability.sh` for the small serial/MPI benchmark study and SVG plot generation
- `test/RESULT.md` for the benchmark discussion
- `test/hw.info` for the hardware report

The repository keeps a small representative `performance.csv` file and plot set under `test/data/` as reproducible sample outputs. Re-running the benchmark script overwrites those files with the current machine results.

Typical workflow:

```bash
./test/run_scalability.sh
```

More detailed instructions are in `test/README.md`.
