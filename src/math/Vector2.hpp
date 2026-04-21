// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Vector2: 2D vector stored in double precision.
//
// Primary use: UV coordinates (textures), screen-space positions
// (anti-aliasing offsets, image coordinates). The 3D math workhorse
// is Vector3; Vector2 only carries the minimum needed for UVs.
//
// Ticket: T0.4.
// Immutability: every operator returns a NEW Vector2 (see rules/common
// coding-style.md). No in-place mutation is exposed.

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

    // Throws rt::MathError if this vector has zero length.
    Vector2 normalized() const
    {
        const double len = length();
        if (len == 0.0) {
            throw MathError("Vector2::normalized(): zero-length vector");
        }
        return *this / len;
    }
};

// Symmetric scalar multiplication: 2.0 * v.
constexpr Vector2 operator*(double s, const Vector2& v) noexcept
{
    return v * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_VECTOR2_HPP
