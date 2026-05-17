#ifndef RAYTRACER_CORE_CAMERA_HPP
#define RAYTRACER_CORE_CAMERA_HPP

#include "core/CameraConfig.hpp"
#include "interfaces/Ray.hpp"
#include "math/Vector3.hpp"

namespace rt {

class Camera {
public:
    explicit Camera(const CameraConfig& cfg);

    // Integer version: samples at pixel centre (px+0.5, py+0.5).
    Ray generateRay(int px, int py) const noexcept;

    // Continuous version: samples at the exact sub-pixel coordinate (px, py).
    Ray generateRay(double px, double py) const noexcept;

    int width()  const noexcept { return width_; }
    int height() const noexcept { return height_; }

private:
    Vector3 origin_;
    Vector3 lowerLeft_;
    Vector3 horizontal_;
    Vector3 vertical_;
    int     width_;
    int     height_;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_CAMERA_HPP
