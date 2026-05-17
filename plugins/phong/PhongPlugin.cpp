// Config block:
//   type      = "phong";
//   color     = { r = 0.8; g = 0.2; b = 0.2; };  // diffuse albedo
//   ka        = 0.1;   // ambient coefficient
//   kd        = 0.9;   // diffuse coefficient
//   ks        = 0.6;   // specular coefficient (scales specular colour)
//   shininess = 64.0;  // Phong exponent (higher = tighter highlight)
//   // optional — specular colour (defaults to white if omitted):
//   specular  = { r = 1.0; g = 1.0; b = 1.0; };

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

class PhongMaterial final : public rt::IMaterial {
public:
    PhongMaterial(rt::Color albedo, rt::Color specCol,
                  float ka, float kd, float shininess)
        : albedo_(albedo), specCol_(specCol)
        , ka_(ka), kd_(kd), shininess_(shininess)
    {
    }

    rt::Color shade(const rt::ShadingContext& /*ctx*/) const override
    {
        return albedo_;
    }

    float ka()        const override { return ka_;        }
    float kd()        const override { return kd_;        }
    float shininess() const override { return shininess_; }

    rt::Color specularColor() const override { return specCol_; }

private:
    rt::Color albedo_;
    rt::Color specCol_;
    float     ka_;
    float     kd_;
    float     shininess_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Color albedo{
            readFloat(cfg["color"]["r"]),
            readFloat(cfg["color"]["g"]),
            readFloat(cfg["color"]["b"]),
        };

        const float ks = cfg.exists("ks")
            ? readFloat(cfg["ks"]) : 0.5f;

        rt::Color specCol{ks, ks, ks};
        if (cfg.exists("specular")) {
            specCol = rt::Color{
                readFloat(cfg["specular"]["r"]) * ks,
                readFloat(cfg["specular"]["g"]) * ks,
                readFloat(cfg["specular"]["b"]) * ks,
            };
        }

        const float ka = cfg.exists("ka")
            ? readFloat(cfg["ka"]) : 0.1f;
        const float kd = cfg.exists("kd")
            ? readFloat(cfg["kd"]) : 0.9f;

        float shininess = 32.0f;
        if (cfg.exists("shininess"))
            shininess = readFloat(cfg["shininess"]);

        return new PhongMaterial(albedo, specCol, ka, kd, shininess);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<PhongMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "phong", &create, &destroy)
