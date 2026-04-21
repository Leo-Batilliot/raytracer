// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Color: RGB triplet stored in LINEAR space, float precision.
//
// Storage convention (locked in PLAN.md):
//   - All intermediate color math (light accumulation, attenuation,
//     reflection) happens in linear space. Channels are floats and
//     CAN go above 1.0 — HDR-style accumulation is legal.
//   - sRGB gamma conversion and clamping to [0,1] happen ONLY at the
//     final write step (PPMWriter, ticket T0.12). Do not convert here.
//
// Operators:
//   +, -                  : additive combination (light accumulation).
//   *float / /float       : intensity scaling.
//   *Color (componentwise): surface albedo × incoming light.
//
// Ticket: T0.4.

#ifndef RAYTRACER_MATH_COLOR_HPP
#define RAYTRACER_MATH_COLOR_HPP

namespace rt {

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
    // Component-wise product: surface albedo × incoming light radiance.
    constexpr Color operator*(const Color& o) const noexcept
    {
        return {r * o.r, g * o.g, b * o.b};
    }

    constexpr bool operator==(const Color&) const = default;
};

// Symmetric scalar multiplication: 0.5f * c.
constexpr Color operator*(float s, const Color& c) noexcept
{
    return c * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_COLOR_HPP
