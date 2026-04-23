// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// AABB: axis-aligned bounding box.
//
// Used by IPrimitive::bounds() (T0.7) and INode::bounds() (scene
// graph). intersects(Ray) provides the slab test consumed by BVH (T3.3).
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

#include "interfaces/Ray.hpp"
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

    // Slab test: true if `ray` overlaps this box within [ray.tMin, ray.tMax].
    // Uses the Andrew Woo / Kay-Kajiya slab method — branchless per axis.
    // An empty box always returns false.
    constexpr bool intersects(const Ray& ray) const noexcept
    {
        if (isEmpty()) return false;

        double tlo = ray.tMin;
        double thi = ray.tMax;

        // X slab
        {
            const double invD = 1.0 / ray.direction.x;
            double t0 = (min.x - ray.origin.x) * invD;
            double t1 = (max.x - ray.origin.x) * invD;
            if (invD < 0.0) { const double tmp = t0; t0 = t1; t1 = tmp; }
            tlo = t0 > tlo ? t0 : tlo;
            thi = t1 < thi ? t1 : thi;
            if (thi < tlo) return false;
        }
        // Y slab
        {
            const double invD = 1.0 / ray.direction.y;
            double t0 = (min.y - ray.origin.y) * invD;
            double t1 = (max.y - ray.origin.y) * invD;
            if (invD < 0.0) { const double tmp = t0; t0 = t1; t1 = tmp; }
            tlo = t0 > tlo ? t0 : tlo;
            thi = t1 < thi ? t1 : thi;
            if (thi < tlo) return false;
        }
        // Z slab
        {
            const double invD = 1.0 / ray.direction.z;
            double t0 = (min.z - ray.origin.z) * invD;
            double t1 = (max.z - ray.origin.z) * invD;
            if (invD < 0.0) { const double tmp = t0; t0 = t1; t1 = tmp; }
            tlo = t0 > tlo ? t0 : tlo;
            thi = t1 < thi ? t1 : thi;
            if (thi < tlo) return false;
        }
        return true;
    }
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_AABB_HPP
