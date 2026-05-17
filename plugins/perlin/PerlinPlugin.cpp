// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Perlin noise procedural material plugin (T4.7).
// Implements Ken Perlin's "Improved Noise" (SIGGRAPH 2002) in 3D,
// evaluated at the world-space hit position.
//
// Three modes (selected via `mode` in the config):
//   "noise"       — raw noise in [0,1], smooth organic pattern.
//   "turbulence"  — sum of |noise| over octaves, rocky/cloudy look (default).
//   "marble"      — sine wave warped by turbulence, classic marble veining.
//
// In all modes, the scalar result linearly interpolates between color1
// and color2, and Phong shading parameters apply normally.
//
// Config block:
//   type    = "perlin";
//   color1  = { r = 0.9; g = 0.85; b = 0.75; };  // "light" tone
//   color2  = { r = 0.3; g = 0.25; b = 0.20; };  // "dark" tone
//   scale   = 1.0;          // spatial frequency in world units (default 1.0)
//   octaves = 4;            // FBM octaves 1..8 (default 4)
//   mode    = "turbulence"; // "noise" | "turbulence" | "marble"
//   ka      = 0.12;         // ambient  (default 0.12)
//   kd      = 0.75;         // diffuse  (default 0.75)
//   ks      = 0.20;         // specular (default 0.20)
//   shininess = 32.0;       // Phong exponent (default 32)
//
// Ticket: T4.7.

#include <cmath>
#include <cstring>

#include <libconfig.h++>

#include "interfaces/IMaterial.hpp"
#include "interfaces/plugin_abi.hpp"
#include "math/Color.hpp"

