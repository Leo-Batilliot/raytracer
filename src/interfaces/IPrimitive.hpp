// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// IPrimitive: pure-virtual interface for a geometric primitive.
//
// A primitive is the *lowest* object in the scene graph: it
// knows only about its own geometry (in object space) and UV
// parameterisation. Positioning in world space is handled by
// the INode that wraps it.
//
// Contract:
//   intersect — fills `rec` (t, point, normal, uv, frontFace)
//               when the ray hits the primitive in [ray.tMin, ray.tMax].
//               Returns false and leaves `rec` untouched on miss.
//   bounds    — returns the AABB in *object* space.
//   uvAt      — maps a localPoint (already in object space) to UV.
//               Used when a material needs texture coordinates.
//
// Ticket: T0.7.

#ifndef RAYTRACER_INTERFACES_IPRIMITIVE_HPP
#define RAYTRACER_INTERFACES_IPRIMITIVE_HPP

#include "interfaces/AABB.hpp"
#include "interfaces/HitRecord.hpp"
#include "interfaces/Ray.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace rt {

class IPrimitive {
public:
    virtual ~IPrimitive() = default;

    virtual bool    intersect(const Ray& ray, HitRecord& rec) const = 0;
    virtual AABB    bounds()                                   const = 0;
    virtual Vector2 uvAt(const Vector3& localPoint)            const = 0;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_IPRIMITIVE_HPP
