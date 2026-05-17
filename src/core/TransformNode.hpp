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
        Ray local       = ray;
        local.origin    = inverseTransform_.applyToPoint(ray.origin);
        local.direction = inverseTransform_.applyToDirection(ray.direction);

        if (!child_->intersect(local, rec)) return false;

        rec.point          = transform_.applyToPoint(rec.point);
        // Normal transform: (M^-1)^T · n.
        const Vector3 wn   = transform_.applyToNormal(rec.normal);
        const double  len  = wn.length();
        if (len > 1e-10) rec.normal = wn / len;

        return true;
    }

    AABB bounds() const override
    {
        const AABB local = child_->bounds();
        if (!local.isFinite()) return local;

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
