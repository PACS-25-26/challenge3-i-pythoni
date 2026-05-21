#pragma once

namespace laplace {

/**
 * @brief Supported manufactured solutions and forcing terms.
 */
enum class ProblemCase {
    Sine,
    Polynomial
};

/**
 * @brief Converts a command-line case name to a problem case.
 */
ProblemCase parse_problem_case(const char* name);

/**
 * @brief Evaluates the source term of the Laplace or Poisson problem.
 */
double source_term(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

/**
 * @brief Evaluates a boundary condition at a point on the domain boundary.
 */
double boundary_value(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

/**
 * @brief Evaluates the exact solution for a manufactured problem case.
 */
double exact_solution(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

}  // namespace laplace
