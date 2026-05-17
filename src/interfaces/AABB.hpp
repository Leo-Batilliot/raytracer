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

    // Default: empty sentinel (+inf / -inf). Any expanded(p) produces a valid box.
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

    constexpr bool isEmpty() const noexcept
    {
        return min.x > max.x || min.y > max.y || min.z > max.z;
    }

    // Returns false for unbounded primitives (planes etc.) with inf components.
    constexpr bool isFinite() const noexcept
    {
        auto fin = [](double v) {
            return v != std::numeric_limits<double>::infinity()
                && v != -std::numeric_limits<double>::infinity()
                && v == v;  // NaN check
        };
        return fin(min.x) && fin(min.y) && fin(min.z)
            && fin(max.x) && fin(max.y) && fin(max.z);
    }

    constexpr AABB expanded(const Vector3& p) const noexcept
    {
        return {
            {std::min(min.x, p.x), std::min(min.y, p.y),
             std::min(min.z, p.z)},
            {std::max(max.x, p.x), std::max(max.y, p.y),
             std::max(max.z, p.z)},
        };
    }

    constexpr AABB merged(const AABB& other) const noexcept
    {
        return {
            {std::min(min.x, other.min.x), std::min(min.y, other.min.y),
             std::min(min.z, other.min.z)},
            {std::max(max.x, other.max.x), std::max(max.y, other.max.y),
             std::max(max.z, other.max.z)},
        };
    }

    // Slab test (Andrew Woo / Kay-Kajiya): true if ray overlaps this box within [tMin, tMax].
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
