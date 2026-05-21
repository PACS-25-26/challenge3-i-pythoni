#pragma once

#include "Functions.hpp"

namespace laplace {

class Grid;

/**
 * @brief Computes the discrete L2 error against the exact manufactured solution.
 */
double compute_l2_error(const Grid& solution, ProblemCase problem_case = ProblemCase::Sine);

}  // namespace laplace
