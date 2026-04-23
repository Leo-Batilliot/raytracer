// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// IMaterial: Strategy interface for surface shading.
//
// `shade()` receives the full ShadingContext (scene, hit record,
// incoming ray, recursion depth) and returns the outgoing colour
// toward the camera for this surface point.
//
// Implementations decide how to:
//   - query lights from ctx.scene (for direct illumination),
//   - spawn secondary rays (reflections, refractions) via
//     IPrimitive::intersect on ctx.scene,
//   - stop recursion when ctx.depth reaches a configured maximum.
//
// `shade()` is NOT called from hot loops directly — it's called
// once per closest hit per pixel. Still, keep it allocation-free
// (use the stack only; no heap allocations in the shade path).
//
// Ticket: T0.7.

#ifndef RAYTRACER_INTERFACES_IMATERIAL_HPP
#define RAYTRACER_INTERFACES_IMATERIAL_HPP

#include "interfaces/ShadingContext.hpp"
#include "math/Color.hpp"

namespace rt {

class IMaterial {
public:
    virtual ~IMaterial() = default;

    // Returns the surface albedo (base colour) for this hit point.
    // The renderer applies ambient, diffuse and specular on top using
    // ka(), kd(), specularColor() and shininess().
    virtual Color shade(const ShadingContext& ctx) const = 0;

    // Phong coefficients — all default to the flat/Lambert model.
    virtual float ka()            const { return 1.0f; }  // ambient scale
    virtual float kd()            const { return 1.0f; }  // diffuse scale
    virtual float shininess()     const { return 0.0f; }  // 0 = no specular
    virtual Color specularColor() const { return {};   }  // ks × highlight colour

    // Reflection (T4.2). 0 = opaque, 1 = perfect mirror.
    virtual float reflectivity()  const { return 0.0f; }

    // Refraction / transparency (T4.3).
    // transparency: 0 = opaque, 1 = fully glass-like.
    // ior: index of refraction (1.0 = air/no-bend, 1.5 = glass, 2.42 = diamond).
    virtual float transparency()  const { return 0.0f; }
    virtual float ior()           const { return 1.0f; }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_IMATERIAL_HPP
