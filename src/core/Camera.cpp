// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/Camera.hpp"

#include <cmath>
#include <numbers>

namespace rt {

Camera::Camera(const CameraConfig& cfg)
    : width_(cfg.width), height_(cfg.height)
{
    // Orthonormal basis: w points from lookAt toward position (camera looks along -w).
    const Vector3 w = (cfg.position - cfg.lookAt).normalized();
    const Vector3 u = cfg.up.cross(w).normalized();
    const Vector3 v = w.cross(u);  // already unit: w and u are orthonormal

    const double halfH = std::tan(cfg.fov * std::numbers::pi / 360.0);
    const double halfW = halfH * static_cast<double>(cfg.width) / cfg.height;

    origin_     = cfg.position;
    horizontal_ = 2.0 * halfW * u;
    vertical_   = 2.0 * halfH * v;
    lowerLeft_  = origin_ - halfW * u - halfH * v - w;
}

Ray Camera::generateRay(int px, int py) const noexcept
{
    const double s = (px + 0.5) / width_;
    const double t = 1.0 - (py + 0.5) / height_;  // flip y: image row 0 = top

    // Screen point in world space, then direction from eye.
    const Vector3 target = lowerLeft_ + s * horizontal_ + t * vertical_;
    const Vector3 rawDir = target - origin_;

    // Safe normalization: for a valid camera config rawDir is never zero.
    const double len = rawDir.length();
    const Vector3 dir = (len > 0.0) ? rawDir / len : Vector3{0.0, 0.0, -1.0};

    return Ray{origin_, dir};
}

}  // namespace rt
