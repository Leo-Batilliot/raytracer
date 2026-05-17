#ifndef RAYTRACER_CORE_SCENEBUILDER_HPP
#define RAYTRACER_CORE_SCENEBUILDER_HPP

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "core/BVHNode.hpp"
#include "core/CameraConfig.hpp"
#include "core/Scene.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/IMaterial.hpp"
#include "interfaces/INode.hpp"
#include "interfaces/IPrimitive.hpp"
#include "math/Transform.hpp"

namespace rt {

class SceneBuilder {
public:
    SceneBuilder()  = default;
    ~SceneBuilder() = default;

    SceneBuilder& setCamera(const CameraConfig& cfg);
    SceneBuilder& setRoot(std::shared_ptr<INode> root);
    SceneBuilder& addLight(std::shared_ptr<ILight> light);

    SceneBuilder& addPrimitive(std::shared_ptr<IPrimitive> prim,
                               const IMaterial* mat             = nullptr,
                               Transform        transform        = Transform::identity());

    SceneBuilder& addNode(std::shared_ptr<INode> node);

    // Duplicate names replace the previous entry (last registration wins).
    SceneBuilder& registerMaterial(std::string name,
                                   std::shared_ptr<IMaterial> mat);

    // Returns nullptr if not found.
    const IMaterial* findMaterial(std::string_view name) const;

    // Throws rt::InvalidConfigError if the camera has not been set.
    Scene build();

private:
    std::optional<CameraConfig>                       camera_;
    std::shared_ptr<INode>                            root_;
    std::vector<std::shared_ptr<INode>>               prims_;
    std::vector<std::shared_ptr<ILight>>              lights_;
    std::map<std::string, std::shared_ptr<IMaterial>> materials_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_SCENEBUILDER_HPP
