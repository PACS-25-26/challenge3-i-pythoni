#pragma once

#include <cstddef>

namespace laplace {

/**
 * @brief Describes a simple row-wise domain decomposition.
 *
 * This class will later store the local row range handled by each MPI rank.
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

private:
    std::size_t global_rows_;
    int rank_;
    int size_;
};

}  // namespace laplace
