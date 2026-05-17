#ifndef RAYTRACER_INTERFACES_INODE_HPP
#define RAYTRACER_INTERFACES_INODE_HPP

#include <memory>

#include "interfaces/AABB.hpp"
#include "interfaces/HitRecord.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class INode {
public:
    virtual ~INode() = default;

    virtual bool intersect(const Ray& ray, HitRecord& rec) const = 0;
    virtual AABB bounds()                                  const = 0;

    // Composite extension point — leaf nodes do nothing.
    virtual void addChild(std::shared_ptr<INode>) {}
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_INODE_HPP
