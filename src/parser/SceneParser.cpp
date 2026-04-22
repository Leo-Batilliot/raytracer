// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "parser/SceneParser.hpp"

#include <string>

#include "core/Exceptions.hpp"

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

}  // namespace rt
