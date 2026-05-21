#include "Grid.hpp"

namespace laplace {

Grid::Grid(std::size_t nx, std::size_t ny, double lx, double ly)
    : nx_(nx), ny_(ny), lx_(lx), ly_(ly) {}

std::size_t Grid::nx() const {
    return nx_;
}

std::size_t Grid::ny() const {
    return ny_;
}

double Grid::lx() const {
    return lx_;
}

double Grid::ly() const {
    return ly_;
}

}  // namespace laplace
