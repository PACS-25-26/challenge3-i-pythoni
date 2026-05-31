#pragma once

#include "Functions.hpp"

namespace laplace {

class Grid;

/**
 * @brief Computes the discrete L2 error against the exact manufactured solution.
 * @param solution Numerical solution on the full grid.
 * @param problem_case Manufactured case used for the exact solution.
 * @return Discrete `L^2` error scaled with the mesh spacing.
 */
double compute_l2_error(const Grid& solution, ProblemCase problem_case = ProblemCase::Sine);

}  // namespace laplace
