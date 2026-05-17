#ifndef RAYTRACER_CORE_CAMERACONFIG_HPP
#define RAYTRACER_CORE_CAMERACONFIG_HPP

#include "math/Vector3.hpp"

namespace rt {

struct CameraConfig {
    Vector3 position{0.0, 0.0, 5.0};
    Vector3 lookAt  {0.0, 0.0, 0.0};
    Vector3 up      {0.0, 1.0, 0.0};
    double  fov    = 60.0;
    int     width  = 800;
    int     height = 600;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_CAMERACONFIG_HPP
