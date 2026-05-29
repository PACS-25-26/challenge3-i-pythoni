# Submission Summary

This repository contains the completed required serial and MPI Jacobi solver together with the reproducibility material requested by the assignment.

## Completed Required Features

- matrix-free Jacobi solver in C++17
- user-selected grid size and manufactured problem case
- row-wise MPI decomposition with ghost-row exchange
- OpenMP directives for local loops
- convergence monitoring with a global MPI reduction
- VTK export
- unit tests
- small scalability benchmark script and result plots

## Completed Extra

- non-homogeneous Dirichlet boundary conditions through the polynomial and constant manufactured solutions

## Additional Simple Variant

- the required solver remains the standard Jacobi method used in the benchmark study
- a simple one-level Schwarz / block-Jacobi variant is also implemented through `--solver schwarz`
- the local solver is repeated matrix-free local Jacobi sweeps with fixed ghost rows, not LU
- `--local-iter` controls the number of local sweeps per outer Schwarz iteration
- convergence is measured on the outer Schwarz update, not inside the local relaxation
- this variant is presented as a small extra with smoke-test validation, not as a separate benchmark study

## Supporting Material

- reproducibility instructions: `test/README.md`
- benchmark discussion: `test/RESULT.md`
- hardware report: `hw.info` and `test/hw.info`

## Benchmark Snapshot

The included benchmark study uses the sine manufactured problem with `n = 16, 32, 64, 128, 256`, tolerance `1e-6`, maximum `2000` Jacobi iterations, and `OMP_NUM_THREADS=1`.

Main observations:

- serial and MPI Jacobi runs match numerically for the same grid size
- `n = 16, 32, 64` converge within the iteration cap
- `n = 128, 256` hit the `2000`-iteration cap, which is consistent with the slow convergence of Jacobi
- the `np = 4` MPI runs give a visible speedup for the medium and large cases, with the current sample results reaching about `2.65x` at `n = 128`

The full tables and generated plots are in `test/RESULT.md` and `test/data/`. The benchmark script uses standard shell tools and writes CSV and SVG files directly.
