#pragma once

#include <vector>

namespace laplace {

/**
 * @brief Dense 2D grid of double values stored in row-major order.
 */
class Grid {
public:
    /**
     * @brief Construct a grid with the given number of rows and columns.
     *
     * All entries are initialized to initial_value.
     */
    Grid(int rows, int cols, double initial_value = 0.0);

    /**
     * @brief Return a modifiable reference to entry (i, j).
     */
    double& operator()(int i, int j);

    /**
     * @brief Return the value of entry (i, j).
     */
    double operator()(int i, int j) const;

    /**
     * @brief Return the number of rows.
     */
    int rows() const;

    /**
     * @brief Return the number of columns.
     */
    int cols() const;

    /**
     * @brief Fill the whole grid with a constant value.
     */
    void fill(double value);

    /**
     * @brief Return a constant reference to the underlying storage.
     */
    const std::vector<double>& data() const;

    /**
     * @brief Return a modifiable reference to the underlying storage.
     */
    std::vector<double>& data();

private:
    int rows_;
    int cols_;
    std::vector<double> data_;

    /**
     * @brief Convert 2D indices to the corresponding row-major 1D index.
     */
    int index(int i, int j) const;
};

}  // namespace laplace
