// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "parser/SceneParser.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <numbers>

#include "core/Exceptions.hpp"
#include "core/GroupNode.hpp"
#include "core/PrimitiveNode.hpp"
#include "core/TransformNode.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/IMaterial.hpp"
#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"
#include "math/Transform.hpp"
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
// parseAntialiasing  (T4.9)
// ----------------------------------------------------------------

AASetting SceneParser::parseAntialiasing() const
{
    AASetting aa;
    if (!config_.exists("antialiasing")) return aa;
    const libconfig::Setting& s = config_.lookup("antialiasing");
    if (s.exists("samples"))
        aa.samples = static_cast<int>(s["samples"]);
    if (s.exists("adaptive"))
        aa.adaptive = static_cast<bool>(s["adaptive"]);
    if (s.exists("threshold"))
        aa.threshold = static_cast<float>(readScalar(s["threshold"]));
    if (aa.samples < 1) aa.samples = 1;
    if (aa.samples > 8) aa.samples = 8;
    return aa;
}

// ----------------------------------------------------------------
// buildPrimitiveList  (shared helper — T4.5)
// ----------------------------------------------------------------

void SceneParser::buildPrimitiveList(const libconfig::Setting& parent,
    const char* key,
    const char* kind,
    const SceneBuilder& matLookup,
    const PluginLoader& loader,
    std::vector<std::shared_ptr<INode>>& nodes) const
{
    if (!parent.exists(key)) return;

    const libconfig::Setting& list = parent[key];
    const PluginDescriptor* desc   = loader.find(PluginKind::Primitive, kind);
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
            const std::string matName = static_cast<const char*>(entry["material"]);
            mat = matLookup.findMaterial(matName);
            if (!mat)
                std::cerr << "raytracer: warning: " << kind << '[' << i
                    << "]: material '" << matName << "' not found — ignored\n";
        }

        // Optional rotation (degrees per axis, applied X → Y → Z).
        Transform rotation = Transform::identity();
        if (entry.exists("rotate")) {
            constexpr double kDeg2Rad = std::numbers::pi / 180.0;
            const libconfig::Setting& rot = entry["rotate"];
            if (rot.exists("x"))
                rotation = Transform::rotationX(readScalar(rot["x"]) * kDeg2Rad);
            if (rot.exists("y"))
                rotation = Transform::rotationY(readScalar(rot["y"]) * kDeg2Rad) * rotation;
            if (rot.exists("z"))
                rotation = Transform::rotationZ(readScalar(rot["z"]) * kDeg2Rad) * rotation;
        }

        // Optional translation (applied after rotation).
        Transform translation = Transform::identity();
        if (entry.exists("translate")) {
            const Vector3 t = readVec3(entry["translate"]);
            translation = Transform::translation(t.x, t.y, t.z);
        }

        nodes.push_back(std::make_shared<PrimitiveNode>(
            std::move(prim), mat, translation * rotation));
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

    std::vector<std::shared_ptr<INode>> nodes;
    buildPrimitiveList(prims, "spheres",   "sphere",   builder, loader, nodes);
    buildPrimitiveList(prims, "planes",    "plane",    builder, loader, nodes);
    buildPrimitiveList(prims, "cylinders", "cylinder", builder, loader, nodes);
    buildPrimitiveList(prims, "cones",     "cone",     builder, loader, nodes);

    for (auto& n : nodes)
        builder.addNode(std::move(n));
}

// ----------------------------------------------------------------
// parseGroups  (T4.5 — scene graph)
// ----------------------------------------------------------------

void SceneParser::parseGroups(SceneBuilder& builder,
    const PluginLoader& loader) const
{
    if (!config_.exists("groups")) return;

    const libconfig::Setting& groups = config_.lookup("groups");

    for (int i = 0, n = groups.getLength(); i < n; ++i) {
        const libconfig::Setting& entry = groups[i];

        // Parse group transform (translate + rotate, same convention as primitives).
        Transform rotation = Transform::identity();
        if (entry.exists("rotate")) {
            constexpr double kDeg2Rad = std::numbers::pi / 180.0;
            const libconfig::Setting& rot = entry["rotate"];
            if (rot.exists("x"))
                rotation = Transform::rotationX(readScalar(rot["x"]) * kDeg2Rad);
            if (rot.exists("y"))
                rotation = Transform::rotationY(readScalar(rot["y"]) * kDeg2Rad) * rotation;
            if (rot.exists("z"))
                rotation = Transform::rotationZ(readScalar(rot["z"]) * kDeg2Rad) * rotation;
        }
        Transform translation = Transform::identity();
        if (entry.exists("translate")) {
            const Vector3 t = readVec3(entry["translate"]);
            translation = Transform::translation(t.x, t.y, t.z);
        }
        const Transform groupTransform = translation * rotation;

        // Parse children (same primitive schema as the `primitives` section).
        if (!entry.exists("children")) {
            std::cerr << "raytracer: warning: group[" << i
                << "]: no 'children' section — skipping\n";
            continue;
        }
        const libconfig::Setting& children = entry["children"];

        std::vector<std::shared_ptr<INode>> childNodes;
        buildPrimitiveList(children, "spheres",   "sphere",   builder, loader, childNodes);
        buildPrimitiveList(children, "planes",    "plane",    builder, loader, childNodes);
        buildPrimitiveList(children, "cylinders", "cylinder", builder, loader, childNodes);
        buildPrimitiveList(children, "cones",     "cone",     builder, loader, childNodes);

        if (childNodes.empty()) continue;

        // Assemble: wrap children in a GroupNode, then apply the group transform.
        auto group = std::make_shared<GroupNode>();
        for (auto& child : childNodes)
            group->addChild(std::move(child));

        builder.addNode(std::make_shared<TransformNode>(std::move(group), groupTransform));
    }
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
        const PluginDescriptor* desc   = loader.find(PluginKind::Light, kind);
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
    loadList("point",       "point");       // T4.10
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
