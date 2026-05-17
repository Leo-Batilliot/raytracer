#ifndef RAYTRACER_INTERFACES_IMATERIAL_HPP
#define RAYTRACER_INTERFACES_IMATERIAL_HPP

#include "interfaces/ShadingContext.hpp"
#include "math/Color.hpp"

namespace rt {

class IMaterial {
public:
    virtual ~IMaterial() = default;

    // Returns the surface albedo for this hit point.
    virtual Color shade(const ShadingContext& ctx) const = 0;

    // Phong coefficients.
    virtual float ka()            const { return 1.0f; }
    virtual float kd()            const { return 1.0f; }
    virtual float shininess()     const { return 0.0f; }
    virtual Color specularColor() const { return {};   }

    // 0 = opaque, 1 = perfect mirror.
    virtual float reflectivity()  const { return 0.0f; }

    // transparency: 0 = opaque, 1 = fully glass-like.
    // ior: index of refraction (1.5 = glass, 2.42 = diamond).
    virtual float transparency()  const { return 0.0f; }
    virtual float ior()           const { return 1.0f; }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_IMATERIAL_HPP
