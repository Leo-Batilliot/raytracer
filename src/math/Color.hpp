#ifndef RAYTRACER_MATH_COLOR_HPP
#define RAYTRACER_MATH_COLOR_HPP

namespace rt {

// RGB triplet in linear float space. sRGB conversion happens only at write time.
struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    constexpr Color() = default;
    constexpr Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}

    static constexpr Color black() noexcept { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Color white() noexcept { return {1.0f, 1.0f, 1.0f}; }

    constexpr Color operator+(const Color& o) const noexcept
    {
        return {r + o.r, g + o.g, b + o.b};
    }
    constexpr Color operator-(const Color& o) const noexcept
    {
        return {r - o.r, g - o.g, b - o.b};
    }
    constexpr Color operator*(float s) const noexcept
    {
        return {r * s, g * s, b * s};
    }
    constexpr Color operator/(float s) const noexcept
    {
        return {r / s, g / s, b / s};
    }
    // Component-wise product: surface albedo × incoming radiance.
    constexpr Color operator*(const Color& o) const noexcept
    {
        return {r * o.r, g * o.g, b * o.b};
    }

    constexpr bool operator==(const Color&) const = default;
};

constexpr Color operator*(float s, const Color& c) noexcept
{
    return c * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_COLOR_HPP
