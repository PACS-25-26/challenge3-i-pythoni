#include "Functions.hpp"

int main() {
    if (laplace::source_term(0.0, 0.0) != 0.0) return 1;
    if (laplace::source_term(0.25, 0.75) != 0.0) return 1;

    if (laplace::boundary_value(0.0, 0.0) != 0.0) return 1;
    if (laplace::boundary_value(1.0, 0.5) != 0.0) return 1;

    return 0;
}
