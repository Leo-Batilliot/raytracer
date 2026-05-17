#ifndef RAYTRACER_MATH_MATRIX4_HPP
#define RAYTRACER_MATH_MATRIX4_HPP

#include <array>
#include <cmath>
#include <cstddef>

#include "math/Vector3.hpp"

namespace rt {

// 4×4 matrix, row-major storage. Default-constructed = identity.
// Vectors are column vectors; w=1 for points, w=0 for directions.
class Matrix4 {
public:
    constexpr Matrix4() noexcept
        : m_{{
              1.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0, 0.0,
              0.0, 0.0, 1.0, 0.0,
              0.0, 0.0, 0.0, 1.0,
          }}
    {
    }

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

    constexpr double operator()(std::size_t row, std::size_t col) const noexcept
    {
        return m_[row * 4 + col];
    }
    constexpr double& operator()(std::size_t row, std::size_t col) noexcept
    {
        return m_[row * 4 + col];
    }

    constexpr bool operator==(const Matrix4&) const = default;

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

    // std::cos/sin are not constexpr in C++20.
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

    // Point (w=1): translation applies.
    constexpr Vector3 applyToPoint(const Vector3& p) const noexcept
    {
        return {
            m_[0] * p.x + m_[1] * p.y + m_[2] * p.z + m_[3],
            m_[4] * p.x + m_[5] * p.y + m_[6] * p.z + m_[7],
            m_[8] * p.x + m_[9] * p.y + m_[10] * p.z + m_[11],
        };
    }

    // Direction (w=0): translation row ignored.
    constexpr Vector3 applyToDirection(const Vector3& d) const noexcept
    {
        return {
            m_[0] * d.x + m_[1] * d.y + m_[2] * d.z,
            m_[4] * d.x + m_[5] * d.y + m_[6] * d.z,
            m_[8] * d.x + m_[9] * d.y + m_[10] * d.z,
        };
    }

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

    // Gauss-Jordan inverse with partial pivoting. Throws rt::MathError if singular.
    Matrix4 inverse() const;

private:
    std::array<double, 16> m_;
};

}  // namespace rt

#endif  // RAYTRACER_MATH_MATRIX4_HPP
