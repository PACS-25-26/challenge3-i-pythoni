#include "JacobiSolver.hpp"

#include "Grid.hpp"
#include "RowDecomposition.hpp"

namespace laplace {

JacobiSolver::JacobiSolver(int max_iterations, double tolerance)
    : max_iterations_(max_iterations), tolerance_(tolerance) {}

void JacobiSolver::solve(const Grid&, const RowDecomposition&, std::vector<double>&) const {
    (void)max_iterations_;
    (void)tolerance_;
}

}  // namespace laplace
