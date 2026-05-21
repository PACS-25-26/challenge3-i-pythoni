#include "Grid.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace laplace {

Grid::Grid(int rows, int cols, double initial_value)
    : rows_(rows), cols_(cols), data_() {
    if (rows_ < 0 || cols_ < 0) {
        throw std::invalid_argument("Grid dimensions must be non-negative.");
    }

    data_.assign(static_cast<std::size_t>(rows_) * static_cast<std::size_t>(cols_), initial_value);
}

double& Grid::operator()(int i, int j) {
    return data_[static_cast<std::size_t>(index(i, j))];
}

double Grid::operator()(int i, int j) const {
    return data_[static_cast<std::size_t>(index(i, j))];
}

int Grid::rows() const {
    return rows_;
}

int Grid::cols() const {
    return cols_;
}

void Grid::fill(double value) {
    std::fill(data_.begin(), data_.end(), value);
}

const std::vector<double>& Grid::data() const {
    return data_;
}

std::vector<double>& Grid::data() {
    return data_;
}

int Grid::index(int i, int j) const {
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
        throw std::out_of_range("Grid index out of range.");
    }

    return i * cols_ + j;
}

}  // namespace laplace
