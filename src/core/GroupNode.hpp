// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// GroupNode: INode that holds an ordered list of child INodes (T1.1).
// Implements the Composite pattern; each child can itself be a group,
// a PrimitiveNode, a TransformNode (T2.3), etc.
//
// intersect() iterates all children, shrinking tMax on each successful
// hit so that only closer hits are reported — correct closest-hit
// semantics without a separate closest-t variable.

#ifndef RAYTRACER_CORE_GROUPNODE_HPP
#define RAYTRACER_CORE_GROUPNODE_HPP

#include <memory>
#include <vector>

#include "interfaces/AABB.hpp"
#include "interfaces/INode.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class GroupNode final : public INode {
public:
    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        bool hit = false;
        Ray  r   = ray;

        for (const auto& child : children_) {
            HitRecord tmp;
            if (child->intersect(r, tmp)) {
                hit    = true;
                r.tMax = tmp.t;  // shrink: only accept closer hits
                rec    = tmp;
            }
        }
        return hit;
    }

    AABB bounds() const override
    {
        AABB result;  // default = empty sentinel (inf/-inf)
        for (const auto& child : children_)
            result = result.merged(child->bounds());
        return result;
    }

    void addChild(std::shared_ptr<INode> child) override
    {
        children_.push_back(std::move(child));
    }

private:
    std::vector<std::shared_ptr<INode>> children_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_GROUPNODE_HPP
