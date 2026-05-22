#include "ParallelJacobiSolver.hpp"

#include "Grid.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace laplace {
namespace {

void initialize_local_grid(Grid& old_values,
                           Grid& new_values,
                           const RowDecomposition& decomposition,
                           int n,
                           ProblemCase problem_case) {
    const double h = 1.0 / static_cast<double>(n - 1);

    for (int local_i = 1; local_i <= static_cast<int>(decomposition.local_rows()); ++local_i) {
        const int global_i = static_cast<int>(decomposition.local_begin()) + local_i - 1;
        const double y = static_cast<double>(global_i) * h;

        for (int j = 0; j < n; ++j) {
            const double x = static_cast<double>(j) * h;
            if (global_i == 0 || global_i == n - 1 || j == 0 || j == n - 1) {
                const double value = boundary_value(x, y, problem_case);
                old_values(local_i, j) = value;
                new_values(local_i, j) = value;
            }
        }
    }
}

void exchange_ghost_rows(Grid& values,
                         const RowDecomposition& decomposition,
                         int rank,
                         int size,
                         MPI_Comm communicator) {
    const int n = values.cols();
    const int local_rows = static_cast<int>(decomposition.local_rows());
    const int top_rank = rank == 0 ? MPI_PROC_NULL : rank - 1;
    const int bottom_rank = rank == size - 1 ? MPI_PROC_NULL : rank + 1;

    MPI_Sendrecv(&values(1, 0),
                 n,
                 MPI_DOUBLE,
                 top_rank,
                 0,
                 &values(local_rows + 1, 0),
                 n,
                 MPI_DOUBLE,
                 bottom_rank,
                 0,
                 communicator,
                 MPI_STATUS_IGNORE);

    MPI_Sendrecv(&values(local_rows, 0),
                 n,
                 MPI_DOUBLE,
                 bottom_rank,
                 1,
                 &values(0, 0),
                 n,
                 MPI_DOUBLE,
                 top_rank,
                 1,
                 communicator,
                 MPI_STATUS_IGNORE);
}

double compute_global_l2_error(const std::vector<double>& values, int n, ProblemCase problem_case) {
    const double h = 1.0 / static_cast<double>(n - 1);
    double error_squared = 0.0;

    for (int i = 0; i < n; ++i) {
        const double y = static_cast<double>(i) * h;
        for (int j = 0; j < n; ++j) {
            const double x = static_cast<double>(j) * h;
            const double difference =
                values[static_cast<std::size_t>(i * n + j)] - exact_solution(x, y, problem_case);
            error_squared += difference * difference;
        }
    }

    return std::sqrt(h * error_squared);
}

std::vector<double> gather_solution(const Grid& local_values,
                                    const RowDecomposition& decomposition,
                                    int n,
                                    int rank,
                                    int size,
                                    MPI_Comm communicator) {
    const int local_count = static_cast<int>(decomposition.local_rows()) * n;
    std::vector<int> receive_counts(size, 0);
    std::vector<int> displacements(size, 0);

    MPI_Gather(&local_count, 1, MPI_INT, receive_counts.data(), 1, MPI_INT, 0, communicator);

    std::vector<double> global_values;
    if (rank == 0) {
        int offset = 0;
        for (int process = 0; process < size; ++process) {
            displacements[process] = offset;
            offset += receive_counts[process];
        }
        global_values.resize(static_cast<std::size_t>(n * n));
    }

    MPI_Gatherv(local_values.data().data() + n,
                local_count,
                MPI_DOUBLE,
                global_values.data(),
                receive_counts.data(),
                displacements.data(),
                MPI_DOUBLE,
                0,
                communicator);

    return global_values;
}

}  // namespace

ParallelJacobiResult solve_parallel_jacobi(const ParallelJacobiConfig& config,
                                           const RowDecomposition& decomposition,
                                           MPI_Comm communicator) {
    if (config.n < 2) {
        throw std::invalid_argument("N must be at least 2.");
    }
    if (config.max_iterations < 0) {
        throw std::invalid_argument("Maximum iterations must be non-negative.");
    }
    if (config.tolerance < 0.0) {
        throw std::invalid_argument("Tolerance must be non-negative.");
    }

    int rank = 0;
    int size = 1;
    MPI_Comm_rank(communicator, &rank);
    MPI_Comm_size(communicator, &size);

    const int n = config.n;
    const int local_rows = static_cast<int>(decomposition.local_rows());
    const double h = 1.0 / static_cast<double>(n - 1);
    const double h_squared = h * h;

    Grid old_values(local_rows + 2, n, 0.0);
    Grid new_values(local_rows + 2, n, 0.0);
    initialize_local_grid(old_values, new_values, decomposition, n, config.problem_case);

    ParallelJacobiResult result;

    for (int iteration = 1; iteration <= config.max_iterations; ++iteration) {
        exchange_ghost_rows(old_values, decomposition, rank, size, communicator);

        double local_increment_squared = 0.0;

        for (int local_i = 1; local_i <= local_rows; ++local_i) {
            const int global_i = static_cast<int>(decomposition.local_begin()) + local_i - 1;
            if (global_i == 0 || global_i == n - 1) {
                continue;
            }

            const double y = static_cast<double>(global_i) * h;
            for (int j = 1; j < n - 1; ++j) {
                const double x = static_cast<double>(j) * h;
                const double next_value =
                    0.25 * (old_values(local_i - 1, j) + old_values(local_i + 1, j) +
                            old_values(local_i, j - 1) + old_values(local_i, j + 1) +
                            h_squared * source_term(x, y, config.problem_case));
                const double difference = next_value - old_values(local_i, j);

                new_values(local_i, j) = next_value;
                local_increment_squared += difference * difference;
            }
        }

        double global_increment_squared = 0.0;
        MPI_Allreduce(&local_increment_squared,
                      &global_increment_squared,
                      1,
                      MPI_DOUBLE,
                      MPI_SUM,
                      communicator);

        result.iterations = iteration;
        result.final_increment = std::sqrt(h * global_increment_squared);
        result.converged = result.final_increment < config.tolerance;
        if (result.converged) {
            break;
        }

        std::swap(old_values.data(), new_values.data());
    }

    const Grid& final_values = result.converged ? new_values : old_values;
    result.global_solution = gather_solution(final_values, decomposition, n, rank, size, communicator);

    if (rank == 0) {
        result.l2_error = compute_global_l2_error(result.global_solution, n, config.problem_case);
    }

    return result;
}

}  // namespace laplace
