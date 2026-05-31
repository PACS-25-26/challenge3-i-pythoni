#include "LocalSolver.hpp"

#include <stdexcept>
#include <string>

namespace laplace {

LocalSolver parse_local_solver(const char* name) {
    const std::string value(name);
    if (value == "jacobi") {
        return LocalSolver::PointJacobi;
    }
    if (value == "schwarz") {
        return LocalSolver::Schwarz;
    }

    throw std::invalid_argument("Unknown local solver: " + value);
}

const char* local_solver_name(LocalSolver solver) {
    switch (solver) {
        case LocalSolver::PointJacobi:
            return "jacobi";
        case LocalSolver::Schwarz:
            return "schwarz";
    }

    return "unknown";
}

}  // namespace laplace
