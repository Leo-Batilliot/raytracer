// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Vector3: 3D vector stored in double precision.
//
// The main math type of the raytracer. Used for points, directions
// and normals. Double precision was chosen over float to avoid
// accumulating error in long chains of transforms / intersections;
// the Color type uses float instead (see Color.hpp).
//
// Ticket: T0.4.
// Immutability: every operator returns a NEW Vector3. No in-place
// mutation is exposed (see rules/common coding-style.md).
//
// Hot paths (intersect/shade/tile loop) must NOT throw — that is why
// the only throwing method here, `normalized()`, is expected to be
// called at construction time (e.g. parsing camera.up), not inside the
// render loop. When a renderer needs a possibly-zero vector, it should
// check `lengthSquared() > 0` and branch explicitly.

#ifndef RAYTRACER_MATH_VECTOR3_HPP
#define RAYTRACER_MATH_VECTOR3_HPP

#include <cmath>

#include "core/Exceptions.hpp"

namespace rt {

struct Vector3 {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    constexpr Vector3() = default;
    constexpr Vector3(double x_, double y_, double z_)
        : x(x_), y(y_), z(z_)
    {
    }

    constexpr Vector3 operator+(const Vector3& o) const noexcept
    {
        return {x + o.x, y + o.y, z + o.z};
    }
    constexpr Vector3 operator-(const Vector3& o) const noexcept
    {
        return {x - o.x, y - o.y, z - o.z};
    }
    constexpr Vector3 operator-() const noexcept { return {-x, -y, -z}; }
    constexpr Vector3 operator*(double s) const noexcept
    {
        return {x * s, y * s, z * s};
    }
    constexpr Vector3 operator/(double s) const noexcept
    {
        return {x / s, y / s, z / s};
    }

    constexpr bool operator==(const Vector3&) const = default;

    constexpr double dot(const Vector3& o) const noexcept
    {
        return x * o.x + y * o.y + z * o.z;
    }
    constexpr Vector3 cross(const Vector3& o) const noexcept
    {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x,
        };
    }
    constexpr double lengthSquared() const noexcept { return dot(*this); }
    double length() const noexcept { return std::sqrt(lengthSquared()); }

    // Throws rt::MathError if this vector has zero length.
    Vector3 normalized() const
    {
        const double len = length();
        if (len == 0.0) {
            throw MathError("Vector3::normalized(): zero-length vector");
        }
        return *this / len;
    }
};

// Symmetric scalar multiplication: 2.0 * v.
constexpr Vector3 operator*(double s, const Vector3& v) noexcept
{
    return v * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_VECTOR3_HPP
