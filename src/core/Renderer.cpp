// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/Renderer.hpp"

#include <algorithm>

#include "interfaces/HitRecord.hpp"

namespace rt {

Renderer::Renderer(const Scene& scene, const Camera& camera)
    : scene_(scene), camera_(camera)
{
}

Color Renderer::shade(const Ray& ray) const
{
    HitRecord rec;
    if (scene_.intersect(ray, rec)) {
        // Visualize surface normal as RGB until T1.5 material system.
        // Maps normal components [-1,1] → [0,1].
        return Color{
            static_cast<float>(rec.normal.x * 0.5 + 0.5),
            static_cast<float>(rec.normal.y * 0.5 + 0.5),
            static_cast<float>(rec.normal.z * 0.5 + 0.5),
        };
    }
    // Sky gradient: white at horizon (dy≈0), blue at zenith (dy≈1).
    const auto t = static_cast<float>(0.5 * (ray.direction.y + 1.0));
    return Color{1.0f, 1.0f, 1.0f} * (1.0f - t)
         + Color{0.5f, 0.7f, 1.0f} * t;
}

void Renderer::renderTile(int x0, int y0, int x1, int y1, Image& img) const
{
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            img.at(x, y) = shade(camera_.generateRay(x, y));
        }
    }
}

void Renderer::renderAll(Image& img) const
{
    const int w = img.width();
    const int h = img.height();
    for (int y0 = 0; y0 < h; y0 += kTileSize) {
        for (int x0 = 0; x0 < w; x0 += kTileSize) {
            renderTile(x0, y0,
                       std::min(x0 + kTileSize, w),
                       std::min(y0 + kTileSize, h),
                       img);
        }
    }
}

}  // namespace rt
