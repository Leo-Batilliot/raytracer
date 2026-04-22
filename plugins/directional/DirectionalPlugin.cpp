// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Directional light plugin (T1.4).
// Models an infinitely distant light source (sun): constant direction and
// radiance everywhere; no distance attenuation; shadow rays run to +inf.
//
// Config `direction` is the vector the light is shining TOWARD (i.e. the
// direction photons travel).  sample() returns the negated direction so
// the Lambert dot-product in the renderer gets a vector pointing FROM the
// surface TOWARD the light (per ILight contract).

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
// DirectionalLight
// ----------------------------------------------------------------

class DirectionalLight final : public rt::ILight {
public:
    // toLight: unit vector pointing FROM scene TOWARD the light source.
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

// ----------------------------------------------------------------
// Plugin factory
// ----------------------------------------------------------------

void* create(const libconfig::Setting& cfg)
{
    try {
        // `direction` in config = direction light shines toward scene.
        // We negate it to get the vector from surface toward light.
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
