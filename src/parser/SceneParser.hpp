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

#include <libconfig.h++>

#include "math/Vector3.hpp"

namespace rt {

// ----------------------------------------------------------------
// Plain data structs — one per config section.
// SceneParser → SceneBuilder → Scene (T0.11).
// ----------------------------------------------------------------

struct CameraConfig {
    Vector3 position{0.0, 0.0, 5.0};
    Vector3 lookAt  {0.0, 0.0, 0.0};
    Vector3 up      {0.0, 1.0, 0.0};
    double  fov    = 60.0;   // vertical field-of-view, degrees
    int     width  = 800;
    int     height = 600;
};

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

private:
    // Read a 3-component sub-group {x, y, z} from a Setting.
    static Vector3 readVec3(const libconfig::Setting& s);

    libconfig::Config config_;
};

}  // namespace rt

#endif  // RAYTRACER_PARSER_SCENEPARSER_HPP
