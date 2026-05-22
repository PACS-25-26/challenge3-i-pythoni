#pragma once

#include "Functions.hpp"
#include "RowDecomposition.hpp"

#ifndef OMPI_SKIP_MPICXX
#define OMPI_SKIP_MPICXX 1
#endif
#include <mpi.h>

#include <vector>

namespace laplace {

/**
 * @brief Configuration values for the MPI Jacobi solver.
 */
struct ParallelJacobiConfig {
    int n = 64;
    int max_iterations = 10000;
    double tolerance = 1.0e-6;
    ProblemCase problem_case = ProblemCase::Sine;
};

/**
 * @brief Summary and gathered output of an MPI Jacobi solve.
 *
 * The global_solution vector is filled only on rank 0.
 */
struct ParallelJacobiResult {
    int iterations = 0;
    double final_increment = 0.0;
    double l2_error = 0.0;
    bool converged = false;
    std::vector<double> global_solution;
};

/**
 * @brief Solves the 2D Laplace problem with MPI row decomposition and ghost rows.
 */
ParallelJacobiResult solve_parallel_jacobi(const ParallelJacobiConfig& config,
                                           const RowDecomposition& decomposition,
                                           MPI_Comm communicator);

}  // namespace laplace
