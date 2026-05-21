#include <iostream>
#include <string>

namespace {

void print_help(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [--help]\n"
              << "Laplace MPI/OpenMP solver skeleton\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--help") {
        print_help(argv[0]);
        return 0;
    }

    std::cout << "Laplace MPI/OpenMP solver skeleton\n";
    return 0;
}
