// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// HitRecord: result of a ray-primitive intersection.
//
// POD-style aggregate filled in by `IPrimitive::intersect(ray, rec)`
// when a ray hits a surface. The renderer inspects it to decide
// between hits (closest t wins) and to drive shading.
//
// Lifetime / ownership:
//   - `material` and `node` are NON-OWNING raw pointers into the
//     Scene. The Scene (and the materials registry it holds) own
//     their contents via shared_ptr; HitRecord just borrows for the
//     duration of a shade call. Never delete through these pointers.
//   - HitRecord is meant to be copied around freely — keep it
//     trivial, no smart pointers.
//
// Forward declarations: `IMaterial` and `INode` land in T0.7. Only
// pointers are needed here, so forward-declaring keeps this header
// self-contained.
//
// Ticket: T0.6.

#ifndef RAYTRACER_INTERFACES_HITRECORD_HPP
#define RAYTRACER_INTERFACES_HITRECORD_HPP

#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace rt {

class IMaterial;  // see T0.7
class INode;      // see T0.7

struct HitRecord {
    double t = 0.0;              // ray parameter at hit point
    Vector3 point{};             // world-space hit position
    Vector3 normal{};            // world-space surface normal (unit)
    Vector2 uv{};                // surface parameterisation (texturing)
    const IMaterial* material = nullptr;
    const INode* node = nullptr;
    // True if the ray hit the outside of the surface. Set by the
    // primitive during intersect() by comparing dot(normal, ray.dir).
    bool frontFace = true;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_HITRECORD_HPP
