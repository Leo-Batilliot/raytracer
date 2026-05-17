// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/SceneBuilder.hpp"

#include <utility>

#include "core/BVHNode.hpp"
#include "core/Exceptions.hpp"
#include "core/GroupNode.hpp"
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

SceneBuilder& SceneBuilder::addPrimitive(std::shared_ptr<IPrimitive> prim,
                                          const IMaterial* mat,
                                          Transform transform)
{
    prims_.push_back(
        std::make_shared<PrimitiveNode>(std::move(prim), mat, std::move(transform)));
    return *this;
}

SceneBuilder& SceneBuilder::addNode(std::shared_ptr<INode> node)
{
    prims_.push_back(std::move(node));
    return *this;
}

const IMaterial* SceneBuilder::findMaterial(std::string_view name) const
{
    auto it = materials_.find(std::string(name));
    return it != materials_.end() ? it->second.get() : nullptr;
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

    if (root_) {
        scene.root_ = std::move(root_);
    } else {
        // Partition: unbounded primitives (planes, sky domes) cannot be
        // sorted by centroid median — they cause NaN in BVHNode::build.
        // Keep them in a linear GroupNode tested alongside the BVH.
        std::vector<std::shared_ptr<INode>> finite, infinite;
        for (auto& n : prims_) {
            if (n->bounds().isFinite())
                finite.push_back(std::move(n));
            else
                infinite.push_back(std::move(n));
        }

        auto bvh = BVHNode::build(std::move(finite));

        if (infinite.empty()) {
            scene.root_ = bvh;
        } else {
            auto group = std::make_shared<GroupNode>();
            if (bvh) group->addChild(std::move(bvh));
            for (auto& n : infinite) group->addChild(std::move(n));
            scene.root_ = std::move(group);
        }
    }

    scene.lights_    = std::move(lights_);
    scene.materials_ = std::move(materials_);
    return scene;
}

}  // namespace rt
