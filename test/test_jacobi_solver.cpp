#include "Grid.hpp"
#include "JacobiSolver.hpp"

#include <cmath>
#include <stdexcept>

namespace {

bool close(double left, double right) {
    return std::abs(left - right) < 1.0e-12;
}

}  // namespace

int main() {
    laplace::Grid solution(3, 3, 0.0);
    laplace::JacobiSolver solver(1, 0.0);

    const laplace::JacobiResult result =
        solver.solve_serial(solution, laplace::ProblemCase::Polynomial);

    if (result.iterations != 1) return 1;
    if (result.converged) return 1;
    if (!close(result.final_increment, 0.25)) return 1;

    if (!close(solution(0, 0), 0.0)) return 1;
    if (!close(solution(0, 1), 0.25)) return 1;
    if (!close(solution(1, 0), 0.25)) return 1;
    if (!close(solution(1, 1), 0.5)) return 1;
    if (!close(solution(2, 2), 2.0)) return 1;

    laplace::Grid constant_solution(3, 3, 0.0);
    const laplace::JacobiResult constant_result =
        solver.solve_serial(constant_solution, laplace::ProblemCase::Constant);

    if (constant_result.iterations != 1) return 1;
    if (constant_result.converged) return 1;
    if (!close(constant_result.final_increment, 0.0625)) return 1;
    if (!close(constant_solution(0, 1), 0.0625)) return 1;
    if (!close(constant_solution(1, 0), 0.0625)) return 1;
    if (!close(constant_solution(1, 1), 0.125)) return 1;
    if (!close(constant_solution(1, 2), 0.0625)) return 1;
    if (!close(constant_solution(2, 1), 0.0625)) return 1;

    laplace::Grid converged_solution(3, 3, 0.0);
    laplace::JacobiSolver fast_solver(1, 1.0);
    const laplace::JacobiResult converged_result =
        fast_solver.solve_serial(converged_solution, laplace::ProblemCase::Polynomial);
    if (!converged_result.converged) return 1;

    try {
        laplace::JacobiSolver invalid_solver(-1, 0.0);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    try {
        laplace::Grid non_square_solution(3, 4, 0.0);
        solver.solve_serial(non_square_solution);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    try {
        laplace::Grid too_small_solution(1, 1, 0.0);
        solver.solve_serial(too_small_solution);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    return 0;
}
