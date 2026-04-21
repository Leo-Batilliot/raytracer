// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Matrix4: 4x4 matrix of doubles, row-major contiguous storage.
//
// Convention:
//   - Row-major: element (row, col) is at flat index row*4 + col.
//   - Default-constructed Matrix4 is the IDENTITY matrix. This is the
//     neutral element for multiplication and what a freshly created
//     transform should represent; it matches PBRT / GLM conventions.
//   - Vectors are treated as column vectors; multiplication is M * v.
//   - Points are in homogeneous coordinates with w = 1 (affine row).
//   - Directions use w = 0 (no translation applied).
//
// Ticket: T0.5.
// Hot paths (intersect / shade / tile loop) must NOT throw. The only
// throwing method is `inverse()`, which is called at Transform
// construction time (boundaries), never inside the render loop.

#ifndef RAYTRACER_MATH_MATRIX4_HPP
#define RAYTRACER_MATH_MATRIX4_HPP

#include <array>
#include <cmath>
#include <cstddef>

#include "math/Vector3.hpp"

namespace rt {

class Matrix4 {
public:
    // Default-constructed Matrix4 is the identity matrix.
    constexpr Matrix4() noexcept
        : m_{{
              1.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0, 0.0,
              0.0, 0.0, 1.0, 0.0,
              0.0, 0.0, 0.0, 1.0,
          }}
    {
    }

    // Explicit 16-value constructor, row-major ordering.
    constexpr Matrix4(double m00, double m01, double m02, double m03,
                      double m10, double m11, double m12, double m13,
                      double m20, double m21, double m22, double m23,
                      double m30, double m31, double m32, double m33) noexcept
        : m_{{
              m00, m01, m02, m03,
              m10, m11, m12, m13,
              m20, m21, m22, m23,
              m30, m31, m32, m33,
          }}
    {
    }

    // Element access: M(row, col).
    constexpr double operator()(std::size_t row, std::size_t col) const noexcept
    {
        return m_[row * 4 + col];
    }
    constexpr double& operator()(std::size_t row, std::size_t col) noexcept
    {
        return m_[row * 4 + col];
    }

    constexpr bool operator==(const Matrix4&) const = default;

    // -----------------------------------------------------------------
    // Named constructors
    // -----------------------------------------------------------------
    static constexpr Matrix4 identity() noexcept { return Matrix4{}; }

    static constexpr Matrix4 translation(double dx, double dy, double dz) noexcept
    {
        return {
            1.0, 0.0, 0.0, dx,
            0.0, 1.0, 0.0, dy,
            0.0, 0.0, 1.0, dz,
            0.0, 0.0, 0.0, 1.0,
        };
    }

    static constexpr Matrix4 scaling(double sx, double sy, double sz) noexcept
    {
        return {
            sx,  0.0, 0.0, 0.0,
            0.0, sy,  0.0, 0.0,
            0.0, 0.0, sz,  0.0,
            0.0, 0.0, 0.0, 1.0,
        };
    }

    // Rotations are not constexpr: std::cos / std::sin are not
    // constexpr in C++20 (they become constexpr in C++23).
    static Matrix4 rotationX(double radians) noexcept
    {
        const double c = std::cos(radians);
        const double s = std::sin(radians);
        return {
            1.0, 0.0, 0.0, 0.0,
            0.0, c,   -s,  0.0,
            0.0, s,   c,   0.0,
            0.0, 0.0, 0.0, 1.0,
        };
    }
    static Matrix4 rotationY(double radians) noexcept
    {
        const double c = std::cos(radians);
        const double s = std::sin(radians);
        return {
            c,   0.0, s,   0.0,
            0.0, 1.0, 0.0, 0.0,
            -s,  0.0, c,   0.0,
            0.0, 0.0, 0.0, 1.0,
        };
    }
    static Matrix4 rotationZ(double radians) noexcept
    {
        const double c = std::cos(radians);
        const double s = std::sin(radians);
        return {
            c,   -s,  0.0, 0.0,
            s,   c,   0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
        };
    }

    // -----------------------------------------------------------------
    // Application to Vector3 (homogeneous)
    // -----------------------------------------------------------------
    // Point: w = 1, translation row applies. No perspective divide (w
    // stays 1 for rigid + scale affine transforms; if a projection is
    // added later, this helper should be revisited).
    constexpr Vector3 applyToPoint(const Vector3& p) const noexcept
    {
        return {
            m_[0] * p.x + m_[1] * p.y + m_[2] * p.z + m_[3],
            m_[4] * p.x + m_[5] * p.y + m_[6] * p.z + m_[7],
            m_[8] * p.x + m_[9] * p.y + m_[10] * p.z + m_[11],
        };
    }

    // Direction: w = 0, translation row ignored.
    constexpr Vector3 applyToDirection(const Vector3& d) const noexcept
    {
        return {
            m_[0] * d.x + m_[1] * d.y + m_[2] * d.z,
            m_[4] * d.x + m_[5] * d.y + m_[6] * d.z,
            m_[8] * d.x + m_[9] * d.y + m_[10] * d.z,
        };
    }

    // -----------------------------------------------------------------
    // Core operations
    // -----------------------------------------------------------------
    constexpr Matrix4 operator*(const Matrix4& rhs) const noexcept
    {
        Matrix4 r;
        for (std::size_t i = 0; i < 4; ++i) {
            for (std::size_t j = 0; j < 4; ++j) {
                double sum = 0.0;
                for (std::size_t k = 0; k < 4; ++k) {
                    sum += (*this)(i, k) * rhs(k, j);
                }
                r(i, j) = sum;
            }
        }
        return r;
    }

    constexpr Matrix4 transposed() const noexcept
    {
        Matrix4 r;
        for (std::size_t i = 0; i < 4; ++i) {
            for (std::size_t j = 0; j < 4; ++j) {
                r(i, j) = (*this)(j, i);
            }
        }
        return r;
    }

    // Gauss-Jordan inverse with partial pivoting. Throws rt::MathError
    // if the matrix is (numerically) singular. Definition lives in
    // Matrix4.cpp to keep the header light.
    Matrix4 inverse() const;

private:
    std::array<double, 16> m_;
};

}  // namespace rt

#endif  // RAYTRACER_MATH_MATRIX4_HPP
