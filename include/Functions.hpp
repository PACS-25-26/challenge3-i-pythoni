#pragma once

namespace laplace {

/**
 * @brief Supported manufactured solutions and forcing terms.
 */
enum class ProblemCase {
    /// Homogeneous Dirichlet sine verification case.
    Sine,
    /// Non-homogeneous Dirichlet polynomial case.
    Polynomial,
    /// Non-homogeneous Dirichlet constant-forcing case.
    Constant
};

/**
 * @brief Converts a command-line case name to a problem case.
 * @param name Command-line value such as `sine`, `poly`, or `constant`.
 * @return Matching manufactured problem identifier.
 */
ProblemCase parse_problem_case(const char* name);

/**
 * @brief Evaluates the source term of the manufactured problem.
 * @param x x-coordinate in the unit square.
 * @param y y-coordinate in the unit square.
 * @param problem_case Selected manufactured case.
 * @return Value of the forcing term `f(x,y)`.
 */
double source_term(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

/**
 * @brief Evaluates the Dirichlet boundary value at a boundary point.
 * @param x x-coordinate in the unit square.
 * @param y y-coordinate in the unit square.
 * @param problem_case Selected manufactured case.
 * @return Boundary value consistent with the exact solution.
 */
double boundary_value(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

/**
 * @brief Evaluates the exact manufactured solution.
 * @param x x-coordinate in the unit square.
 * @param y y-coordinate in the unit square.
 * @param problem_case Selected manufactured case.
 * @return Exact solution value `u(x,y)`.
 */
double exact_solution(double x, double y, ProblemCase problem_case = ProblemCase::Sine);

}  // namespace laplace
