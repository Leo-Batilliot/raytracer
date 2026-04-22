// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// CameraConfig: plain data extracted from the scene file's `camera`
// section. Produced by SceneParser::parseCamera(), consumed by
// SceneBuilder::setCamera() and ultimately by Camera (T0.13).
//
// Kept in src/core/ so that Scene.hpp can include it without
// creating a core → parser dependency.
//
// Ticket: T0.11 (extracted from SceneParser.hpp, T0.10).

#ifndef RAYTRACER_CORE_CAMERACONFIG_HPP
#define RAYTRACER_CORE_CAMERACONFIG_HPP

#include "math/Vector3.hpp"

namespace rt {

struct CameraConfig {
    Vector3 position{0.0, 0.0, 5.0};
    Vector3 lookAt  {0.0, 0.0, 0.0};
    Vector3 up      {0.0, 1.0, 0.0};
    double  fov    = 60.0;   // vertical field-of-view in degrees
    int     width  = 800;
    int     height = 600;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_CAMERACONFIG_HPP
