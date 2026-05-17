#ifndef RAYTRACER_INTERFACES_IPRIMITIVE_HPP
#define RAYTRACER_INTERFACES_IPRIMITIVE_HPP

#include "interfaces/AABB.hpp"
#include "interfaces/HitRecord.hpp"
#include "interfaces/Ray.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace rt {

class IPrimitive {
public:
    virtual ~IPrimitive() = default;

    virtual bool    intersect(const Ray& ray, HitRecord& rec) const = 0;
    virtual AABB    bounds()                                   const = 0;
    virtual Vector2 uvAt(const Vector3& localPoint)            const = 0;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_IPRIMITIVE_HPP
