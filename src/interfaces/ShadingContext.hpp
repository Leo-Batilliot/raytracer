#ifndef RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP
#define RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP

#include "interfaces/HitRecord.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class Scene;

struct ShadingContext {
    const Scene& scene;
    const HitRecord& hit;
    const Ray& incomingRay;
    int depth = 0;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP
