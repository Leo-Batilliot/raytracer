// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Scene: immutable snapshot of everything the renderer needs.
//
// Constructed exclusively by SceneBuilder::build(). Once built, no
// field can be changed — enabling lock-free multithreading (T3.2).
//
// Ownership model:
//   - root_      : shared_ptr<INode>   — scene graph (Composite)
//   - lights_    : vector of shared_ptr<ILight>
//   - materials_ : map name → shared_ptr<IMaterial>
//   Primitives are owned by nodes which are owned by the root subtree.
//
// IMaterial is forward-declared to break the circular dependency:
//   Scene → IMaterial → ShadingContext → Scene  (would be circular)
// Scene.cpp includes IMaterial.hpp where the full type is needed.
//
// Ticket: T0.11.

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
    // Non-copyable (unique authoritative state), movable (for NRVO).
    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = default;
    Scene& operator=(Scene&&)      = default;

    // Destructor defined in Scene.cpp where IMaterial is complete.
    ~Scene();

    // ── Accessors ────────────────────────────────────────────────
    const CameraConfig& camera() const noexcept { return camera_; }

    const std::vector<std::shared_ptr<ILight>>& lights() const noexcept
    {
        return lights_;
    }

    // Returns nullptr if `name` has no registered material.
    const IMaterial* findMaterial(std::string_view name) const;

    // Intersect the scene graph. Returns false if no hit.
    // Fills `rec` with the closest hit in [ray.tMin, ray.tMax].
    bool intersect(const Ray& ray, HitRecord& rec) const;

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
