#pragma once

#include <string>
#include <vector>

namespace laplace {

class Grid;

/**
 * @brief Writes solution fields to VTK files.
 *
 * This utility will later export grid-based results for ParaView.
 */
class VTKWriter {
public:
    /**
     * @brief Writes a scalar field to a VTK file.
     * @param filename Output file path.
     * @param grid Grid metadata.
     * @param values Scalar values on the grid.
     */
    static void write(const std::string& filename, const Grid& grid, const std::vector<double>& values);
};

}  // namespace laplace
