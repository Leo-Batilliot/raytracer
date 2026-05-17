// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// SceneParser: reads a libconfig++ scene file and exposes its
// sections as typed C++ structs.
//
// Responsibilities:
//   - Open and parse the .cfg file in the constructor.
//   - Expose one method per section (parseCamera, parsePrimitives,
//     parseLights, parseMaterials …). Only parseCamera exists in T0.10;
//     the others land in T0.11+ when SceneBuilder needs them.
//   - Translate libconfig exceptions into rt::InvalidConfigError
//     (or rt::IOError for file-open failures) so main()'s single
//     catch block handles everything.
//
// Owns a `libconfig::Config` by value — the Config object holds the
// parsed tree in memory for the lifetime of the SceneParser. Once
// all data has been extracted into C++ structs and the Scene has been
// built (T0.11), the SceneParser can be destroyed.
//
// Ticket: T0.10.

#ifndef RAYTRACER_PARSER_SCENEPARSER_HPP
#define RAYTRACER_PARSER_SCENEPARSER_HPP

#include <filesystem>
#include <memory>
#include <vector>

#include <libconfig.h++>

#include "core/CameraConfig.hpp"  // CameraConfig moved here in T0.11
#include "core/SceneBuilder.hpp"
#include "interfaces/INode.hpp"
#include "math/Vector3.hpp"

namespace rt {

class PluginLoader;  // defined in plugins/PluginLoader.hpp

// ----------------------------------------------------------------
// SceneParser
// ----------------------------------------------------------------

class SceneParser {
public:
    // Opens and parses `cfgPath`. Throws:
    //   rt::IOError           — file not found or not readable
    //   rt::InvalidConfigError — libconfig syntax error
    explicit SceneParser(const std::filesystem::path& cfgPath);

    // Reads the mandatory `camera` group. Throws:
    //   rt::InvalidConfigError — missing / wrong-typed field
    CameraConfig parseCamera() const;

    // Reads the optional `primitives` section and registers each
    // primitive with `builder` via the plugin registry `loader`.
    void parsePrimitives(SceneBuilder& builder,
        const PluginLoader& loader) const;

    // Reads the optional `groups` section (T4.5 — scene graph).
    // Each group entry has an optional transform (translate + rotate) and a
    // `children` sub-section using the same primitive schema as `primitives`.
    // Children are wrapped in a TransformNode whose transform is the group's.
    void parseGroups(SceneBuilder& builder,
        const PluginLoader& loader) const;

    // Reads the optional `lights` section and registers each light.
    void parseLights(SceneBuilder& builder,
        const PluginLoader& loader) const;

    // Reads the optional `materials` group and registers each named material.
    // Each entry must have a `type` field matching a loaded Material plugin.
    void parseMaterials(SceneBuilder& builder,
        const PluginLoader& loader) const;

private:
    // Read a 3-component sub-group {x, y, z} from a Setting.
    static Vector3 readVec3(const libconfig::Setting& s);

    // Read a scalar Setting that may be TypeInt or TypeFloat.
    static double readScalar(const libconfig::Setting& s);

    // Build PrimitiveNodes from parent[key] (plugin kind = `kind`) and
    // append them to `nodes`. Uses `matLookup` only for findMaterial().
    // Shared between parsePrimitives() and parseGroups().
    void buildPrimitiveList(const libconfig::Setting& parent,
        const char* key,
        const char* kind,
        const SceneBuilder& matLookup,
        const PluginLoader& loader,
        std::vector<std::shared_ptr<INode>>& nodes) const;

    libconfig::Config config_;
};

}  // namespace rt

#endif  // RAYTRACER_PARSER_SCENEPARSER_HPP
