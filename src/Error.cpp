#include "Error.hpp"

#include "Functions.hpp"
#include "Grid.hpp"

#include <cmath>
#include <stdexcept>

namespace laplace {

double compute_l2_error(const Grid& solution, ProblemCase problem_case) {
    if (solution.rows() != solution.cols()) {
        throw std::invalid_argument("L2 error computation expects a square grid.");
    }
    if (solution.rows() < 2) {
        throw std::invalid_argument("The grid must contain at least two points per direction.");
    }

    const int n = solution.rows();
    const double h = 1.0 / static_cast<double>(n - 1);
    double error_squared = 0.0;

    for (int i = 0; i < n; ++i) {
        const double y = static_cast<double>(i) * h;
        for (int j = 0; j < n; ++j) {
            const double x = static_cast<double>(j) * h;
            const double difference = solution(i, j) - exact_solution(x, y, problem_case);
            error_squared += difference * difference;
        }
    }

    return h * std::sqrt(error_squared);
}

}  // namespace laplace
