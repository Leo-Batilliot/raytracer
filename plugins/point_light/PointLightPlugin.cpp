// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Point light plugin (T4.10).
// Models an omnidirectional light source at a fixed position in the scene.
//
// Differences from directional light:
//   - Direction is computed per sample: normalize(position - surfacePoint).
//   - Distance is finite: shadow rays stop at the light position (correct
//     self-shadowing and correct occlusion by objects behind the light).
//   - Optional attenuation: radiance *= 1 / (c + l*d + q*d²).
//     Default (c=1, l=0, q=0) gives uniform intensity (no falloff).
//     Setting q=1 gives physically-based inverse-square falloff (1/d²).
//
// Config block:
//   lights:
//   {
//       point = (
//           { position  = { x = 0.0; y = 5.0; z = 0.0; };
//             color     = { r = 1.0; g = 0.8; b = 0.6; };
//             intensity = 1.0;
//             // Optional attenuation (default = no falloff):
//             // constant  = 1.0;
//             // linear    = 0.0;
//             // quadratic = 0.0;
//           }
//       );
//   };
//
// Ticket: T4.10.

#include <cmath>
#include <limits>

#include <libconfig.h++>

#include "interfaces/ILight.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

float readFloat(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<float>(static_cast<int>(s));
    return static_cast<float>(static_cast<double>(s));
}

double readDouble(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<double>(static_cast<int>(s));
    return static_cast<double>(s);
}

// ----------------------------------------------------------------
// PointLight
// ----------------------------------------------------------------

class PointLight final : public rt::ILight {
public:
    PointLight(rt::Vector3 pos, rt::Color radiance,
        double kConstant, double kLinear, double kQuadratic)
        : pos_(pos), radiance_(radiance)
        , kC_(kConstant), kL_(kLinear), kQ_(kQuadratic)
    {
    }

    rt::LightSample sample(const rt::Vector3& surfacePoint) const override
    {
        const rt::Vector3 delta = pos_ - surfacePoint;
        const double      dist  = delta.length();

        if (dist < 1e-10)
            return {rt::Vector3{0.0, 1.0, 0.0}, rt::Color::black(), 0.0};

        const rt::Vector3 dir = delta / dist;

        // Attenuation: 1 / (c + l*d + q*d²)
        const double attn = 1.0 / (kC_ + kL_ * dist + kQ_ * dist * dist);
        const rt::Color rad = radiance_ * static_cast<float>(attn);

        return {dir, rad, dist};
    }

private:
    rt::Vector3 pos_;
    rt::Color   radiance_;
    double      kC_;
    double      kL_;
    double      kQ_;
};

// ----------------------------------------------------------------
// Plugin factory
// ----------------------------------------------------------------

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Vector3 pos{
            readDouble(cfg["position"]["x"]),
            readDouble(cfg["position"]["y"]),
            readDouble(cfg["position"]["z"]),
        };
        const rt::Color color{
            readFloat(cfg["color"]["r"]),
            readFloat(cfg["color"]["g"]),
            readFloat(cfg["color"]["b"]),
        };
        const float intensity = cfg.exists("intensity")
            ? readFloat(cfg["intensity"]) : 1.0f;

        const double kC = cfg.exists("constant")
            ? readDouble(cfg["constant"]) : 1.0;
        const double kL = cfg.exists("linear")
            ? readDouble(cfg["linear"]) : 0.0;
        const double kQ = cfg.exists("quadratic")
            ? readDouble(cfg["quadratic"]) : 0.0;

        return new PointLight(pos, color * intensity, kC, kL, kQ);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<PointLight*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Light, "point", &create, &destroy)
