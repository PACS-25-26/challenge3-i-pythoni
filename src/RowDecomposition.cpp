#include "RowDecomposition.hpp"

#include <stdexcept>

namespace laplace {

RowDecomposition::RowDecomposition(std::size_t global_rows, int rank, int size)
    : global_rows_(global_rows),
      rank_(rank),
      size_(size),
      local_begin_(0),
      local_end_(0) {
    if (size_ <= 0) {
        throw std::invalid_argument("MPI size must be positive.");
    }
    if (rank_ < 0 || rank_ >= size_) {
        throw std::invalid_argument("MPI rank must be in [0, size).");
    }

    const std::size_t base_rows = global_rows_ / static_cast<std::size_t>(size_);
    const std::size_t remainder = global_rows_ % static_cast<std::size_t>(size_);
    const std::size_t rank_as_size = static_cast<std::size_t>(rank_);

    local_begin_ = rank_as_size * base_rows + (rank_as_size < remainder ? rank_as_size : remainder);
    local_end_ = local_begin_ + base_rows + (rank_as_size < remainder ? 1U : 0U);
}

std::size_t RowDecomposition::global_rows() const {
    return global_rows_;
}

std::size_t RowDecomposition::local_begin() const {
    return local_begin_;
}

std::size_t RowDecomposition::local_end() const {
    return local_end_;
}

std::size_t RowDecomposition::local_rows() const {
    return local_end_ - local_begin_;
}

}  // namespace laplace
