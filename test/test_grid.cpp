#include "Grid.hpp"

#include <stdexcept>

int main() {
    laplace::Grid grid(2, 3, 1.5);
    if (grid.rows() != 2) return 1;
    if (grid.cols() != 3) return 1;
    if (grid.data().size() != 6) return 1;

    for (double value : grid.data()) {
        if (value != 1.5) return 1;
    }

    grid(1, 2) = 4.0;
    if (grid(1, 2) != 4.0) return 1;
    if (grid.data()[5] != 4.0) return 1;

    grid.fill(-2.0);
    if (grid(0, 0) != -2.0) return 1;
    if (grid(1, 2) != -2.0) return 1;

    try {
        laplace::Grid invalid_grid(-1, 3);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    try {
        (void)grid(2, 0);
        return 1;
    } catch (const std::out_of_range&) {
    }

    return 0;
}
