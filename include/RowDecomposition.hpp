#pragma once

#include <cstddef>

namespace laplace {

/**
 * @brief Describes a simple row-wise domain decomposition.
 *
 * The local range is represented as [local_begin, local_end), using global
 * zero-based row indices.
 */
class RowDecomposition {
public:
    /**
     * @brief Constructs a row decomposition descriptor.
     * @param global_rows Total number of rows in the global grid.
     * @param rank MPI rank identifier.
     * @param size Total number of MPI ranks.
     */
    RowDecomposition(std::size_t global_rows = 0, int rank = 0, int size = 1);

    /**
     * @brief Returns the total number of global rows.
     */
    std::size_t global_rows() const;

    /**
     * @brief Returns the local starting row index.
     */
    std::size_t local_begin() const;

    /**
     * @brief Returns the local ending row index.
     */
    std::size_t local_end() const;

    /**
     * @brief Returns the number of rows owned by this rank.
     */
    std::size_t local_rows() const;

private:
    std::size_t global_rows_;
    int rank_;
    int size_;
    std::size_t local_begin_;
    std::size_t local_end_;
};

}  // namespace laplace
