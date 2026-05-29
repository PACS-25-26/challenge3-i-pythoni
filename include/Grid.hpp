#pragma once

#include <vector>

namespace laplace {

/**
 * @brief Dense 2D grid of scalar values stored in row-major order.
 *
 * This class is used both for the full serial solution and for local MPI
 * subdomains with ghost rows.
 */
class Grid {
public:
    /**
     * @brief Constructs a grid with the given number of rows and columns.
     * @param rows Number of grid rows.
     * @param cols Number of grid columns.
     * @param initial_value Value used to initialize all entries.
     */
    Grid(int rows, int cols, double initial_value = 0.0);

    /**
     * @brief Returns a modifiable reference to entry `(i, j)`.
     * @param i Row index.
     * @param j Column index.
     * @return Reference to the requested entry.
     */
    double& operator()(int i, int j);

    /**
     * @brief Returns the value stored at entry `(i, j)`.
     * @param i Row index.
     * @param j Column index.
     * @return Value of the requested entry.
     */
    double operator()(int i, int j) const;

    /**
     * @brief Returns the number of rows.
     */
    int rows() const;

    /**
     * @brief Returns the number of columns.
     */
    int cols() const;

    /**
     * @brief Fills the whole grid with a constant value.
     * @param value Value assigned to every entry.
     */
    void fill(double value);

    /**
     * @brief Returns a constant reference to the underlying storage.
     */
    const std::vector<double>& data() const;

    /**
     * @brief Returns a modifiable reference to the underlying storage.
     */
    std::vector<double>& data();

private:
    int rows_;
    int cols_;
    std::vector<double> data_;

    /**
     * @brief Converts 2D indices to the corresponding row-major 1D index.
     */
    int index(int i, int j) const;
};

}  // namespace laplace
