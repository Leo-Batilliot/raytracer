// Config block:
//   type         = "glass";
//   transparency = 0.98;              // 0 = opaque, 1 = fully transparent
//   ior          = 1.5;               // index of refraction (glass ≈ 1.5, diamond ≈ 2.42)
//   color        = { r g b };         // tint applied to transmitted rays (white = clear)
//   // Optional surface highlight:
//   ka           = 0.02;
//   kd           = 0.05;
//   ks           = 0.3;
//   shininess    = 64.0;

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

class GlassMaterial final : public rt::IMaterial {
public:
    GlassMaterial(rt::Color tint, rt::Color specCol,
                  float ka, float kd, float shininess,
                  float transparency, float ior)
        : tint_(tint), specCol_(specCol)
        , ka_(ka), kd_(kd), shininess_(shininess)
        , transparency_(transparency), ior_(ior)
    {
    }

    // shade() returns the tint — renderer multiplies it onto refracted rays.
    rt::Color shade(const rt::ShadingContext& /*ctx*/) const override
    {
        return tint_;
    }

    float ka()            const override { return ka_;           }
    float kd()            const override { return kd_;           }
    float shininess()     const override { return shininess_;    }
    rt::Color specularColor() const override { return specCol_;  }
    float transparency()  const override { return transparency_; }
    float ior()           const override { return ior_;          }

private:
    rt::Color tint_;
    rt::Color specCol_;
    float     ka_;
    float     kd_;
    float     shininess_;
    float     transparency_;
    float     ior_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const float transparency = cfg.exists("transparency")
            ? readFloat(cfg["transparency"]) : 0.98f;
        const float ior = cfg.exists("ior")
            ? readFloat(cfg["ior"]) : 1.5f;

        rt::Color tint{1.0f, 1.0f, 1.0f};
        if (cfg.exists("color")) {
            tint = rt::Color{
                readFloat(cfg["color"]["r"]),
                readFloat(cfg["color"]["g"]),
                readFloat(cfg["color"]["b"]),
            };
        }

        const float ks = cfg.exists("ks") ? readFloat(cfg["ks"]) : 0.3f;
        rt::Color specCol{ks, ks, ks};
        if (cfg.exists("specular")) {
            specCol = rt::Color{
                readFloat(cfg["specular"]["r"]) * ks,
                readFloat(cfg["specular"]["g"]) * ks,
                readFloat(cfg["specular"]["b"]) * ks,
            };
        }

        const float ka = cfg.exists("ka") ? readFloat(cfg["ka"]) : 0.02f;
        const float kd = cfg.exists("kd") ? readFloat(cfg["kd"]) : 0.05f;
        const float sh = cfg.exists("shininess") ? readFloat(cfg["shininess"]) : 64.0f;

        return new GlassMaterial(tint, specCol, ka, kd, sh, transparency, ior);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<GlassMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "glass", &create, &destroy)
