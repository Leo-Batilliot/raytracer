// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Ray: origin + direction + valid parametric interval [tMin, tMax].
//
// Direction is EXPECTED to be normalised (unit length). The type does
// not enforce it — enforcement happens at ray construction sites
// (Camera::generateRay in T0.13, shadow-ray / reflection / refraction
// builders in Phase 1+).
//
// tMin defaults to `kRayEpsilon` (small positive) to avoid self-
// intersection when a reflected / refracted ray leaves the surface
// it just hit. tMax defaults to +infinity.
//
// Ticket: T0.6.

#ifndef RAYTRACER_INTERFACES_RAY_HPP
#define RAYTRACER_INTERFACES_RAY_HPP

#include <limits>

#include "math/Vector3.hpp"

namespace rt {

// Shared offset used to push a ray's origin away from the surface it
// spawned from, preventing self-intersection due to floating-point
// precision. Used as the default tMin.
inline constexpr double kRayEpsilon = 1e-4;

struct Ray {
    Vector3 origin{};
    Vector3 direction{};
    double tMin = kRayEpsilon;
    double tMax = std::numeric_limits<double>::infinity();

    // Parametric position along the ray: origin + t * direction.
    constexpr Vector3 at(double t) const noexcept
    {
        return origin + direction * t;
    }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_RAY_HPP
