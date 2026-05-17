#include "core/Camera.hpp"

#include <cmath>
#include <numbers>

namespace rt {

Camera::Camera(const CameraConfig& cfg)
    : width_(cfg.width), height_(cfg.height)
{
    // w points from lookAt toward position; camera looks along -w.
    const Vector3 w = (cfg.position - cfg.lookAt).normalized();
    const Vector3 u = cfg.up.cross(w).normalized();
    const Vector3 v = w.cross(u);

    const double halfH = std::tan(cfg.fov * std::numbers::pi / 360.0);
    const double halfW = halfH * static_cast<double>(cfg.width) / cfg.height;

    origin_     = cfg.position;
    horizontal_ = 2.0 * halfW * u;
    vertical_   = 2.0 * halfH * v;
    lowerLeft_  = origin_ - halfW * u - halfH * v - w;
}

Ray Camera::generateRay(int px, int py) const noexcept
{
    return generateRay(px + 0.5, py + 0.5);
}

Ray Camera::generateRay(double px, double py) const noexcept
{
    const double s = px / width_;
    const double t = 1.0 - py / height_;  // flip y: image row 0 = top

    const Vector3 target = lowerLeft_ + s * horizontal_ + t * vertical_;
    const Vector3 rawDir = target - origin_;

    const double len = rawDir.length();
    const Vector3 dir = (len > 0.0) ? rawDir / len : Vector3{0.0, 0.0, -1.0};

    return Ray{origin_, dir};
}

}  // namespace rt
