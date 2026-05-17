// Config block:
//   type         = "mirror";
//   reflectivity = 0.9;               // 0 = matte, 1 = perfect mirror
//   color        = { r g b };         // surface tint visible at grazing angles
//   // Optional Phong highlight:
//   ks           = 0.5;
//   shininess    = 128.0;
//   ka           = 0.05;
//   kd           = 0.5;

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

class MirrorMaterial final : public rt::IMaterial {
public:
    MirrorMaterial(rt::Color albedo, rt::Color specCol,
                   float ka, float kd, float shininess,
                   float reflectivity)
        : albedo_(albedo), specCol_(specCol)
        , ka_(ka), kd_(kd), shininess_(shininess)
        , reflectivity_(reflectivity)
    {
    }

    rt::Color shade(const rt::ShadingContext& /*ctx*/) const override
    {
        return albedo_;
    }

    float ka()            const override { return ka_;           }
    float kd()            const override { return kd_;           }
    float shininess()     const override { return shininess_;    }
    rt::Color specularColor() const override { return specCol_;  }
    float reflectivity()  const override { return reflectivity_; }

private:
    rt::Color albedo_;
    rt::Color specCol_;
    float     ka_;
    float     kd_;
    float     shininess_;
    float     reflectivity_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const float reflectivity = cfg.exists("reflectivity")
            ? readFloat(cfg["reflectivity"]) : 0.9f;

        rt::Color albedo{0.9f, 0.9f, 0.95f};
        if (cfg.exists("color")) {
            albedo = rt::Color{
                readFloat(cfg["color"]["r"]),
                readFloat(cfg["color"]["g"]),
                readFloat(cfg["color"]["b"]),
            };
        }

        const float ks = cfg.exists("ks") ? readFloat(cfg["ks"]) : 0.8f;
        rt::Color specCol{ks, ks, ks};
        if (cfg.exists("specular")) {
            specCol = rt::Color{
                readFloat(cfg["specular"]["r"]) * ks,
                readFloat(cfg["specular"]["g"]) * ks,
                readFloat(cfg["specular"]["b"]) * ks,
            };
        }

        const float ka = cfg.exists("ka") ? readFloat(cfg["ka"]) : 0.05f;
        const float kd = cfg.exists("kd") ? readFloat(cfg["kd"]) : 0.3f;
        const float sh = cfg.exists("shininess") ? readFloat(cfg["shininess"]) : 128.0f;

        return new MirrorMaterial(albedo, specCol, ka, kd, sh, reflectivity);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<MirrorMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "mirror", &create, &destroy)
