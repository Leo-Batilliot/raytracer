#ifndef RAYTRACER_MATH_TRANSFORM_HPP
#define RAYTRACER_MATH_TRANSFORM_HPP

#include "math/Matrix4.hpp"
#include "math/Vector3.hpp"

namespace rt {

// Affine transform that caches its inverse for efficient ray-to-object-space
// conversion in the render loop.
class Transform {
public:
    Transform() noexcept = default;

    // Generic constructor: computes inverse via Matrix4::inverse().
    // Throws rt::MathError if `matrix` is singular.
    explicit Transform(const Matrix4& matrix);

    // Direct constructor when the caller has the inverse analytically.
    Transform(const Matrix4& matrix, const Matrix4& inverse) noexcept
        : matrix_(matrix), inverse_(inverse)
    {
    }

    static Transform identity() noexcept { return Transform{}; }
    static Transform translation(double dx, double dy, double dz) noexcept;

    // Throws rt::MathError if any scale component is 0.
    static Transform scaling(double sx, double sy, double sz);

    static Transform rotationX(double radians) noexcept;
    static Transform rotationY(double radians) noexcept;
    static Transform rotationZ(double radians) noexcept;

    Vector3 applyToPoint(const Vector3& p) const noexcept
    {
        return matrix_.applyToPoint(p);
    }
    Vector3 applyToDirection(const Vector3& d) const noexcept
    {
        return matrix_.applyToDirection(d);
    }

    // Normal transform: (M^-1)^T * n. Does not re-normalise.
    Vector3 applyToNormal(const Vector3& n) const noexcept;

    // (this * other): apply `other` first, then `this`.
    Transform operator*(const Transform& other) const noexcept
    {
        return Transform{matrix_ * other.matrix_,
                         other.inverse_ * inverse_};
    }

    Transform inverse() const noexcept { return Transform{inverse_, matrix_}; }

    const Matrix4& matrix()        const noexcept { return matrix_; }
    const Matrix4& inverseMatrix() const noexcept { return inverse_; }

private:
    Matrix4 matrix_{};
    Matrix4 inverse_{};
};

}  // namespace rt

#endif  // RAYTRACER_MATH_TRANSFORM_HPP
