#ifndef RAYTRACER_CORE_BVHNODE_HPP
#define RAYTRACER_CORE_BVHNODE_HPP

#include <algorithm>
#include <memory>
#include <vector>

#include "interfaces/AABB.hpp"
#include "interfaces/INode.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class BVHNode final : public INode {
public:
    // Returns nullptr if empty, the single node if size == 1, or a tree otherwise.
    static std::shared_ptr<INode>
    build(std::vector<std::shared_ptr<INode>> nodes);

    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        if (!bounds_.intersects(ray)) return false;

        bool hit = false;

        HitRecord tmp;
        if (left_ && left_->intersect(ray, tmp)) {
            hit = true;
            rec  = tmp;
        }

        // Shrink tMax to the left hit so the right child only wins if closer.
        Ray trimmed = ray;
        if (hit) trimmed.tMax = rec.t;
        if (right_ && right_->intersect(trimmed, tmp)) {
            hit = true;
            rec  = tmp;
        }

        return hit;
    }

    AABB bounds() const override { return bounds_; }

private:
    BVHNode(std::shared_ptr<INode> left,
            std::shared_ptr<INode> right,
            AABB                   bounds)
        : left_(std::move(left))
        , right_(std::move(right))
        , bounds_(bounds)
    {
    }

    std::shared_ptr<INode> left_;
    std::shared_ptr<INode> right_;
    AABB                   bounds_;
};

inline std::shared_ptr<INode>
BVHNode::build(std::vector<std::shared_ptr<INode>> nodes)
{
    if (nodes.empty()) return nullptr;
    if (nodes.size() == 1) return nodes[0];

    // Split by centroid median on the longest axis.
    AABB centroidBounds;
    for (const auto& n : nodes) {
        const AABB b = n->bounds();
        centroidBounds = centroidBounds.expanded({
            (b.min.x + b.max.x) * 0.5,
            (b.min.y + b.max.y) * 0.5,
            (b.min.z + b.max.z) * 0.5,
        });
    }

    const double extX = centroidBounds.max.x - centroidBounds.min.x;
    const double extY = centroidBounds.max.y - centroidBounds.min.y;
    const double extZ = centroidBounds.max.z - centroidBounds.min.z;

    int axis = 0;
    if (extY > extX) axis = 1;
    if (extZ > (axis == 0 ? extX : extY)) axis = 2;

    std::sort(nodes.begin(), nodes.end(),
        [axis](const std::shared_ptr<INode>& a,
               const std::shared_ptr<INode>& b) {
            const AABB ba = a->bounds();
            const AABB bb = b->bounds();
            const double ca = (axis == 0) ? (ba.min.x + ba.max.x)
                            : (axis == 1) ? (ba.min.y + ba.max.y)
                                          : (ba.min.z + ba.max.z);
            const double cb = (axis == 0) ? (bb.min.x + bb.max.x)
                            : (axis == 1) ? (bb.min.y + bb.max.y)
                                          : (bb.min.z + bb.max.z);
            return ca < cb;
        });

    const std::size_t mid = nodes.size() / 2;
    auto left  = build({nodes.begin(), nodes.begin() + mid});
    auto right = build({nodes.begin() + mid, nodes.end()});

    AABB b;
    if (left)  b = b.merged(left->bounds());
    if (right) b = b.merged(right->bounds());

    return std::shared_ptr<INode>(new BVHNode(std::move(left),
                                              std::move(right), b));
}

}  // namespace rt

#endif  // RAYTRACER_CORE_BVHNODE_HPP
