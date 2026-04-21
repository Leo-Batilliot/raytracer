// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Transform: a rigid/affine 4x4 transform that caches its inverse.
//
// Why cache the inverse?
// Every primary and secondary ray has to be transformed from world
// space to the object space of each candidate primitive before
// intersection. That's a matrix * ray operation with the INVERSE of
// the primitive's Transform. Computing the inverse on demand (Gauss-
// Jordan, dozens of FLOPs) in the render loop would be wasteful;
// building it once at Transform construction time and storing it is
// essentially free.
//
// Analytical inverses:
//   - Named constructors (translation / scaling / rotationX|Y|Z)
//     build both the matrix AND its inverse directly from the input
//     parameters. This is faster AND numerically cleaner than running
//     Matrix4::inverse() on the resulting matrix.
//   - The generic Transform(const Matrix4&) constructor falls back to
//     the numerical Matrix4::inverse() — used only when the caller
//     constructs a matrix by hand.
//
// Normal transform:
//   Surfaces normals do NOT transform like directions under a
//   non-uniform scale. The correct rule is n' = (M^-1)^T * n. That's
//   why Transform caches the inverse: applyToNormal() is a straight
//   lookup.
//
// Ticket: T0.5.

#ifndef RAYTRACER_MATH_TRANSFORM_HPP
#define RAYTRACER_MATH_TRANSFORM_HPP

#include "math/Matrix4.hpp"
#include "math/Vector3.hpp"

namespace rt {

class Transform {
public:
    // Identity transform.
    Transform() noexcept = default;

    // Generic constructor: computes the inverse via Matrix4::inverse().
    // Throws rt::MathError if `matrix` is singular.
    explicit Transform(const Matrix4& matrix);

    // Direct constructor when caller has an analytical inverse on hand.
    // No invariant check: caller guarantees `matrix * inverse == I`.
    Transform(const Matrix4& matrix, const Matrix4& inverse) noexcept
        : matrix_(matrix), inverse_(inverse)
    {
    }

    // -----------------------------------------------------------------
    // Named constructors — build the inverse analytically (exact).
    // -----------------------------------------------------------------
    static Transform identity() noexcept { return Transform{}; }
    static Transform translation(double dx, double dy, double dz) noexcept;

    // Throws rt::MathError if any scale component is 0 (singular).
    static Transform scaling(double sx, double sy, double sz);

    static Transform rotationX(double radians) noexcept;
    static Transform rotationY(double radians) noexcept;
    static Transform rotationZ(double radians) noexcept;

    // -----------------------------------------------------------------
    // Application
    // -----------------------------------------------------------------
    Vector3 applyToPoint(const Vector3& p) const noexcept
    {
        return matrix_.applyToPoint(p);
    }
    Vector3 applyToDirection(const Vector3& d) const noexcept
    {
        return matrix_.applyToDirection(d);
    }

    // Normal transform: (M^-1)^T * n. Does NOT re-normalise — callers
    // must do it themselves if they need a unit normal after a
    // non-uniform scale.
    Vector3 applyToNormal(const Vector3& n) const noexcept;

    // -----------------------------------------------------------------
    // Composition & inverse
    // -----------------------------------------------------------------
    // (this * other) means: apply `other` first, then `this`.
    // Equivalent matrix:    matrix_ * other.matrix_
    // Equivalent inverse:   other.inverse_ * inverse_
    Transform operator*(const Transform& other) const noexcept
    {
        return Transform{matrix_ * other.matrix_,
                         other.inverse_ * inverse_};
    }

    // Cheap inverse: just swap the cached matrices.
    Transform inverse() const noexcept { return Transform{inverse_, matrix_}; }

    // -----------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------
    const Matrix4& matrix() const noexcept { return matrix_; }
    const Matrix4& inverseMatrix() const noexcept { return inverse_; }

private:
    Matrix4 matrix_{};   // identity by default
    Matrix4 inverse_{};  // identity by default
};

}  // namespace rt

#endif  // RAYTRACER_MATH_TRANSFORM_HPP
