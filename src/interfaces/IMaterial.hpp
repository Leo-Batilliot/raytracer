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

    virtual Color shade(const ShadingContext& ctx) const = 0;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_IMATERIAL_HPP
