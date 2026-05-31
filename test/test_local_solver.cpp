#include "LocalSolver.hpp"

#include <stdexcept>
#include <string>

int main() {
    if (laplace::parse_local_solver("jacobi") != laplace::LocalSolver::PointJacobi) return 1;
    if (laplace::parse_local_solver("schwarz") != laplace::LocalSolver::Schwarz) return 1;

    if (std::string(laplace::local_solver_name(laplace::LocalSolver::PointJacobi)) != "jacobi") {
        return 1;
    }
    if (std::string(laplace::local_solver_name(laplace::LocalSolver::Schwarz)) != "schwarz") {
        return 1;
    }

    try {
        (void)laplace::parse_local_solver("invalid");
        return 1;
    } catch (const std::invalid_argument&) {
    }

    return 0;
}
