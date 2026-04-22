// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// PrimitiveNode: INode adapter that wraps a single IPrimitive (T1.1).
// Bridges the scene-graph layer (INode) and the geometry layer (IPrimitive).
//
// T1.7: material assignment + translation via Transform.
// T2.3: full Transform (rotation + translation) replaces bare translate_.
//
//   transform_        — forward transform: object-space → world-space.
//   inverseTransform_ — world-space → object-space (cached for hot path).
//
// Ray transform: localRay = inverse.applyTo(worldRay) before intersect.
// Hit transform: rec.point  = forward.applyToPoint(objectPoint).
//               rec.normal = forward.applyToNormal(objectNormal).normalize()
//               Normal rule: n' = (M^-1)^T * n — forward.applyToNormal() does
//               exactly this using its cached inverse.

#ifndef RAYTRACER_CORE_PRIMITIVENODE_HPP
#define RAYTRACER_CORE_PRIMITIVENODE_HPP

#include <memory>

#include "interfaces/INode.hpp"
#include "interfaces/IPrimitive.hpp"
#include "math/Transform.hpp"

namespace rt {

class PrimitiveNode final : public INode {
public:
    PrimitiveNode(std::shared_ptr<IPrimitive> prim,
                  const IMaterial*            mat,
                  Transform                   transform)
        : prim_(std::move(prim))
        , material_(mat)
        , transform_(transform)
        , inverseTransform_(transform.inverse())   // cached — never recomputed
    {
    }

    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        // Transform ray to object space.
        Ray localRay    = ray;
        localRay.origin    = inverseTransform_.applyToPoint(ray.origin);
        localRay.direction = inverseTransform_.applyToDirection(ray.direction);

        if (!prim_->intersect(localRay, rec)) return false;

        // Transform hit back to world space.
        rec.point = transform_.applyToPoint(rec.point);

        // Normal transform: (M^-1)^T · n.
        // forward.applyToNormal() computes exactly this using the cached inverse.
        const Vector3 wn  = transform_.applyToNormal(rec.normal);
        const double  len = wn.length();
        if (len > 1e-10) rec.normal = wn / len;

        // Assign material.
        if (material_) rec.material = material_;

        return true;
    }

    // World-space AABB: transform all 8 corners of the object-space AABB.
    AABB bounds() const override
    {
        const AABB local = prim_->bounds();
        AABB world{};
        for (int i = 0; i < 8; ++i) {
            const Vector3 corner{
                (i & 1) ? local.max.x : local.min.x,
                (i & 2) ? local.max.y : local.min.y,
                (i & 4) ? local.max.z : local.min.z,
            };
            world = world.expanded(transform_.applyToPoint(corner));
        }
        return world;
    }

private:
    std::shared_ptr<IPrimitive> prim_;
    const IMaterial*            material_ = nullptr;
    Transform                   transform_;
    Transform                   inverseTransform_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_PRIMITIVENODE_HPP
