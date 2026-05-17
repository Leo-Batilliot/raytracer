#ifndef RAYTRACER_PARSER_SCENEPARSER_HPP
#define RAYTRACER_PARSER_SCENEPARSER_HPP

#include <filesystem>
#include <memory>
#include <vector>

#include <libconfig.h++>

#include "core/AASetting.hpp"
#include "core/CameraConfig.hpp"
#include "core/SceneBuilder.hpp"
#include "interfaces/INode.hpp"
#include "math/Vector3.hpp"

namespace rt {

class PluginLoader;

class SceneParser {
public:
    // Throws rt::IOError or rt::InvalidConfigError on failure.
    explicit SceneParser(const std::filesystem::path& cfgPath);

    CameraConfig parseCamera() const;

    // Returns default AASetting if the block is absent.
    AASetting parseAntialiasing() const;

    void parsePrimitives(SceneBuilder& builder,
        const PluginLoader& loader) const;

    void parseGroups(SceneBuilder& builder,
        const PluginLoader& loader) const;

    void parseLights(SceneBuilder& builder,
        const PluginLoader& loader) const;

    void parseMaterials(SceneBuilder& builder,
        const PluginLoader& loader) const;

private:
    static Vector3 readVec3(const libconfig::Setting& s);
    static double readScalar(const libconfig::Setting& s);

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
