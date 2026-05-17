#ifndef RAYTRACER_INTERFACES_RAY_HPP
#define RAYTRACER_INTERFACES_RAY_HPP

#include <limits>

#include "math/Vector3.hpp"

namespace rt {

// Small positive offset to push a spawned ray's origin away from the surface,
// preventing self-intersection due to floating-point imprecision.
inline constexpr double kRayEpsilon = 1e-4;

struct Ray {
    Vector3 origin{};
    Vector3 direction{};
    double tMin = kRayEpsilon;
    double tMax = std::numeric_limits<double>::infinity();

    constexpr Vector3 at(double t) const noexcept
    {
        return origin + direction * t;
    }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_RAY_HPP