namespace {

// ----------------------------------------------------------------
// Perlin noise — Ken Perlin "Improving Noise" (2002)
// ----------------------------------------------------------------

// Original permutation table (256 entries, used doubled to avoid masking).
static const int kPerm[256] = {
    151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
    140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
    247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
     57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
     74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
     60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
     65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
     52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
     81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

// Improved fade: 6t^5 - 15t^4 + 10t^3  (zero first and second derivative at 0 and 1)
static double fade(double t) noexcept
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

static double lerp(double a, double b, double t) noexcept
{
    return a + t * (b - a);
}

// Gradient: maps hash to one of 12 unit vectors on edges of a cube.
static double grad(int hash, double x, double y, double z) noexcept
{
    const int    h = hash & 15;
    const double u = h < 8 ? x : y;
    const double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

// Integer floor (handles negative values correctly unlike int cast).
static int ifloor(double x) noexcept
{
    const int i = static_cast<int>(x);
    return (x < static_cast<double>(i)) ? i - 1 : i;
}

// 3D noise — returns value in [-1, 1].
static double noise3(double x, double y, double z) noexcept
{
    // Unit-cube cell containing the point.
    const int X = ifloor(x) & 255;
    const int Y = ifloor(y) & 255;
    const int Z = ifloor(z) & 255;

    // Fractional position inside the cube.
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    const double u = fade(x);
    const double v = fade(y);
    const double w = fade(z);

    // Hash the 8 corners via the permutation table (& 255 replaces the doubled table).
    auto p = [](int i) noexcept { return kPerm[i & 255]; };

    const int A  = p(X)   + Y;
    const int AA = p(A)   + Z;
    const int AB = p(A+1) + Z;
    const int B  = p(X+1) + Y;
    const int BA = p(B)   + Z;
    const int BB = p(B+1) + Z;

    const double n000 = grad(p(AA),   x,   y,   z);
    const double n100 = grad(p(BA),   x-1, y,   z);
    const double n010 = grad(p(AB),   x,   y-1, z);
    const double n110 = grad(p(BB),   x-1, y-1, z);
    const double n001 = grad(p(AA+1), x,   y,   z-1);
    const double n101 = grad(p(BA+1), x-1, y,   z-1);
    const double n011 = grad(p(AB+1), x,   y-1, z-1);
    const double n111 = grad(p(BB+1), x-1, y-1, z-1);

    return lerp(
        lerp(lerp(n000, n100, u), lerp(n010, n110, u), v),
        lerp(lerp(n001, n101, u), lerp(n011, n111, u), v),
        w);
}

// Fractal Brownian Motion — turbulence, returns [0, 1].
// Sums absolute values of noise at doubling frequencies.
static double turbulence(double x, double y, double z, int octaves) noexcept
{
    double total  = 0.0;
    double maxAmp = 0.0;
    double amp    = 1.0;
    for (int i = 0; i < octaves; ++i) {
        total  += std::abs(noise3(x, y, z)) * amp;
        maxAmp += amp;
        amp *= 0.5;
        x *= 2.0; y *= 2.0; z *= 2.0;
    }
    return total / maxAmp;
}

// ----------------------------------------------------------------
// PerlinMaterial
// ----------------------------------------------------------------

enum class Mode { kNoise, kTurbulence, kMarble };

class PerlinMaterial final : public rt::IMaterial {
public:
    PerlinMaterial(rt::Color c1, rt::Color c2,
        float scale, int octaves, Mode mode,
        float ka, float kd, float ks, float shininess)
        : color1_(c1), color2_(c2)
        , specCol_(rt::Color{ks, ks, ks})
        , scale_(static_cast<double>(scale))
        , octaves_(octaves), mode_(mode)
        , ka_(ka), kd_(kd), shininess_(shininess)
    {
    }

    rt::Color shade(const rt::ShadingContext& ctx) const override
    {
        const double x = ctx.hit.point.x * scale_;
        const double y = ctx.hit.point.y * scale_;
        const double z = ctx.hit.point.z * scale_;

        double t = 0.0;
        switch (mode_) {
        case Mode::kNoise:
            // Map [-1,1] → [0,1]
            t = noise3(x, y, z) * 0.5 + 0.5;
            break;
        case Mode::kTurbulence:
            t = turbulence(x, y, z, octaves_);
            break;
        case Mode::kMarble:
            // Classic marble: sine wave along one axis, warped by turbulence.
            t = 0.5 + 0.5 * std::sin(x + 5.0 * turbulence(x, y, z, octaves_));
            break;
        }

        const auto tf = static_cast<float>(t);
        return color1_ * (1.0f - tf) + color2_ * tf;
    }

    float     ka()            const override { return ka_;        }
    float     kd()            const override { return kd_;        }
    float     shininess()     const override { return shininess_;  }
    rt::Color specularColor() const override { return specCol_;    }

private:
    rt::Color color1_;
    rt::Color color2_;
    rt::Color specCol_;
    double    scale_;
    int       octaves_;
    Mode      mode_;
    float     ka_;
    float     kd_;
    float     shininess_;
};

// ----------------------------------------------------------------
// Plugin factory
// ----------------------------------------------------------------

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

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Color c1 = readColor(cfg, "color1");
        const rt::Color c2 = readColor(cfg, "color2");

        const float scale    = cfg.exists("scale")    ? readFloat(cfg["scale"])    : 1.0f;
        const int   octaves  = cfg.exists("octaves")  ? static_cast<int>(cfg["octaves"]) : 4;
        const float ka       = cfg.exists("ka")       ? readFloat(cfg["ka"])       : 0.12f;
        const float kd       = cfg.exists("kd")       ? readFloat(cfg["kd"])       : 0.75f;
        const float ks       = cfg.exists("ks")       ? readFloat(cfg["ks"])       : 0.20f;
        const float shininess = cfg.exists("shininess") ? readFloat(cfg["shininess"]) : 32.0f;

        Mode mode = Mode::kTurbulence;
        if (cfg.exists("mode")) {
            const std::string m = static_cast<const char*>(cfg["mode"]);
            if      (m == "noise")       mode = Mode::kNoise;
            else if (m == "marble")      mode = Mode::kMarble;
            // "turbulence" → default
        }

        const int clampedOctaves = (octaves < 1) ? 1 : (octaves > 8 ? 8 : octaves);

        return new PerlinMaterial(c1, c2, scale, clampedOctaves, mode,
            ka, kd, ks, shininess);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<PerlinMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "perlin", &create, &destroy)
