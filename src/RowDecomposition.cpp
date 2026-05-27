#include "RowDecomposition.hpp"

namespace laplace {

RowDecomposition::RowDecomposition(std::size_t global_rows, int rank, int size)
    : global_rows_(global_rows), rank_(rank), size_(size) {}

std::size_t RowDecomposition::global_rows() const {
    return global_rows_;
}

std::size_t RowDecomposition::local_begin() const {
    const std::size_t base_rows = global_rows_ / static_cast<std::size_t>(size_);
    const std::size_t extra_rows = global_rows_ % static_cast<std::size_t>(size_);
    const std::size_t rank = static_cast<std::size_t>(rank_);

    return rank * base_rows + (rank < extra_rows ? rank : extra_rows);
}

std::size_t RowDecomposition::local_end() const {
    const std::size_t base_rows = global_rows_ / static_cast<std::size_t>(size_);
    const std::size_t extra_rows = global_rows_ % static_cast<std::size_t>(size_);
    const std::size_t rank = static_cast<std::size_t>(rank_);

    return local_begin() + base_rows + (rank < extra_rows ? 1 : 0);
}

}  // namespace laplace
