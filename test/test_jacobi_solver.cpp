#include "Grid.hpp"
#include "JacobiSolver.hpp"
#include "RowDecomposition.hpp"

#include <vector>

int main() {
    laplace::Grid grid(3, 3, 1.0, 1.0);
    laplace::RowDecomposition decomposition(grid.ny(), 0, 1);
    laplace::JacobiSolver solver(10, 1.0e-8);

    std::vector<double> values = {1.0, 2.0, 3.0, 4.0};
    const std::vector<double> expected = values;

    solver.solve(grid, decomposition, values);

    if (values != expected) return 1;

    return 0;
}
