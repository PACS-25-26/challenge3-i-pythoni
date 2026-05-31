#pragma once

#include "Functions.hpp"
#include "LocalSolver.hpp"
#include "RowDecomposition.hpp"

#ifndef OMPI_SKIP_MPICXX
#define OMPI_SKIP_MPICXX 1
#endif
#include <mpi.h>

#include <vector>

namespace laplace {

/**
 * @brief Configuration values for the MPI solver.
 */
struct ParallelJacobiConfig {
    /// Global grid size in each coordinate direction.
    int n = 64;
    /// Maximum number of outer iterations.
    int max_iterations = 10000;
    /// Stopping tolerance applied to the local increment norm.
    double tolerance = 1.0e-6;
    /// Manufactured problem case used for forcing and boundary data.
    ProblemCase problem_case = ProblemCase::Sine;
    /// Outer solver mode: point Jacobi or simple Schwarz / block Jacobi.
    LocalSolver local_solver = LocalSolver::PointJacobi;
    /// Number of local Jacobi sweeps per outer Schwarz iteration.
    int local_iterations = 10;
};

/**
 * @brief Summary and gathered output of an MPI solve.
 *
 * The global_solution vector is filled only on rank 0.
 */
struct ParallelJacobiResult {
    /// Number of outer iterations executed.
    int iterations = 0;
    /// Maximum local increment over all ranks at the final iteration.
    double final_increment = 0.0;
    /// Discrete `L^2` error, computed on rank 0 after gathering the solution.
    double l2_error = 0.0;
    /// True if all ranks satisfied the local stopping criterion.
    bool converged = false;
    /// Gathered full-grid solution on rank 0.
    std::vector<double> global_solution;
};

/**
 * @brief Solves the 2D Laplace problem with MPI row decomposition and ghost rows.
 * @param config Solver configuration.
 * @param decomposition Row ownership descriptor for the current rank.
 * @param communicator MPI communicator used by the solver.
 * @return Iteration summary together with the gathered solution on rank 0.
 */
ParallelJacobiResult solve_parallel_jacobi(const ParallelJacobiConfig& config,
                                           const RowDecomposition& decomposition,
                                           MPI_Comm communicator);

}  // namespace laplace
