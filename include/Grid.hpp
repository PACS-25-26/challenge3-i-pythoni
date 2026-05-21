#pragma once

#include <cstddef>

namespace laplace {

/**
 * @brief Stores the basic geometry of a 2D Cartesian grid.
 *
 * This class will later provide the grid metadata needed by the solver.
 */
class Grid {
public:
    /**
     * @brief Constructs a grid description.
     * @param nx Number of grid points in the x direction.
     * @param ny Number of grid points in the y direction.
     * @param lx Domain length in the x direction.
     * @param ly Domain length in the y direction.
     */
    Grid(std::size_t nx = 0, std::size_t ny = 0, double lx = 1.0, double ly = 1.0);

    /**
     * @brief Returns the number of grid points in the x direction.
     */
    std::size_t nx() const;

    /**
     * @brief Returns the number of grid points in the y direction.
     */
    std::size_t ny() const;

    /**
     * @brief Returns the domain length in the x direction.
     */
    double lx() const;

    /**
     * @brief Returns the domain length in the y direction.
     */
    double ly() const;

private:
    std::size_t nx_;
    std::size_t ny_;
    double lx_;
    double ly_;
};

}  // namespace laplace
