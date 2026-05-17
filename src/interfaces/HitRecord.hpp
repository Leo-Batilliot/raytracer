#ifndef RAYTRACER_INTERFACES_HITRECORD_HPP
#define RAYTRACER_INTERFACES_HITRECORD_HPP

#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace rt {

class IMaterial;
class INode;

struct HitRecord {
    double t = 0.0;
    Vector3 point{};
    Vector3 normal{};
    Vector2 uv{};
    const IMaterial* material = nullptr;
    const INode* node = nullptr;
    // True if the ray hit the outside of the surface.
    bool frontFace = true;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_HITRECORD_HPP
