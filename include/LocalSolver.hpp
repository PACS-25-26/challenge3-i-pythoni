#pragma once

namespace laplace {

/**
 * @brief Local solver used inside each MPI subdomain.
 */
enum class LocalSolver {
    PointJacobi,
    Schwarz
};

/**
 * @brief Converts a command-line local solver name to a local solver kind.
 */
LocalSolver parse_local_solver(const char* name);

/**
 * @brief Converts a local solver kind to its command-line name.
 */
const char* local_solver_name(LocalSolver solver);

}  // namespace laplace
