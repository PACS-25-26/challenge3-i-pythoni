#include "RowDecomposition.hpp"

#include <stdexcept>

int main() {
    laplace::RowDecomposition single_rank(8, 0, 1);
    if (single_rank.global_rows() != 8) return 1;
    if (single_rank.local_begin() != 0) return 1;
    if (single_rank.local_end() != 8) return 1;
    if (single_rank.local_rows() != 8) return 1;

    laplace::RowDecomposition first_rank(10, 0, 3);
    if (first_rank.local_begin() != 0) return 1;
    if (first_rank.local_end() != 4) return 1;
    if (first_rank.local_rows() != 4) return 1;

    laplace::RowDecomposition middle_rank(10, 1, 3);
    if (middle_rank.local_begin() != 4) return 1;
    if (middle_rank.local_end() != 7) return 1;
    if (middle_rank.local_rows() != 3) return 1;

    laplace::RowDecomposition last_rank(10, 2, 3);
    if (last_rank.local_begin() != 7) return 1;
    if (last_rank.local_end() != 10) return 1;
    if (last_rank.local_rows() != 3) return 1;

    laplace::RowDecomposition exact_division(12, 2, 3);
    if (exact_division.local_begin() != 8) return 1;
    if (exact_division.local_end() != 12) return 1;
    if (exact_division.local_rows() != 4) return 1;

    laplace::RowDecomposition empty_rank(3, 4, 5);
    if (empty_rank.local_begin() != 3) return 1;
    if (empty_rank.local_end() != 3) return 1;
    if (empty_rank.local_rows() != 0) return 1;

    try {
        laplace::RowDecomposition invalid_size(3, 0, 0);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    try {
        laplace::RowDecomposition invalid_rank(3, 3, 3);
        return 1;
    } catch (const std::invalid_argument&) {
    }

    return 0;
}
