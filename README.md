[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/tKSbaXxd)
# Challenge 3

This repository contains the initial setup for Challenge 3 of the course project: a matrix-free parallel solver for the 2D Laplace equation.

## Goal

The planned objective is to implement a Jacobi iterative solver for the 2D Laplace equation on a structured grid, keeping the project simple and readable for an academic setting.

## Planned approach

- Discretization: 2D Cartesian grid
- Solver: matrix-free Jacobi iteration
- Parallelism: MPI row decomposition between processes
- Shared-memory acceleration: OpenMP loops inside each MPI rank
- Output: VTK files for visualization in ParaView

## Repository structure

```text
include/      Header files for the main project components
src/          Source files and executable entry points
test/         Future tests
test/data/    Input data for tests
output/       Generated output files
```

## Build

The project currently uses `mpic++` as compiler so that MPI support can be added later without changing the basic build flow.

```bash
make
```

Optional targets:

```bash
make parallel
make serial
make clean
```

## Run

```bash
./laplace_solver
./laplace_serial
```

Use `--help` with either executable to print the current skeleton help message.

## Current status

Step 0 only:

- project structure created
- placeholder headers and source files added
- build system added
- solver logic not implemented yet

Not implemented yet:

- Jacobi iteration
- MPI communication
- OpenMP parallel loops
- VTK output logic
- error computation
