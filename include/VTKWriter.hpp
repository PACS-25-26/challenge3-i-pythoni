#pragma once

#include <string>
#include <vector>

namespace laplace {

class Grid;

/**
 * @brief Writes scalar solution fields to legacy ASCII VTK files.
 *
 * The output format is `STRUCTURED_POINTS`, which is directly readable by
 * ParaView for the structured grids used in this project.
 */
class VTKWriter {
public:
    /**
     * @brief Writes a scalar field to a VTK file.
     * @param filename Output file path.
     * @param grid Grid metadata.
     * @param values Scalar values on the grid.
     * @throws std::invalid_argument If the grid is not square or the field size is inconsistent.
     * @throws std::runtime_error If the file cannot be opened for writing.
     */
    static void write(const std::string& filename, const Grid& grid, const std::vector<double>& values);
};

}  // namespace laplace
