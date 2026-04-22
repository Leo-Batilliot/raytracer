// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// AABB: axis-aligned bounding box.
//
// Used by IPrimitive::bounds() (T0.7) and INode::bounds() (scene
// graph). Eventually consumed by a BVH (bonus T3.3) for fast ray
// rejection.
//
// Default convention: a default-constructed AABB is EMPTY — its
// component-wise min is +inf and max is -inf. This makes `merged()`
// and `expanded()` trivially correct:
//   AABB b; for (p : points) b = b.expanded(p);   // always grows right
// Concrete well-formed boxes satisfy min <= max on every axis.
//
// Ticket: T0.6.

#ifndef RAYTRACER_INTERFACES_AABB_HPP
#define RAYTRACER_INTERFACES_AABB_HPP

#include <algorithm>
#include <limits>

#include "math/Vector3.hpp"

namespace rt {

struct AABB {
    Vector3 min;
    Vector3 max;

    // Empty box sentinel: inverted bounds (+inf / -inf). Any
    // `expanded(p)` call produces a finite, well-ordered box.
    constexpr AABB() noexcept
        : min{std::numeric_limits<double>::infinity(),
              std::numeric_limits<double>::infinity(),
              std::numeric_limits<double>::infinity()},
          max{-std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity(),
              -std::numeric_limits<double>::infinity()}
    {
    }

    constexpr AABB(const Vector3& min_, const Vector3& max_) noexcept
        : min(min_), max(max_)
    {
    }

    // A box is empty iff any axis has min > max. The default-
    // constructed sentinel satisfies this on all three axes.
    constexpr bool isEmpty() const noexcept
    {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    // Returns a new AABB grown to include point `p`.
    constexpr AABB expanded(const Vector3& p) const noexcept
    {
        return {
            {std::min(min.x, p.x), std::min(min.y, p.y),
             std::min(min.z, p.z)},
            {std::max(max.x, p.x), std::max(max.y, p.y),
             std::max(max.z, p.z)},
        };
    }

    // Returns the union of this box and `other`.
    constexpr AABB merged(const AABB& other) const noexcept
    {
        return {
            {std::min(min.x, other.min.x), std::min(min.y, other.min.y),
             std::min(min.z, other.min.z)},
            {std::max(max.x, other.max.x), std::max(max.y, other.max.y),
             std::max(max.z, other.max.z)},
        };
    }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_AABB_HPP
