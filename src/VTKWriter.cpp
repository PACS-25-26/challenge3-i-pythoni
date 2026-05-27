#include "VTKWriter.hpp"

#include "Grid.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace laplace {

void VTKWriter::write(const std::string& filename, const Grid& grid, const std::vector<double>& values) {
    if (grid.rows() != grid.cols()) {
        throw std::invalid_argument("VTK output expects a square grid.");
    }
    if (grid.rows() < 2) {
        throw std::invalid_argument("VTK output expects at least two points per direction.");
    }
    if (values.size() != static_cast<std::size_t>(grid.rows() * grid.cols())) {
        throw std::invalid_argument("VTK output received a field with the wrong size.");
    }

    std::ofstream output(filename);
    if (!output) {
        throw std::runtime_error("Unable to open VTK output file: " + filename);
    }

    const int n = grid.rows();
    const double h = 1.0 / static_cast<double>(n - 1);

    output << "# vtk DataFile Version 3.0\n"
           << "Laplace solution\n"
           << "ASCII\n"
           << "DATASET STRUCTURED_POINTS\n"
           << "DIMENSIONS " << n << " " << n << " 1\n"
           << "ORIGIN 0 0 0\n"
           << "SPACING " << h << " " << h << " 1\n"
           << "POINT_DATA " << n * n << '\n'
           << "SCALARS u double 1\n"
           << "LOOKUP_TABLE default\n";

    output << std::setprecision(16);
    for (double value : values) {
        output << value << '\n';
    }
}

}  // namespace laplace
