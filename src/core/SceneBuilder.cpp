// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/SceneBuilder.hpp"

#include <utility>

#include "core/Exceptions.hpp"
#include "core/PrimitiveNode.hpp"

namespace rt {

SceneBuilder& SceneBuilder::setCamera(const CameraConfig& cfg)
{
    camera_ = cfg;
    return *this;
}

SceneBuilder& SceneBuilder::setRoot(std::shared_ptr<INode> root)
{
    root_ = std::move(root);
    return *this;
}

SceneBuilder& SceneBuilder::addPrimitive(std::shared_ptr<IPrimitive> prim)
{
    if (!group_) group_ = std::make_shared<GroupNode>();
    group_->addChild(std::make_shared<PrimitiveNode>(std::move(prim)));
    return *this;
}

SceneBuilder& SceneBuilder::addLight(std::shared_ptr<ILight> light)
{
    lights_.push_back(std::move(light));
    return *this;
}

SceneBuilder& SceneBuilder::registerMaterial(std::string name,
                                             std::shared_ptr<IMaterial> mat)
{
    materials_[std::move(name)] = std::move(mat);
    return *this;
}

Scene SceneBuilder::build()
{
    if (!camera_) {
        throw InvalidConfigError(
            "scene is missing a camera section — cannot build Scene");
    }

    Scene scene;
    scene.camera_    = std::move(*camera_);
    // Explicit setRoot() wins; fall back to the addPrimitive() group.
    scene.root_      = root_ ? std::move(root_) : std::move(group_);
    scene.lights_    = std::move(lights_);
    scene.materials_ = std::move(materials_);
    return scene;
}

}  // namespace rt
