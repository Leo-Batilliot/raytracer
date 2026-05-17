#ifndef RAYTRACER_OUTPUT_PPMWRITER_HPP
#define RAYTRACER_OUTPUT_PPMWRITER_HPP

#include <filesystem>
#include <iosfwd>
#include <vector>

#include "math/Color.hpp"

namespace rt {

// PPM P6 binary writer with linear → sRGB conversion.
class PPMWriter {
public:
    static void write(std::ostream& out, int width, int height,
                      const std::vector<Color>& pixels);

    // Throws rt::IOError on failure.
    static void writeFile(const std::filesystem::path& path, int width,
                          int height, const std::vector<Color>& pixels);
};

}  // namespace rt

#endif  // RAYTRACER_OUTPUT_PPMWRITER_HPP
