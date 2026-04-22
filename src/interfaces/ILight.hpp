// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// ILight + LightSample: interface for all light sources.
//
// LightSample is the result of ILight::sample():
//   direction — unit vector pointing FROM the surface point TOWARD
//               the light (used for the dot-product in Lambert,
//               and as the direction of shadow rays).
//   radiance  — incident spectral radiance at the surface point
//               (HDR, linear). Directional lights are infinitely
//               far so their radiance doesn't fall off with distance.
//   distance  — world-space distance to the light. Shadow rays use
//               `tMax = distance` so they stop at the light source.
//               Directional lights set this to +infinity.
//
// isAmbient / ambientContribution — out-of-band query for the
//   renderer's ambient term. Ambient lights are special: they have
//   no direction and don't cast shadows. The renderer sums all
//   `ambientContribution()` values once per shading call.
//   Non-ambient lights return the default false / black.
//
// Ticket: T0.7.

#ifndef RAYTRACER_INTERFACES_ILIGHT_HPP
#define RAYTRACER_INTERFACES_ILIGHT_HPP

#include <limits>

#include "math/Color.hpp"
#include "math/Vector3.hpp"

namespace rt {

struct LightSample {
    Vector3 direction;                                 // unit, toward light
    Color   radiance;                                  // incident radiance (HDR)
    double  distance = std::numeric_limits<double>::infinity();  // +inf for directional
};

class ILight {
public:
    virtual ~ILight() = default;

    virtual LightSample sample(const Vector3& surfacePoint) const = 0;

    virtual bool  isAmbient()              const { return false; }
    virtual Color ambientContribution()    const { return Color::black(); }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_ILIGHT_HPP
