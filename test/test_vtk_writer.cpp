#include "Grid.hpp"
#include "VTKWriter.hpp"

#include <fstream>
#include <cstdio>
#include <stdexcept>
#include <string>

int main() {
    const std::string filename = "test/data/test_vtk_writer_output.vtk";

    laplace::Grid grid(2, 2, 0.0);
    grid(0, 0) = 1.0;
    grid(0, 1) = 2.0;
    grid(1, 0) = 3.0;
    grid(1, 1) = 4.0;

    laplace::VTKWriter::write(filename, grid, grid.data());

    std::ifstream input(filename);
    if (!input) return 1;

    std::string contents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    if (contents.find("# vtk DataFile Version 3.0") == std::string::npos) return 1;
    if (contents.find("DIMENSIONS 2 2 1") == std::string::npos) return 1;
    if (contents.find("POINT_DATA 4") == std::string::npos) return 1;
    if (contents.find("1\n2\n3\n4\n") == std::string::npos) return 1;

    try {
        laplace::Grid non_square_grid(2, 3, 0.0);
        laplace::VTKWriter::write(filename, non_square_grid, non_square_grid.data());
        return 1;
    } catch (const std::invalid_argument&) {
    }

    try {
        laplace::VTKWriter::write(filename, grid, {1.0, 2.0, 3.0});
        return 1;
    } catch (const std::invalid_argument&) {
    }

    std::remove(filename.c_str());

    return 0;
}
