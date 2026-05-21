#pragma once

namespace laplace {

/**
 * @brief Evaluates the source term of the Laplace or Poisson problem.
 *
 * This function will later define the right-hand side used in test cases.
 */
double source_term(double x, double y);

/**
 * @brief Evaluates a boundary condition at a point on the domain boundary.
 *
 * This function will later provide problem-specific boundary values.
 */
double boundary_value(double x, double y);

}  // namespace laplace
