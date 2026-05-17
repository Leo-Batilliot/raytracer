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

class AmbientLight final : public rt::ILight {
public:
    explicit AmbientLight(rt::Color contribution)
        : contribution_(contribution)
    {
    }

    rt::LightSample sample(const rt::Vector3& /*surfacePoint*/) const override
    {
        return rt::LightSample{
            rt::Vector3{0.0, 1.0, 0.0},
            contribution_,
            std::numeric_limits<double>::infinity(),
        };
    }

    bool  isAmbient()               const override { return true; }
    rt::Color ambientContribution() const override { return contribution_; }

private:
    rt::Color contribution_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Color color{
            readFloat(cfg["color"]["r"]),
            readFloat(cfg["color"]["g"]),
            readFloat(cfg["color"]["b"]),
        };
        const float intensity = readFloat(cfg["intensity"]);
        return new AmbientLight(color * intensity);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<AmbientLight*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Light, "ambient", &create, &destroy)
