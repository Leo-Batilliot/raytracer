// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// SceneBuilder: Builder pattern for constructing an immutable Scene.
//
// Usage:
//   SceneBuilder b;
//   b.setCamera(parser.parseCamera())
//    .addLight(std::make_shared<AmbientLight>(...))
//    .registerMaterial("red", std::make_shared<FlatColor>(...))
//    .setRoot(rootNode);
//   Scene scene = b.build();
//
// build() validates that mandatory fields are present (camera) and
// moves data into the Scene. Throws rt::InvalidConfigError on error.
// Each builder instance should call build() at most once.
//
// Ticket: T0.11.

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

    // Fluent setters — return *this for chaining.
    SceneBuilder& setCamera(const CameraConfig& cfg);
    SceneBuilder& setRoot(std::shared_ptr<INode> root);
    SceneBuilder& addLight(std::shared_ptr<ILight> light);

    // Wrap prim in a PrimitiveNode and add it to the internal GroupNode.
    // The GroupNode becomes the scene root if no explicit root is set.
    // `mat`       — non-owning pointer into the materials_ map (may be null).
    // `transform` — world transform applied via full ray transform in PrimitiveNode
    //               (supports translation + rotation from T2.3).
    SceneBuilder& addPrimitive(std::shared_ptr<IPrimitive> prim,
                               const IMaterial* mat             = nullptr,
                               Transform        transform        = Transform::identity());

    // Register a material under a scene-unique name.
    // The name is used in the .cfg file to associate a material to
    // a primitive.  Duplicate names replace the previous entry
    // (last registration wins — matches libconfig override semantics).
    SceneBuilder& registerMaterial(std::string name,
                                   std::shared_ptr<IMaterial> mat);

    // Look up a registered material by name. Returns nullptr if not found.
    // Call after registerMaterial() (i.e. after parseMaterials) so the
    // parser can resolve material names while building primitives.
    const IMaterial* findMaterial(std::string_view name) const;

    // Validate and assemble the immutable Scene.
    // Throws rt::InvalidConfigError if the camera has not been set.
    Scene build();

private:
    std::optional<CameraConfig>                       camera_;
    std::shared_ptr<INode>                            root_;
    std::vector<std::shared_ptr<INode>>               prims_;  // BVH leaves
    std::vector<std::shared_ptr<ILight>>              lights_;
    std::map<std::string, std::shared_ptr<IMaterial>> materials_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_SCENEBUILDER_HPP
