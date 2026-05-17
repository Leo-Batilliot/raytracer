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

    // Throws rt::MathError if zero-length.
    Vector3 normalized() const
    {
        const double len = length();
        if (len == 0.0) {
            throw MathError("Vector3::normalized(): zero-length vector");
        }
        return *this / len;
    }
};

constexpr Vector3 operator*(double s, const Vector3& v) noexcept
{
    return v * s;
}

}  // namespace rt

#endif  // RAYTRACER_MATH_VECTOR3_HPP
