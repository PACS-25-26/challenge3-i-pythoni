#include "RowDecomposition.hpp"

namespace laplace {

RowDecomposition::RowDecomposition(std::size_t global_rows, int rank, int size)
    : global_rows_(global_rows), rank_(rank), size_(size) {}

std::size_t RowDecomposition::global_rows() const {
    return global_rows_;
}

std::size_t RowDecomposition::local_begin() const {
    return 0;
}

std::size_t RowDecomposition::local_end() const {
    return 0;
}

}  // namespace laplace
