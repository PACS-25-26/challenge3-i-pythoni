#pragma once

#include <vector>

namespace laplace {

class Grid;

/**
 * @brief Computes simple error measures for the numerical solution.
 *
 * The implementation will later compare the computed field with a reference.
 */
double compute_max_error(const Grid& grid, const std::vector<double>& values);

}  // namespace laplace
