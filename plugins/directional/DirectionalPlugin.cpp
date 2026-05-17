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

class DirectionalLight final : public rt::ILight {
public:
    // toLight: unit vector from scene toward the light source.
    DirectionalLight(rt::Vector3 toLight, rt::Color radiance)
        : toLight_(toLight), radiance_(radiance)
    {
    }

    rt::LightSample sample(const rt::Vector3& /*surfacePoint*/) const override
    {
        return rt::LightSample{
            toLight_,
            radiance_,
            std::numeric_limits<double>::infinity(),
        };
    }

private:
    rt::Vector3 toLight_;
    rt::Color   radiance_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        // `direction` = direction light shines toward scene; negate for surface→light.
        const rt::Vector3 toward{
            readDouble(cfg["direction"]["x"]),
            readDouble(cfg["direction"]["y"]),
            readDouble(cfg["direction"]["z"]),
        };
        const rt::Vector3 toLight = (-toward).normalized();

        const rt::Color color{
            readFloat(cfg["color"]["r"]),
            readFloat(cfg["color"]["g"]),
            readFloat(cfg["color"]["b"]),
        };
        const float intensity = readFloat(cfg["intensity"]);

        return new DirectionalLight(toLight, color * intensity);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<DirectionalLight*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Light, "directional", &create, &destroy)
