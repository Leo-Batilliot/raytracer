// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// INode: Composite pattern node in the scene graph.
//
// Both leaf nodes (holding a single primitive + transform) and
// group nodes (holding child INodes) implement this interface.
// The renderer only knows INode — it never touches IPrimitive
// directly.
//
// Contract:
//   intersect — walks the subtree and fills `rec` with the
//               *closest* hit in [ray.tMin, ray.tMax] world-space.
//               Implementations narrow `ray.tMax` as they find hits
//               (no need for a separate "closest so far" variable in
//               the caller).
//   bounds    — returns the AABB in *world* space (post-transform).
//               Used by BVH (T3.3) and scene construction sanity checks.
//   addChild  — default no-op; overridden by GroupNode.
//               Calling it on a leaf is silently ignored, which lets
//               the Builder add children without downcasting.
//
// Ticket: T0.7.

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
