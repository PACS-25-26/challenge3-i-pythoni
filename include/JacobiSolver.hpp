#pragma once

#include <vector>

namespace laplace {

class Grid;
class RowDecomposition;

/**
 * @brief Skeleton for a matrix-free Jacobi solver.
 *
 * The implementation will later manage iterations, convergence checks,
 * and the local solution storage updates.
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
     * @brief Placeholder solve routine for the future solver.
     * @param grid Global grid description.
     * @param decomposition Local row decomposition.
     * @param values Solution storage.
     */
    void solve(const Grid& grid, const RowDecomposition& decomposition, std::vector<double>& values) const;

private:
    int max_iterations_;
    double tolerance_;
};

}  // namespace laplace
