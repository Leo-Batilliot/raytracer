// Config block:
//   type   = "checkerboard";
//   color1 = { r = 1.0; g = 1.0; b = 1.0; };  // light squares
//   color2 = { r = 0.1; g = 0.1; b = 0.1; };  // dark squares
//   scale  = 1.0;   // tiles per world unit (default 1.0)
//   ka     = 0.15;  // optional Phong ambient  (default 0.15)
//   kd     = 0.80;  // optional Phong diffuse  (default 0.80)
//   ks     = 0.10;  // optional specular scale (default 0.10)
//   shininess = 16.0;  // optional (default 16.0)

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

class CheckerboardMaterial final : public rt::IMaterial {
public:
    CheckerboardMaterial(rt::Color c1, rt::Color c2, float scale,
        float ka, float kd, float ks, float shininess)
        : color1_(c1), color2_(c2), specCol_(rt::Color{ks, ks, ks})
        , scale_(scale), ka_(ka), kd_(kd), shininess_(shininess)
    {
    }

    rt::Color shade(const rt::ShadingContext& ctx) const override
    {
        // Small bias to avoid parity flips from floating-point noise near tile edges.
        constexpr double kBias = 1e-4;
        const double x = ctx.hit.point.x * static_cast<double>(scale_) + kBias;
        const double y = ctx.hit.point.y * static_cast<double>(scale_) + kBias;
        const double z = ctx.hit.point.z * static_cast<double>(scale_) + kBias;

        const int ix = static_cast<int>(std::floor(x));
        const int iy = static_cast<int>(std::floor(y));
        const int iz = static_cast<int>(std::floor(z));

        return ((ix + iy + iz) & 1) == 0 ? color1_ : color2_;
    }

    float     ka()            const override { return ka_;        }
    float     kd()            const override { return kd_;        }
    float     shininess()     const override { return shininess_;  }
    rt::Color specularColor() const override { return specCol_;   }

private:
    rt::Color color1_;
    rt::Color color2_;
    rt::Color specCol_;
    float     scale_;
    float     ka_;
    float     kd_;
    float     shininess_;
};

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
