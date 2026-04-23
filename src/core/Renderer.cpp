// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/Renderer.hpp"

#include <algorithm>
#include <atomic>
#include <limits>
#include <thread>
#include <vector>

#include "interfaces/HitRecord.hpp"
#include "interfaces/ILight.hpp"
#include "interfaces/IMaterial.hpp"
#include "interfaces/Ray.hpp"
#include "interfaces/ShadingContext.hpp"
#include "math/Vector3.hpp"

namespace rt {

Renderer::Renderer(const Scene& scene, const Camera& camera)
    : scene_(scene), camera_(camera)
{
}

Color Renderer::shade(const Ray& ray, int depth) const
{
    HitRecord rec;
    if (!scene_.intersect(ray, rec)) {
        // Sky gradient: white at horizon (dy≈0), blue at zenith (dy≈1).
        const auto t = static_cast<float>(0.5 * (ray.direction.y + 1.0));
        return Color{1.0f, 1.0f, 1.0f} * (1.0f - t)
             + Color{0.5f, 0.7f, 1.0f} * t;
    }

    // Albedo from the assigned material; white if none.
    Color albedo = Color::white();
    const IMaterial* mat = rec.material;
    if (mat) {
        const ShadingContext ctx{scene_, rec, ray, 0};
        albedo = mat->shade(ctx);
    }

    // Phong coefficients (defaults reproduce the original Lambert model).
    const float mat_ka = mat ? mat->ka()        : 1.0f;
    const float mat_kd = mat ? mat->kd()        : 1.0f;
    const float mat_sh = mat ? mat->shininess() : 0.0f;
    const Color mat_ks = mat ? mat->specularColor() : Color{};

    // Lighting: ambient + Lambert diffuse + Phong specular (T4.1).
    Color ambient{};
    Color diffuse{};
    Color specular{};
    for (const auto& light : scene_.lights()) {
        if (light->isAmbient()) {
            ambient = ambient + light->ambientContribution() * mat_ka;
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

            diffuse = diffuse + ls.radiance * (lambert * mat_kd);

            // Phong specular: R = 2(N·L)N − L, highlight = (R·V)^shininess.
            if (mat_sh > 0.0f) {
                const double NdotL = rec.normal.dot(ls.direction);
                const Vector3 Rvec = rec.normal * (2.0 * NdotL) - ls.direction;
                const Vector3 Vdir = -ray.direction;  // unit: camera ray is unit
                const float cosRV  = static_cast<float>(
                    std::max(0.0, Rvec.dot(Vdir)));
                const float spec   = std::pow(cosRV, mat_sh);
                specular = specular + mat_ks * (ls.radiance * spec);
            }
        }
    }

    Color color = albedo * (ambient + diffuse) + specular;

    // Reflection (T4.2): blend surface color with reflected ray color.
    const float refl = mat ? mat->reflectivity() : 0.0f;
    if (refl > 0.0f && depth < kMaxBounces) {
        // r = d − 2(d·N)N  — unit since d and N are unit.
        const double NdotD = rec.normal.dot(ray.direction);
        const Vector3 Rdir = ray.direction - rec.normal * (2.0 * NdotD);

        Ray reflected;
        reflected.origin    = rec.point + rec.normal * kRayEpsilon;
        reflected.direction = Rdir;
        reflected.tMin      = kRayEpsilon;
        reflected.tMax      = std::numeric_limits<double>::infinity();

        const Color reflColor = shade(reflected, depth + 1);
        color = color * (1.0f - refl) + reflColor * refl;
    }

    return color;
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
