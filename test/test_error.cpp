#include "Error.hpp"
#include "Grid.hpp"

#include <cmath>
#include <stdexcept>

namespace {

bool close(double left, double right) {
    return std::abs(left - right) < 1.0e-12;
}

}  // namespace

int main() {
    laplace::Grid exact_polynomial(3, 3, 0.0);
    exact_polynomial(0, 0) = 0.0;
    exact_polynomial(0, 1) = 0.25;
    exact_polynomial(0, 2) = 1.0;
    exact_polynomial(1, 0) = 0.25;
    exact_polynomial(1, 1) = 0.5;
    exact_polynomial(1, 2) = 1.25;
    exact_polynomial(2, 0) = 1.0;
    exact_polynomial(2, 1) = 1.25;
    exact_polynomial(2, 2) = 2.0;

    if (!close(laplace::compute_l2_error(exact_polynomial, laplace::ProblemCase::Polynomial), 0.0)) {
        return 1;
    }

    laplace::Grid shifted_solution = exact_polynomial;
    shifted_solution(1, 1) += 2.0;
    if (!close(laplace::compute_l2_error(shifted_solution, laplace::ProblemCase::Polynomial), 1.0)) {
        return 1;
    }

    try {
        laplace::Grid non_square_solution(2, 3, 0.0);
        laplace::compute_l2_error(non_square_solution);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    return 0;
}
