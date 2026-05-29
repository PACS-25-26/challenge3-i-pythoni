#pragma once

namespace laplace {

/**
 * @brief Local solver mode used inside each MPI subdomain.
 */
enum class LocalSolver {
    /// Standard point Jacobi with one global sweep per outer iteration.
    PointJacobi,
    /// One-level Schwarz / block Jacobi with repeated local Jacobi sweeps.
    Schwarz
};

/**
 * @brief Converts a command-line local solver name to a local solver kind.
 * @param name Command-line value such as `jacobi` or `schwarz`.
 * @return Matching local solver identifier.
 */
LocalSolver parse_local_solver(const char* name);

/**
 * @brief Converts a local solver kind to its command-line name.
 * @param solver Local solver identifier.
 * @return Null-terminated command-line name.
 */
const char* local_solver_name(LocalSolver solver);

}  // namespace laplace
