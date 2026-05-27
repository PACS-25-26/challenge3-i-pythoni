#include "JacobiSolver.hpp"

#include "Functions.hpp"
#include "Grid.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace laplace {

JacobiSolver::JacobiSolver(int max_iterations, double tolerance)
    : max_iterations_(max_iterations), tolerance_(tolerance) {
    if (max_iterations_ < 0) {
        throw std::invalid_argument("Maximum iterations must be non-negative.");
    }
    if (tolerance_ < 0.0) {
        throw std::invalid_argument("Tolerance must be non-negative.");
    }
}

JacobiResult JacobiSolver::solve_serial(Grid& solution, ProblemCase problem_case) const {
    if (solution.rows() != solution.cols()) {
        throw std::invalid_argument("The serial solver expects a square grid.");
    }
    if (solution.rows() < 2) {
        throw std::invalid_argument("The grid must contain at least two points per direction.");
    }

    const int n = solution.rows();
    const double h = 1.0 / static_cast<double>(n - 1);
    const double h_squared = h * h;

    Grid old_values(n, n, 0.0);
    Grid new_values(n, n, 0.0);

    for (int i = 0; i < n; ++i) {
        const double y = static_cast<double>(i) * h;
        for (int j = 0; j < n; ++j) {
            const double x = static_cast<double>(j) * h;
            if (i == 0 || i == n - 1 || j == 0 || j == n - 1) {
                const double value = boundary_value(x, y, problem_case);
                old_values(i, j) = value;
                new_values(i, j) = value;
            }
        }
    }

    JacobiResult result;

    for (int iteration = 1; iteration <= max_iterations_; ++iteration) {
        double increment_squared = 0.0;

#pragma omp parallel for reduction(+ : increment_squared)
        for (int i = 1; i < n - 1; ++i) {
            const double y = static_cast<double>(i) * h;
            for (int j = 1; j < n - 1; ++j) {
                const double x = static_cast<double>(j) * h;
                const double next_value =
                    0.25 * (old_values(i - 1, j) + old_values(i + 1, j) + old_values(i, j - 1) +
                            old_values(i, j + 1) + h_squared * source_term(x, y, problem_case));
                const double difference = next_value - old_values(i, j);

                new_values(i, j) = next_value;
                increment_squared += difference * difference;
            }
        }

        result.iterations = iteration;
        result.final_increment = h * std::sqrt(increment_squared);
        if (result.final_increment < tolerance_) {
            result.converged = true;
            break;
        }

        std::swap(old_values.data(), new_values.data());
    }

    if (result.converged) {
        solution = new_values;
    } else {
        solution = old_values;
    }

    return result;
}

}  // namespace laplace
