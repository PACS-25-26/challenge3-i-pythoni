#include "Grid.hpp"

#include <iostream>
#include <string>

namespace {

void print_help(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [--help] [--test-grid]\n"
              << "Serial Laplace solver skeleton\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--help") {
        print_help(argv[0]);
        return 0;
    }

    if (argc > 1 && std::string(argv[1]) == "--test-grid") {
        laplace::Grid grid(3, 4, 1.0);
        grid(1, 2) = 5.0;

        std::cout << "Grid rows: " << grid.rows() << '\n'
                  << "Grid cols: " << grid.cols() << '\n'
                  << "grid(1, 2): " << grid(1, 2) << '\n';
        return 0;
    }

    std::cout << "Serial Laplace solver skeleton\n";
    return 0;
}
