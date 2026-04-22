// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// PrimitiveNode: INode adapter that wraps a single IPrimitive (T1.1).
// Bridges the scene-graph layer (INode) and the geometry layer
// (IPrimitive). Positioned in world space via GroupNode + Transform
// (T2.3+); for now, object space == world space.

#ifndef RAYTRACER_CORE_PRIMITIVENODE_HPP
#define RAYTRACER_CORE_PRIMITIVENODE_HPP

#include <memory>

#include "interfaces/INode.hpp"
#include "interfaces/IPrimitive.hpp"

namespace rt {

class PrimitiveNode final : public INode {
public:
    explicit PrimitiveNode(std::shared_ptr<IPrimitive> prim)
        : prim_(std::move(prim))
    {
    }

    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        return prim_->intersect(ray, rec);
    }

    AABB bounds() const override { return prim_->bounds(); }

private:
    std::shared_ptr<IPrimitive> prim_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_PRIMITIVENODE_HPP
