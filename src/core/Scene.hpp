#ifndef RAYTRACER_CORE_SCENE_HPP
#define RAYTRACER_CORE_SCENE_HPP

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "core/CameraConfig.hpp"
#include "interfaces/HitRecord.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/INode.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class IMaterial;    // forward decl — full def in Scene.cpp
class SceneBuilder; // only builder may construct

class Scene {
public:
    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = default;
    Scene& operator=(Scene&&)      = default;

    ~Scene();

    const CameraConfig& camera() const noexcept { return camera_; }

    const std::vector<std::shared_ptr<ILight>>& lights() const noexcept
    {
        return lights_;
    }

    const IMaterial* findMaterial(std::string_view name) const;

    // Fills `rec` with the closest hit in [ray.tMin, ray.tMax].
    bool intersect(const Ray& ray, HitRecord& rec) const;

    // Returns true if any geometry blocks the shadow ray.
    bool occluded(const Ray& shadowRay) const;

private:
    friend class SceneBuilder;
    Scene() = default;

    CameraConfig                                       camera_;
    std::shared_ptr<INode>                             root_;
    std::vector<std::shared_ptr<ILight>>               lights_;
    std::map<std::string, std::shared_ptr<IMaterial>>  materials_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_SCENE_HPP
