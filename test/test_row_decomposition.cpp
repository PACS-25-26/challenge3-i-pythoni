#include "RowDecomposition.hpp"

int main() {
    laplace::RowDecomposition single_rank(8, 0, 1);
    if (single_rank.global_rows() != 8) return 1;
    if (single_rank.local_begin() != 0) return 1;
    if (single_rank.local_end() != 8) return 1;

    laplace::RowDecomposition first_rank(10, 0, 3);
    if (first_rank.local_begin() != 0) return 1;
    if (first_rank.local_end() != 4) return 1;

    laplace::RowDecomposition middle_rank(10, 1, 3);
    if (middle_rank.local_begin() != 4) return 1;
    if (middle_rank.local_end() != 7) return 1;

    laplace::RowDecomposition last_rank(10, 2, 3);
    if (last_rank.local_begin() != 7) return 1;
    if (last_rank.local_end() != 10) return 1;

    return 0;
}
