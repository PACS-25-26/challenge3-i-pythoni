#include "Functions.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace laplace {

namespace {

constexpr double pi = 3.141592653589793238462643383279502884;

}  // namespace

ProblemCase parse_problem_case(const char* name) {
    const std::string value(name);
    if (value == "sine") {
        return ProblemCase::Sine;
    }
    if (value == "poly" || value == "polynomial") {
        return ProblemCase::Polynomial;
    }
    if (value == "constant") {
        return ProblemCase::Constant;
    }

    throw std::invalid_argument("Unknown problem case: " + value);
}

double source_term(double x, double y, ProblemCase problem_case) {
    switch (problem_case) {
        case ProblemCase::Sine:
            return 8.0 * pi * pi * std::sin(2.0 * pi * x) * std::sin(2.0 * pi * y);
        case ProblemCase::Polynomial:
            return -4.0;
        case ProblemCase::Constant:
            return 1.0;
    }

    return 0.0;
}

double boundary_value(double x, double y, ProblemCase problem_case) {
    return exact_solution(x, y, problem_case);
}

double exact_solution(double x, double y, ProblemCase problem_case) {
    switch (problem_case) {
        case ProblemCase::Sine:
            return std::sin(2.0 * pi * x) * std::sin(2.0 * pi * y);
        case ProblemCase::Polynomial:
            return x * x + y * y;
        case ProblemCase::Constant:
            return 0.25 * (x * (1.0 - x) + y * (1.0 - y));
    }

    return 0.0;
}

}  // namespace laplace
