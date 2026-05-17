// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// TransformNode: wraps any INode with a Transform for scene-graph grouping
// (T4.5 — bonus "scene graph" 2 pts).
//
// Enables hierarchical transforms: a group's parent transform is applied to
// all its children by transforming the ray to group-local space before
// delegating to the wrapped child node.
//
// Ray transform : localRay  = inverse.applyTo(worldRay)
// Hit transform : rec.point  = forward.applyToPoint(localPoint)
//                rec.normal = (forward.applyToNormal(localNormal)).normalize()
//                             — i.e. (M^-1)^T * n, same rule as PrimitiveNode.
// AABB          : transform all 8 corners of the child's local AABB.
//                 If the child's bounds are infinite (plane etc.) the infinite
//                 AABB is passed through unchanged — the BVH partition in
//                 SceneBuilder::build() will route it to the linear GroupNode.
//
// Ticket: T4.5.

#ifndef RAYTRACER_CORE_TRANSFORMNODE_HPP
#define RAYTRACER_CORE_TRANSFORMNODE_HPP

#include <memory>

#include "interfaces/AABB.hpp"
#include "interfaces/INode.hpp"
#include "interfaces/Ray.hpp"
#include "math/Transform.hpp"

namespace rt {

class TransformNode final : public INode {
public:
    TransformNode(std::shared_ptr<INode> child, Transform transform)
        : child_(std::move(child))
        , transform_(transform)
        , inverseTransform_(transform.inverse())
    {
    }

    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        // Transform ray to group-local (object) space.
        Ray local       = ray;
        local.origin    = inverseTransform_.applyToPoint(ray.origin);
        local.direction = inverseTransform_.applyToDirection(ray.direction);

        if (!child_->intersect(local, rec)) return false;

        // Transform hit back to world space.
        rec.point          = transform_.applyToPoint(rec.point);
        const Vector3 wn   = transform_.applyToNormal(rec.normal);
        const double  len  = wn.length();
        if (len > 1e-10) rec.normal = wn / len;

        return true;
    }

    AABB bounds() const override
    {
        const AABB local = child_->bounds();
        if (!local.isFinite()) return local;  // infinite child — pass through

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
    std::shared_ptr<INode> child_;
    Transform              transform_;
    Transform              inverseTransform_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_TRANSFORMNODE_HPP
