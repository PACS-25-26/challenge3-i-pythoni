#include "Grid.hpp"

int main() {
    laplace::Grid default_grid;
    if (default_grid.nx() != 0) return 1;
    if (default_grid.ny() != 0) return 1;
    if (default_grid.lx() != 1.0) return 1;
    if (default_grid.ly() != 1.0) return 1;

    laplace::Grid grid(5, 7, 2.0, 3.0);
    if (grid.nx() != 5) return 1;
    if (grid.ny() != 7) return 1;
    if (grid.lx() != 2.0) return 1;
    if (grid.ly() != 3.0) return 1;

    return 0;
}
