#pragma once

#include "Functions.hpp"

namespace laplace {

class Grid;

/**
 * @brief Summary of a Jacobi solve.
 */
struct JacobiResult {
    int iterations = 0;
    double final_increment = 0.0;
    bool converged = false;
};

/**
 * @brief Matrix-free Jacobi solver for the 2D Laplace problem.
 */
class JacobiSolver {
public:
    /**
     * @brief Constructs a solver configuration.
     * @param max_iterations Maximum number of Jacobi iterations.
     * @param tolerance Convergence tolerance.
     */
    JacobiSolver(int max_iterations = 0, double tolerance = 0.0);

    /**
     * @brief Solves the problem on a full serial grid.
     * @param solution Grid storing the computed solution on return.
     * @param problem_case Manufactured forcing and boundary case.
     */
    JacobiResult solve_serial(Grid& solution, ProblemCase problem_case = ProblemCase::Sine) const;

private:
    int max_iterations_;
    double tolerance_;
};

}  // namespace laplace
