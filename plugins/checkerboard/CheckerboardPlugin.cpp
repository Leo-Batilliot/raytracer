// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Checkerboard procedural material plugin (T4.6).
//
// Generates an infinite checkerboard pattern using the world-space hit
// position. Each cell has dimensions (1/scale)³ in world units. Cells
// whose coordinates sum to an even integer get color1; odd cells get
// color2.
//
// Using world position (rather than UV) makes the pattern consistent
// across all primitives (plane, sphere, cylinder, cone) and avoids UV
// seam artefacts. Scale a floor plane by choosing `scale` so that
// 1/scale matches the desired tile size in scene units.
//
// Supports the same Phong coefficients as the phong plugin (ka, kd,
// ks, shininess) — the two colors act as the diffuse albedo, shaded
// with the same Phong model the renderer drives.
//
// Config block:
//   type   = "checkerboard";
//   color1 = { r = 1.0; g = 1.0; b = 1.0; };  // light squares
//   color2 = { r = 0.1; g = 0.1; b = 0.1; };  // dark squares
//   scale  = 1.0;   // tiles per world unit (default 1.0)
//   ka     = 0.15;  // optional Phong ambient  (default 0.15)
//   kd     = 0.80;  // optional Phong diffuse  (default 0.80)
//   ks     = 0.10;  // optional specular scale (default 0.10)
//   shininess = 16.0;  // optional (default 16.0)
//
// Ticket: T4.6.

#include <cmath>

#include <libconfig.h++>

#include "interfaces/IMaterial.hpp"
#include "interfaces/plugin_abi.hpp"
#include "math/Color.hpp"

namespace {

float readFloat(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<float>(static_cast<int>(s));
    return static_cast<float>(static_cast<double>(s));
}

rt::Color readColor(const libconfig::Setting& cfg, const char* key)
{
    const libconfig::Setting& c = cfg[key];
    return rt::Color{readFloat(c["r"]), readFloat(c["g"]), readFloat(c["b"])};
}

// ----------------------------------------------------------------
// CheckerboardMaterial
// ----------------------------------------------------------------

class CheckerboardMaterial final : public rt::IMaterial {
public:
    CheckerboardMaterial(rt::Color c1, rt::Color c2, float scale,
        float ka, float kd, float ks, float shininess)
        : color1_(c1), color2_(c2), scale_(scale)
        , ka_(ka), kd_(kd), shininess_(shininess)
    {
        // Pre-multiply specular colour by ks (same convention as PhongMaterial).
        specCol_ = rt::Color{ks, ks, ks};
    }

    rt::Color shade(const rt::ShadingContext& ctx) const override
    {
        // 3D checkerboard: sum of floor(coord * scale) parities.
        //
        // Bias each coordinate by a small epsilon before floor() to avoid
        // parity flips from floating-point noise near tile boundaries.
        // Ray-plane intersection gives hit.point.y ≈ 0 for a floor at y=0,
        // but rounding can make it slightly negative, causing floor() to
        // return -1 instead of 0 — inverting the color of individual pixels
        // (salt-and-pepper artefact).  A bias of 1e-4 in scaled space is
        // imperceptibly small (~1.6e-4 world units at scale=0.6) yet large
        // enough to absorb any FP residual from the intersection arithmetic.
        constexpr double kBias = 1e-4;
        const double x = ctx.hit.point.x * static_cast<double>(scale_) + kBias;
        const double y = ctx.hit.point.y * static_cast<double>(scale_) + kBias;
        const double z = ctx.hit.point.z * static_cast<double>(scale_) + kBias;

        const int ix = static_cast<int>(std::floor(x));
        const int iy = static_cast<int>(std::floor(y));
        const int iz = static_cast<int>(std::floor(z));

        // Even parity → color1, odd → color2.
        return ((ix + iy + iz) & 1) == 0 ? color1_ : color2_;
    }

    float     ka()           const override { return ka_;       }
    float     kd()           const override { return kd_;       }
    float     shininess()    const override { return shininess_; }
    rt::Color specularColor() const override { return specCol_; }

private:
    rt::Color color1_;
    rt::Color color2_;
    rt::Color specCol_;
    float     scale_;
    float     ka_;
    float     kd_;
    float     shininess_;
};

// ----------------------------------------------------------------
// Plugin factory
// ----------------------------------------------------------------

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Color c1 = readColor(cfg, "color1");
        const rt::Color c2 = readColor(cfg, "color2");

        const float scale = cfg.exists("scale")
            ? readFloat(cfg["scale"]) : 1.0f;
        const float ka = cfg.exists("ka")
            ? readFloat(cfg["ka"]) : 0.15f;
        const float kd = cfg.exists("kd")
            ? readFloat(cfg["kd"]) : 0.80f;
        const float ks = cfg.exists("ks")
            ? readFloat(cfg["ks"]) : 0.10f;
        const float shininess = cfg.exists("shininess")
            ? readFloat(cfg["shininess"]) : 16.0f;

        return new CheckerboardMaterial(c1, c2, scale, ka, kd, ks, shininess);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<CheckerboardMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "checkerboard", &create, &destroy)
