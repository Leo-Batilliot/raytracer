// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "parser/SceneParser.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "core/Exceptions.hpp"
#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"
#include "plugins/PluginLoader.hpp"

namespace rt {

// ----------------------------------------------------------------
// Constructor — open + parse
// ----------------------------------------------------------------

SceneParser::SceneParser(const std::filesystem::path& cfgPath)
{
    try {
        config_.readFile(cfgPath.c_str());
    } catch (const libconfig::FileIOException&) {
        throw IOError("cannot read scene file: " + cfgPath.string());
    } catch (const libconfig::ParseException& e) {
        throw InvalidConfigError(
            std::string("parse error in ") + e.getFile()
            + " at line " + std::to_string(e.getLine())
            + ": " + e.getError());
    }
}

// ----------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------

double SceneParser::readScalar(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<double>(static_cast<int>(s));
    return static_cast<double>(s);
}

Vector3 SceneParser::readVec3(const libconfig::Setting& s)
{
    // libconfig stores integers and floats as distinct types.
    // Use double() cast to accept both from the config file.
    auto get = [&](const char* key) -> double {
        const libconfig::Setting& f = s[key];
        if (f.getType() == libconfig::Setting::TypeInt)
            return static_cast<double>(static_cast<int>(f));
        return static_cast<double>(f);
    };
    return {get("x"), get("y"), get("z")};
}

// ----------------------------------------------------------------
// parseCamera
// ----------------------------------------------------------------

CameraConfig SceneParser::parseCamera() const
{
    try {
        const libconfig::Setting& cam = config_.lookup("camera");

        CameraConfig cfg;
        cfg.position = readVec3(cam["position"]);
        cfg.lookAt   = readVec3(cam["lookAt"]);
        cfg.up       = readVec3(cam["up"]);

        // fov may be stored as float or double in the config.
        const libconfig::Setting& fovSetting = cam["fov"];
        if (fovSetting.getType() == libconfig::Setting::TypeInt)
            cfg.fov = static_cast<double>(static_cast<int>(fovSetting));
        else
            cfg.fov = static_cast<double>(fovSetting);

        cfg.width  = static_cast<int>(cam["width"]);
        cfg.height = static_cast<int>(cam["height"]);

        return cfg;
    } catch (const libconfig::SettingNotFoundException& e) {
        throw InvalidConfigError(
            std::string("missing camera field: ") + e.getPath());
    } catch (const libconfig::SettingTypeException& e) {
        throw InvalidConfigError(
            std::string("wrong type for camera field: ") + e.getPath());
    }
}

// ----------------------------------------------------------------
// parsePrimitives
// ----------------------------------------------------------------

void SceneParser::parsePrimitives(SceneBuilder& builder,
                                  const PluginLoader& loader) const
{
    if (!config_.exists("primitives")) return;

    const libconfig::Setting& prims = config_.lookup("primitives");

    auto loadList = [&](const char* key, const char* kind) {
        if (!prims.exists(key)) return;
        const libconfig::Setting& list = prims[key];
        const PluginDescriptor*  desc  = loader.find(PluginKind::Primitive, kind);
        if (!desc) {
            std::cerr << "raytracer: warning: " << kind
                      << " plugin not loaded — skipping " << key << " section\n";
            return;
        }
        for (int i = 0, n = list.getLength(); i < n; ++i) {
            void* raw = desc->create(list[i]);
            if (!raw) {
                std::cerr << "raytracer: warning: " << kind << '[' << i
                          << "]: create() failed — skipping\n";
                continue;
            }
            auto* prim = static_cast<IPrimitive*>(raw);
            builder.addPrimitive(
                std::shared_ptr<IPrimitive>(prim,
                    [desc](IPrimitive* p) { desc->destroy(p); }));
        }
    };

    loadList("spheres", "sphere");  // T1.1
    loadList("planes",  "plane");   // T1.2
}

}  // namespace rt
