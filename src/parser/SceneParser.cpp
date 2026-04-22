// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "parser/SceneParser.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "core/Exceptions.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/IMaterial.hpp"
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
            const libconfig::Setting& entry = list[i];
            void* raw = desc->create(entry);
            if (!raw) {
                std::cerr << "raytracer: warning: " << kind << '[' << i
                          << "]: create() failed — skipping\n";
                continue;
            }
            auto prim = std::shared_ptr<IPrimitive>(
                static_cast<IPrimitive*>(raw),
                [desc](IPrimitive* p) { desc->destroy(p); });

            // Optional material reference.
            const IMaterial* mat = nullptr;
            if (entry.exists("material")) {
                const std::string matName =
                    static_cast<const char*>(entry["material"]);
                mat = builder.findMaterial(matName);
                if (!mat) {
                    std::cerr << "raytracer: warning: " << kind << '[' << i
                              << "]: material '" << matName
                              << "' not found — ignored\n";
                }
            }

            // Optional translation.
            Vector3 translate{};
            if (entry.exists("translate")) {
                translate = readVec3(entry["translate"]);
            }

            builder.addPrimitive(std::move(prim), mat, translate);
        }
    };

    loadList("spheres",    "sphere");    // T1.1
    loadList("planes",     "plane");     // T1.2
    loadList("cylinders",  "cylinder");  // T2.1
}

// ----------------------------------------------------------------
// parseLights
// ----------------------------------------------------------------

void SceneParser::parseLights(SceneBuilder& builder,
                               const PluginLoader& loader) const
{
    if (!config_.exists("lights")) return;

    const libconfig::Setting& lights = config_.lookup("lights");

    auto loadList = [&](const char* key, const char* kind) {
        if (!lights.exists(key)) return;
        const libconfig::Setting& list = lights[key];
        const PluginDescriptor*  desc  = loader.find(PluginKind::Light, kind);
        if (!desc) {
            std::cerr << "raytracer: warning: " << kind
                      << " light plugin not loaded — skipping " << key << '\n';
            return;
        }
        for (int i = 0, n = list.getLength(); i < n; ++i) {
            void* raw = desc->create(list[i]);
            if (!raw) {
                std::cerr << "raytracer: warning: " << kind << '[' << i
                          << "]: create() failed — skipping\n";
                continue;
            }
            auto* light = static_cast<ILight*>(raw);
            builder.addLight(std::shared_ptr<ILight>(light,
                [desc](ILight* l) { desc->destroy(l); }));
        }
    };

    loadList("ambient",     "ambient");     // T1.3
    loadList("directional", "directional"); // T1.4
}

// ----------------------------------------------------------------
// parseMaterials
// ----------------------------------------------------------------

void SceneParser::parseMaterials(SceneBuilder& builder,
                                  const PluginLoader& loader) const
{
    if (!config_.exists("materials")) return;

    const libconfig::Setting& mats = config_.lookup("materials");

    for (int i = 0, n = mats.getLength(); i < n; ++i) {
        const libconfig::Setting& entry = mats[i];
        const std::string name = entry.getName();

        if (!entry.exists("type")) {
            std::cerr << "raytracer: warning: material '" << name
                      << "' has no 'type' field — skipping\n";
            continue;
        }

        const std::string type =
            static_cast<const char*>(entry["type"]);
        const PluginDescriptor* desc =
            loader.find(PluginKind::Material, type);
        if (!desc) {
            std::cerr << "raytracer: warning: material type '" << type
                      << "' not loaded — skipping '" << name << "'\n";
            continue;
        }

        void* raw = desc->create(entry);
        if (!raw) {
            std::cerr << "raytracer: warning: material '" << name
                      << "': create() failed — skipping\n";
            continue;
        }

        builder.registerMaterial(name,
            std::shared_ptr<IMaterial>(static_cast<IMaterial*>(raw),
                [desc](IMaterial* m) { desc->destroy(m); }));
    }
}

}  // namespace rt
