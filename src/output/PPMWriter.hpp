// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// PPM P6 binary writer with linear → sRGB conversion (T0.12).
// All intermediate rendering is done in linear float space; this is the
// only place where gamma encoding and byte-clamping occur.

#ifndef RAYTRACER_OUTPUT_PPMWRITER_HPP
#define RAYTRACER_OUTPUT_PPMWRITER_HPP

#include <filesystem>
#include <iosfwd>
#include <vector>

#include "math/Color.hpp"

namespace rt {

class PPMWriter {
public:
    // Write a PPM P6 image to an already-open stream.
    // pixels must have exactly width * height elements, row-major.
    static void write(std::ostream& out, int width, int height,
                      const std::vector<Color>& pixels);

    // Convenience overload: open path for writing, then call write().
    // Throws rt::IOError on failure to open or write.
    static void writeFile(const std::filesystem::path& path, int width,
                          int height, const std::vector<Color>& pixels);
};

}  // namespace rt

#endif  // RAYTRACER_OUTPUT_PPMWRITER_HPP
