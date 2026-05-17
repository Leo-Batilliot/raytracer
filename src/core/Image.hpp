#ifndef RAYTRACER_CORE_IMAGE_HPP
#define RAYTRACER_CORE_IMAGE_HPP

#include <vector>

#include "math/Color.hpp"

namespace rt {

// Row-major pixel buffer in linear float space. PPMWriter converts to sRGB at write time.
class Image {
public:
    Image(int width, int height)
        : width_(width), height_(height), pixels_(width * height)
    {
    }

    Color& at(int x, int y) noexcept { return pixels_[y * width_ + x]; }
    const Color& at(int x, int y) const noexcept
    {
        return pixels_[y * width_ + x];
    }

    int width()  const noexcept { return width_; }
    int height() const noexcept { return height_; }
    const std::vector<Color>& pixels() const noexcept { return pixels_; }

private:
    int width_;
    int height_;
    std::vector<Color> pixels_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_IMAGE_HPP
