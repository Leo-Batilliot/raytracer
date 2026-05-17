// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Camera: pinhole model (T0.13).
//
// Precomputes a world-space virtual screen from CameraConfig at
// construction time so generateRay() is a pure arithmetic hot path.
//
// Screen parameterisation (at distance 1 along -w from origin):
//   lowerLeft_  : world position of the screen's bottom-left corner
//   horizontal_ : world vector spanning the full screen width  (left→right)
//   vertical_   : world vector spanning the full screen height (bottom→top)
//
// Pixel convention: (0,0) is top-left; y increases downward.
// generateRay() samples at pixel centre (px+0.5, py+0.5).

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
    // Used by the antialiasing logic in Renderer::shadePixel().
    // Caller is responsible for the offset (e.g. px + (sx+0.5)/N).
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
