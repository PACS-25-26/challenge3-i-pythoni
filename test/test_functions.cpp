#include "Functions.hpp"

#include <cmath>
#include <stdexcept>

namespace {

bool close(double left, double right) {
    return std::abs(left - right) < 1.0e-12;
}

}  // namespace

int main() {
    if (laplace::parse_problem_case("sine") != laplace::ProblemCase::Sine) return 1;
    if (laplace::parse_problem_case("poly") != laplace::ProblemCase::Polynomial) return 1;
    if (laplace::parse_problem_case("polynomial") != laplace::ProblemCase::Polynomial) return 1;

    try {
        (void)laplace::parse_problem_case("unknown");
        return 1;
    } catch (const std::invalid_argument&) {
    }

    if (!close(laplace::exact_solution(0.5, 0.25, laplace::ProblemCase::Polynomial), 0.3125)) return 1;
    if (!close(laplace::source_term(0.5, 0.25, laplace::ProblemCase::Polynomial), -4.0)) return 1;
    if (!close(laplace::boundary_value(0.5, 0.25, laplace::ProblemCase::Polynomial), 0.3125)) return 1;

    if (!close(laplace::exact_solution(0.25, 0.25, laplace::ProblemCase::Sine), 1.0)) return 1;
    if (!close(laplace::source_term(0.0, 0.5, laplace::ProblemCase::Sine), 0.0)) return 1;

    return 0;
}
