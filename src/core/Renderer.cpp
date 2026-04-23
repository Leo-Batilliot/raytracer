// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/Renderer.hpp"

#include <algorithm>
#include <atomic>
#include <thread>
#include <vector>

#include "interfaces/HitRecord.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/IMaterial.hpp"
#include "interfaces/Ray.hpp"
#include "interfaces/ShadingContext.hpp"

namespace rt {

Renderer::Renderer(const Scene& scene, const Camera& camera)
    : scene_(scene), camera_(camera)
{
}

Color Renderer::shade(const Ray& ray) const
{
    HitRecord rec;
    if (!scene_.intersect(ray, rec)) {
        // Sky gradient: white at horizon (dy≈0), blue at zenith (dy≈1).
        const auto t = static_cast<float>(0.5 * (ray.direction.y + 1.0));
        return Color{1.0f, 1.0f, 1.0f} * (1.0f - t)
             + Color{0.5f, 0.7f, 1.0f} * t;
    }

    // Albedo from the assigned material; white if none (T1.7 assigns materials).
    Color albedo = Color::white();
    if (rec.material) {
        const ShadingContext ctx{scene_, rec, ray, 0};
        albedo = rec.material->shade(ctx);
    }

    // Lighting: ambient accumulation + Lambert diffuse (with shadow test).
    Color ambient{};
    Color diffuse{};
    for (const auto& light : scene_.lights()) {
        if (light->isAmbient()) {
            ambient = ambient + light->ambientContribution();
        } else {
            const LightSample ls = light->sample(rec.point);
            const float lambert = static_cast<float>(
                std::max(0.0, rec.normal.dot(ls.direction)));
            if (lambert <= 0.0f) continue;

            // Shadow ray biased along normal to avoid self-intersection.
            Ray shadow;
            shadow.origin    = rec.point + rec.normal * kRayEpsilon;
            shadow.direction = ls.direction;
            shadow.tMin      = kRayEpsilon;
            shadow.tMax      = ls.distance - kRayEpsilon;
            if (scene_.occluded(shadow)) continue;

            diffuse = diffuse + ls.radiance * lambert;
        }
    }

    return albedo * (ambient + diffuse);
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

    struct Tile { int x0, y0, x1, y1; };
    std::vector<Tile> tiles;
    for (int y0 = 0; y0 < h; y0 += kTileSize)
        for (int x0 = 0; x0 < w; x0 += kTileSize)
            tiles.push_back({x0, y0,
                             std::min(x0 + kTileSize, w),
                             std::min(y0 + kTileSize, h)});

    const int nTiles = static_cast<int>(tiles.size());
    std::atomic<int> next{0};

    const int nThreads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    workers.reserve(nThreads);

    for (int i = 0; i < nThreads; ++i) {
        workers.emplace_back([&]() {
            for (int idx = next.fetch_add(1, std::memory_order_relaxed);
                 idx < nTiles;
                 idx = next.fetch_add(1, std::memory_order_relaxed)) {
                const Tile& t = tiles[idx];
                renderTile(t.x0, t.y0, t.x1, t.y1, img);
            }
        });
    }

    for (auto& w : workers) w.join();
}

}  // namespace rt
