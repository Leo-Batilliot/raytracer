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
                r.tMax = tmp.t;
                rec    = tmp;
            }
        }
        return hit;
    }

    AABB bounds() const override
    {
        AABB result;
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
