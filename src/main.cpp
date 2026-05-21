#include "Grid.hpp"
#include "ParallelJacobiSolver.hpp"
#include "RowDecomposition.hpp"
#include "VTKWriter.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    laplace::ParallelJacobiConfig solver;
    std::string output_filename;
    bool print_decomposition = false;
};

void print_help(const std::string& program_name) {
    std::cout << "Usage: " << program_name
              << " [--n N] [--tol TOL] [--max-iter K] [--case sine|poly]"
              << " [--output FILE] [--print-decomposition]\n"
              << "MPI Jacobi solver for the 2D Laplace problem.\n";
}

Options parse_options(int argc, char* argv[]) {
    Options options;

    for (int arg = 1; arg < argc; ++arg) {
        const std::string name(argv[arg]);
        if (name == "--n" && arg + 1 < argc) {
            options.solver.n = std::atoi(argv[++arg]);
        } else if (name == "--tol" && arg + 1 < argc) {
            options.solver.tolerance = std::atof(argv[++arg]);
        } else if (name == "--max-iter" && arg + 1 < argc) {
            options.solver.max_iterations = std::atoi(argv[++arg]);
        } else if (name == "--case" && arg + 1 < argc) {
            options.solver.problem_case = laplace::parse_problem_case(argv[++arg]);
        } else if (name == "--output" && arg + 1 < argc) {
            options.output_filename = argv[++arg];
        } else if (name == "--print-decomposition") {
            options.print_decomposition = true;
        } else {
            throw std::invalid_argument("Unknown or incomplete option: " + name);
        }
    }

    if (options.solver.n < 2) {
        throw std::invalid_argument("N must be at least 2.");
    }
    if (options.solver.max_iterations < 0) {
        throw std::invalid_argument("Maximum iterations must be non-negative.");
    }
    if (options.solver.tolerance < 0.0) {
        throw std::invalid_argument("Tolerance must be non-negative.");
    }

    return options;
}

void print_decomposition(const laplace::RowDecomposition& decomposition,
                         int rank,
                         int size,
                         MPI_Comm communicator) {
    for (int current_rank = 0; current_rank < size; ++current_rank) {
        MPI_Barrier(communicator);
        if (rank == current_rank) {
            std::cout << "rank " << rank << "/" << size << " rows ["
                      << decomposition.local_begin() << ", " << decomposition.local_end() << ")"
                      << " local_rows=" << decomposition.local_rows() << '\n';
        }
    }
    MPI_Barrier(communicator);
}

}  // namespace

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int size = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 1 && std::string(argv[1]) == "--help") {
        if (rank == 0) {
            print_help(argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    try {
        const Options options = parse_options(argc, argv);
        if (size > options.solver.n) {
            throw std::invalid_argument("The number of MPI processes cannot exceed N.");
        }

        const laplace::RowDecomposition decomposition(
            static_cast<std::size_t>(options.solver.n), rank, size);

        if (options.print_decomposition) {
            print_decomposition(decomposition, rank, size, MPI_COMM_WORLD);
            MPI_Finalize();
            return 0;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        const auto start = std::chrono::steady_clock::now();
        const laplace::ParallelJacobiResult result =
            laplace::solve_parallel_jacobi(options.solver, decomposition, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        const auto end = std::chrono::steady_clock::now();

        if (rank == 0) {
            const std::chrono::duration<double> elapsed = end - start;

            std::cout << std::setprecision(10)
                      << "n: " << options.solver.n << '\n'
                      << "processes: " << size << '\n'
                      << "iterations: " << result.iterations << '\n'
                      << "converged: " << (result.converged ? "yes" : "no") << '\n'
                      << "final_increment: " << result.final_increment << '\n'
                      << "l2_error: " << result.l2_error << '\n'
                      << "time_seconds: " << elapsed.count() << '\n';

            if (!options.output_filename.empty()) {
                const laplace::Grid grid(options.solver.n, options.solver.n, 0.0);
                laplace::VTKWriter::write(options.output_filename, grid, result.global_solution);
                std::cout << "output: " << options.output_filename << '\n';
            }
        }
    } catch (const std::exception& error) {
        if (rank == 0) {
            std::cerr << "Error: " << error.what() << '\n';
        }
        MPI_Finalize();
        return 1;
    }

    MPI_Finalize();
    return 0;
}
