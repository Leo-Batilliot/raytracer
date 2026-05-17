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

Renderer::Renderer(const Scene& scene, const Camera& camera, AASetting aa)
    : scene_(scene), camera_(camera), aa_(aa)
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
    const float refl  = mat ? mat->reflectivity()  : 0.0f;
    const float transp = mat ? mat->transparency() : 0.0f;

    if (refl > 0.0f && transp == 0.0f && depth < kMaxBounces) {
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

    // Refraction / transparency (T4.3): Snell's law + Schlick Fresnel.
    if (transp > 0.0f && depth < kMaxBounces) {
        const float matIOR = mat ? mat->ior() : 1.5f;
        // eta = ratio n1/n2; frontFace means entering the medium.
        const float  eta   = rec.frontFace ? (1.0f / matIOR) : matIOR;
        const float  cosI  = static_cast<float>(-ray.direction.dot(rec.normal));

        // Perpendicular component of refracted direction.
        const Vector3 rPerp = (ray.direction + rec.normal * static_cast<double>(cosI))
                              * static_cast<double>(eta);
        const double rPerpLenSq = rPerp.lengthSquared();

        // Build reflected ray (needed for TIR and Fresnel blend).
        const double  NdotD = rec.normal.dot(ray.direction);
        const Vector3 Rdir  = ray.direction - rec.normal * (2.0 * NdotD);
        Ray reflRay;
        reflRay.origin    = rec.point + rec.normal * kRayEpsilon;
        reflRay.direction = Rdir;
        reflRay.tMin      = kRayEpsilon;
        reflRay.tMax      = std::numeric_limits<double>::infinity();
        const Color reflColor = shade(reflRay, depth + 1);

        if (rPerpLenSq >= 1.0) {
            // Total internal reflection — full reflection, no transmission.
            color = color * (1.0f - transp) + reflColor * transp;
        } else {
            // Schlick Fresnel: use angle in the less-dense medium (air side).
            const float schlickCos = rec.frontFace ? cosI
                                   : static_cast<float>(std::sqrt(1.0 - rPerpLenSq));
            const float r0val = (1.0f - matIOR) / (1.0f + matIOR);
            const float R0    = r0val * r0val;
            const float fresnel = R0 + (1.0f - R0) * std::pow(1.0f - schlickCos, 5.0f);

            // Refracted direction: rPerp + rPar.
            const Vector3 rPar      = rec.normal * (-std::sqrt(1.0 - rPerpLenSq));
            const Vector3 refractDir = rPerp + rPar;

            Ray refractRay;
            refractRay.origin    = rec.point - rec.normal * kRayEpsilon;
            refractRay.direction = refractDir;
            refractRay.tMin      = kRayEpsilon;
            refractRay.tMax      = std::numeric_limits<double>::infinity();
            const Color refractColor = shade(refractRay, depth + 1);

            color = color * (1.0f - transp)
                  + (reflColor * fresnel
                  +  refractColor * albedo * (1.0f - fresnel)) * transp;
        }
    }

    return color;
}

// ----------------------------------------------------------------
// Anti-aliasing helpers (T4.9)
// ----------------------------------------------------------------

Color Renderer::uniformSample(int px, int py, int n) const
{
    Color sum{};
    for (int sy = 0; sy < n; ++sy) {
        for (int sx = 0; sx < n; ++sx) {
            const double ox = (sx + 0.5) / n;
            const double oy = (sy + 0.5) / n;
            sum = sum + shade(camera_.generateRay(px + ox, py + oy));
        }
    }
    return sum / static_cast<float>(n * n);
}

Color Renderer::shadePixel(int px, int py) const
{
    // No AA: single ray at pixel centre.
    if (aa_.samples <= 1 && !aa_.adaptive)
        return shade(camera_.generateRay(px, py));

    // Uniform supersampling (T4.9 basic).
    if (!aa_.adaptive)
        return uniformSample(px, py, aa_.samples);

    // Adaptive supersampling (T4.9 adaptive):
    // Start with 2×2 (4 samples). If any sample deviates from the mean
    // by more than aa_.threshold, upgrade to max(aa_.samples, 3)×… .

    const Color c00 = shade(camera_.generateRay(px + 0.25, py + 0.25));
    const Color c10 = shade(camera_.generateRay(px + 0.75, py + 0.25));
    const Color c01 = shade(camera_.generateRay(px + 0.25, py + 0.75));
    const Color c11 = shade(camera_.generateRay(px + 0.75, py + 0.75));

    const Color mean = (c00 + c10 + c01 + c11) * 0.25f;

    // Max per-channel deviation from mean across the 4 samples.
    auto diff = [](const Color& a, const Color& b) {
        return std::max({std::abs(a.r - b.r),
                         std::abs(a.g - b.g),
                         std::abs(a.b - b.b)});
    };
    const float maxDev = std::max({diff(mean, c00), diff(mean, c10),
                                   diff(mean, c01), diff(mean, c11)});

    if (maxDev <= aa_.threshold)
        return mean;  // Flat area: 4 samples suffice.

    // High variance (edge/boundary): full N×N refinement.
    const int n = std::max(aa_.samples, 3);
    return uniformSample(px, py, n);
}

void Renderer::renderTile(int x0, int y0, int x1, int y1, Image& img) const
{
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            img.at(x, y) = shadePixel(x, y);
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
