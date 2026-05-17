#include "math/Transform.hpp"

#include "core/Exceptions.hpp"
#include "math/Matrix4.hpp"

namespace rt {

Transform::Transform(const Matrix4& matrix)
    : matrix_(matrix), inverse_(matrix.inverse())
{
}

Transform Transform::translation(double dx, double dy, double dz) noexcept
{
    // Inverse of a translation is the negated translation.
    return Transform{
        Matrix4::translation(dx, dy, dz),
        Matrix4::translation(-dx, -dy, -dz),
    };
}

Transform Transform::scaling(double sx, double sy, double sz)
{
    if (sx == 0.0 || sy == 0.0 || sz == 0.0) {
        throw MathError(
            "Transform::scaling(): zero component (non-invertible)");
    }
    // Inverse of a scale is the componentwise reciprocal.
    return Transform{
        Matrix4::scaling(sx, sy, sz),
        Matrix4::scaling(1.0 / sx, 1.0 / sy, 1.0 / sz),
    };
}

Transform Transform::rotationX(double radians) noexcept
{
    // Rotations are orthogonal: inverse == rotation by -radians.
    return Transform{
        Matrix4::rotationX(radians),
        Matrix4::rotationX(-radians),
    };
}

Transform Transform::rotationY(double radians) noexcept
{
    return Transform{
        Matrix4::rotationY(radians),
        Matrix4::rotationY(-radians),
    };
}

Transform Transform::rotationZ(double radians) noexcept
{
    return Transform{
        Matrix4::rotationZ(radians),
        Matrix4::rotationZ(-radians),
    };
}

Vector3 Transform::applyToNormal(const Vector3& n) const noexcept
{
    // n' = (M^-1)^T * n — multiply by the inverse transposed (upper-left 3×3 only).
    const Matrix4& inv = inverse_;
    return {
        inv(0, 0) * n.x + inv(1, 0) * n.y + inv(2, 0) * n.z,
        inv(0, 1) * n.x + inv(1, 1) * n.y + inv(2, 1) * n.z,
        inv(0, 2) * n.x + inv(1, 2) * n.y + inv(2, 2) * n.z,
    };
}

}  // namespace rt
