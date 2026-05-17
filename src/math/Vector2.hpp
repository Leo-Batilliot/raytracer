#ifndef RAYTRACER_MATH_VECTOR2_HPP
#define RAYTRACER_MATH_VECTOR2_HPP

#include <cmath>

#include "core/Exceptions.hpp"

namespace rt {

struct Vector2 {
    double x = 0.0;
    double y = 0.0;

    constexpr Vector2() = default;
    constexpr Vector2(double x_, double y_) : x(x_), y(y_) {}

    constexpr Vector2 operator+(const Vector2& o) const noexcept
    {
        return {x + o.x, y + o.y};
    }
    constexpr Vector2 operator-(const Vector2& o) const noexcept
    {
        return {x - o.x, y - o.y};
    }
    constexpr Vector2 operator-() const noexcept { return {-x, -y}; }
    constexpr Vector2 operator*(double s) const noexcept
    {
        return {x * s, y * s};
    }
    constexpr Vector2 operator/(double s) const noexcept
    {
        return {x / s, y / s};
    }

    constexpr bool operator==(const Vector2&) const = default;

    constexpr double dot(const Vector2& o) const noexcept
    {
        return x * o.x + y * o.y;
    }
    constexpr double lengthSquared() const noexcept { return dot(*this); }
    double length() const noexcept { return std::sqrt(lengthSquared()); }

    // Throws rt::MathError if zero-length.
    Vector2 normalized() const
    {
        const double len = length();
        if (len == 0.0) {
            throw MathError("Vector2::normalized(): zero-length vector");
        }
        return *this / len;
    }
};

constexpr Vector2 operator*(double s, const Vector2& v) noexcept
{
    return v * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_VECTOR2_HPP
