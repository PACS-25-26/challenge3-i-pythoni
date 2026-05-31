#include "Error.hpp"
#include "Functions.hpp"
#include "Grid.hpp"
#include "JacobiSolver.hpp"
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
    int n = 64;
    int max_iterations = 10000;
    double tolerance = 1.0e-6;
    laplace::ProblemCase problem_case = laplace::ProblemCase::Sine;
    std::string output_filename;
    bool test_grid = false;
};

void print_help(const std::string& program_name) {
    std::cout << "Usage: " << program_name
              << " [--n N] [--tol TOL] [--max-iter K] [--case sine|poly|constant]"
              << " [--output FILE] [--test-grid]\n"
              << "Serial Jacobi solver for the 2D Laplace problem.\n";
}

Options parse_options(int argc, char* argv[]) {
    Options options;

    for (int arg = 1; arg < argc; ++arg) {
        const std::string name(argv[arg]);
        if (name == "--test-grid") {
            options.test_grid = true;
        } else if (name == "--n" && arg + 1 < argc) {
            options.n = std::atoi(argv[++arg]);
        } else if (name == "--tol" && arg + 1 < argc) {
            options.tolerance = std::atof(argv[++arg]);
        } else if (name == "--max-iter" && arg + 1 < argc) {
            options.max_iterations = std::atoi(argv[++arg]);
        } else if (name == "--case" && arg + 1 < argc) {
            options.problem_case = laplace::parse_problem_case(argv[++arg]);
        } else if (name == "--output" && arg + 1 < argc) {
            options.output_filename = argv[++arg];
        } else {
            throw std::invalid_argument("Unknown or incomplete option: " + name);
        }
    }

    if (options.n < 2) {
        throw std::invalid_argument("N must be at least 2.");
    }
    if (options.max_iterations < 0) {
        throw std::invalid_argument("Maximum iterations must be non-negative.");
    }
    if (options.tolerance < 0.0) {
        throw std::invalid_argument("Tolerance must be non-negative.");
    }

    return options;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--help") {
        print_help(argv[0]);
        return 0;
    }

    try {
        const Options options = parse_options(argc, argv);

        if (options.test_grid) {
        laplace::Grid grid(3, 4, 1.0);
        grid(1, 2) = 5.0;

        std::cout << "Grid rows: " << grid.rows() << '\n'
                  << "Grid cols: " << grid.cols() << '\n'
                  << "grid(1, 2): " << grid(1, 2) << '\n';
        return 0;
    }

        laplace::Grid solution(options.n, options.n, 0.0);
        const laplace::JacobiSolver solver(options.max_iterations, options.tolerance);

        const auto start = std::chrono::steady_clock::now();
        const laplace::JacobiResult result = solver.solve_serial(solution, options.problem_case);
        const auto end = std::chrono::steady_clock::now();

        const std::chrono::duration<double> elapsed = end - start;
        const double l2_error = laplace::compute_l2_error(solution, options.problem_case);

        std::cout << std::setprecision(10)
                  << "n: " << options.n << '\n'
                  << "iterations: " << result.iterations << '\n'
                  << "converged: " << (result.converged ? "yes" : "no") << '\n'
                  << "final_increment: " << result.final_increment << '\n'
                  << "l2_error: " << l2_error << '\n'
                  << "time_seconds: " << elapsed.count() << '\n';

        if (!options.output_filename.empty()) {
            laplace::VTKWriter::write(options.output_filename, solution, solution.data());
            std::cout << "output: " << options.output_filename << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
