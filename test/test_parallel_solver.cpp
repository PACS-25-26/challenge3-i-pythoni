#include "ParallelJacobiSolver.hpp"

#include <cmath>

namespace {

bool close(double left, double right) {
    return std::abs(left - right) < 1.0e-12;
}

bool check_local_iter_one_matches_jacobi() {
    laplace::ParallelJacobiConfig jacobi_config;
    jacobi_config.n = 16;
    jacobi_config.max_iterations = 10;
    jacobi_config.tolerance = 0.0;
    jacobi_config.problem_case = laplace::ProblemCase::Sine;
    jacobi_config.local_solver = laplace::LocalSolver::PointJacobi;

    laplace::ParallelJacobiConfig schwarz_config = jacobi_config;
    schwarz_config.local_solver = laplace::LocalSolver::Schwarz;
    schwarz_config.local_iterations = 1;

    const laplace::RowDecomposition decomposition(
        static_cast<std::size_t>(jacobi_config.n), 0, 1);

    const laplace::ParallelJacobiResult jacobi_result =
        laplace::solve_parallel_jacobi(jacobi_config, decomposition, MPI_COMM_WORLD);
    const laplace::ParallelJacobiResult schwarz_result =
        laplace::solve_parallel_jacobi(schwarz_config, decomposition, MPI_COMM_WORLD);

    if (jacobi_result.iterations != schwarz_result.iterations) return false;
    if (jacobi_result.converged != schwarz_result.converged) return false;
    if (!close(jacobi_result.final_increment, schwarz_result.final_increment)) return false;
    if (!close(jacobi_result.l2_error, schwarz_result.l2_error)) return false;
    if (jacobi_result.global_solution.size() != schwarz_result.global_solution.size()) return false;

    for (std::size_t index = 0; index < jacobi_result.global_solution.size(); ++index) {
        if (!close(jacobi_result.global_solution[index], schwarz_result.global_solution[index])) {
            return false;
        }
    }

    return true;
}

bool check_schwarz_progresses_with_small_local_sweeps() {
    laplace::ParallelJacobiConfig jacobi_config;
    jacobi_config.n = 16;
    jacobi_config.max_iterations = 20;
    jacobi_config.tolerance = 0.0;
    jacobi_config.problem_case = laplace::ProblemCase::Sine;
    jacobi_config.local_solver = laplace::LocalSolver::PointJacobi;

    laplace::ParallelJacobiConfig schwarz_config = jacobi_config;
    schwarz_config.local_solver = laplace::LocalSolver::Schwarz;
    schwarz_config.local_iterations = 5;

    const laplace::RowDecomposition decomposition(
        static_cast<std::size_t>(jacobi_config.n), 0, 1);

    const laplace::ParallelJacobiResult jacobi_result =
        laplace::solve_parallel_jacobi(jacobi_config, decomposition, MPI_COMM_WORLD);
    const laplace::ParallelJacobiResult schwarz_result =
        laplace::solve_parallel_jacobi(schwarz_config, decomposition, MPI_COMM_WORLD);

    if (!(schwarz_result.l2_error < jacobi_result.l2_error)) return false;
    if (!(schwarz_result.final_increment > 0.0)) return false;

    return true;
}

bool check_schwarz_uses_outer_increment() {
    laplace::ParallelJacobiConfig config;
    config.n = 16;
    config.max_iterations = 1;
    config.tolerance = 1.0e-6;
    config.problem_case = laplace::ProblemCase::Sine;
    config.local_solver = laplace::LocalSolver::Schwarz;
    config.local_iterations = 500;

    const laplace::RowDecomposition decomposition(static_cast<std::size_t>(config.n), 0, 1);
    const laplace::ParallelJacobiResult result =
        laplace::solve_parallel_jacobi(config, decomposition, MPI_COMM_WORLD);

    if (result.iterations != 1) return false;
    if (result.converged) return false;
    if (!(result.final_increment > config.tolerance)) return false;
    if (!(result.final_increment > 1.0e-3)) return false;
    if (!(result.l2_error < 5.0e-2)) return false;

    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    bool success = check_local_iter_one_matches_jacobi();
    success = success && check_schwarz_progresses_with_small_local_sweeps();
    success = success && check_schwarz_uses_outer_increment();

    MPI_Finalize();
    return success ? 0 : 1;
}
