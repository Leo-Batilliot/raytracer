#include "output/PPMWriter.hpp"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>

#include "core/Exceptions.hpp"

namespace rt {

namespace {

// IEC 61966-2-1 linear → sRGB, clamped to [0, 255].
uint8_t linearToSRGB8(float linear) noexcept
{
    if (linear <= 0.0f) return 0;
    if (linear >= 1.0f) return 255;

    float srgb = (linear <= 0.0031308f)
                     ? linear * 12.92f
                     : 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;

    return static_cast<uint8_t>(srgb * 255.0f + 0.5f);
}

}  // namespace

void PPMWriter::write(std::ostream& out, int width, int height,
                      const std::vector<Color>& pixels)
{
    out << "P6\n" << width << ' ' << height << "\n255\n";

    for (const auto& c : pixels) {
        const uint8_t rgb[3] = {
            linearToSRGB8(c.r),
            linearToSRGB8(c.g),
            linearToSRGB8(c.b),
        };
        out.write(reinterpret_cast<const char*>(rgb), 3);
    }
}

void PPMWriter::writeFile(const std::filesystem::path& path, int width,
                          int height, const std::vector<Color>& pixels)
{
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        throw IOError("cannot open output file: " + path.string());
    }
    write(ofs, width, height, pixels);
    if (!ofs) {
        throw IOError("write error on file: " + path.string());
    }
}

}  // namespace rt
