#include <libconfig.h++>

#include "interfaces/IMaterial.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

float readFloat(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<float>(static_cast<int>(s));
    return static_cast<float>(static_cast<double>(s));
}

class FlatColor final : public rt::IMaterial {
public:
    explicit FlatColor(rt::Color albedo) : albedo_(albedo) {}

    rt::Color shade(const rt::ShadingContext& /*ctx*/) const override
    {
        return albedo_;
    }

private:
    rt::Color albedo_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Color color{
            readFloat(cfg["color"]["r"]),
            readFloat(cfg["color"]["g"]),
            readFloat(cfg["color"]["b"]),
        };
        return new FlatColor(color);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<FlatColor*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "flat_color", &create, &destroy)
