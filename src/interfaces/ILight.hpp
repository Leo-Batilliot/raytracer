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

    virtual bool  isAmbient()           const { return false; }
    virtual Color ambientContribution() const { return Color::black(); }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_ILIGHT_HPP
