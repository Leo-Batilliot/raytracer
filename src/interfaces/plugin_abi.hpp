#ifndef RAYTRACER_INTERFACES_PLUGIN_ABI_HPP
#define RAYTRACER_INTERFACES_PLUGIN_ABI_HPP

#include <cstdint>

// Forward-declare libconfig::Setting so this header stays lightweight.
namespace libconfig { class Setting; }

namespace rt {

// Increment on any breaking change to this header.
inline constexpr uint32_t RAYTRACER_ABI_VERSION = 1;

enum class PluginKind : uint32_t {
    Primitive = 0,
    Light     = 1,
    Material  = 2,
};

struct PluginDescriptor {
    uint32_t    abi_version;
    PluginKind  kind;

    // Lowercase ASCII identifier used as the key in the scene config.
    const char* name;

    // Factory: allocates and returns the concrete implementation.
    // Return nullptr to signal a creation error (do not throw).
    void* (*create)(const libconfig::Setting& cfg);

    // Must call the correct delete. Must not throw.
    void (*destroy)(void* obj);
};

}  // namespace rt

extern "C" const rt::PluginDescriptor* raytracer_plugin_entry();

// Convenience macro — fills abi_version automatically.
//
//   RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "sphere",
//                          &SpherePlugin::create,
//                          &SpherePlugin::destroy)
#define RAYTRACER_PLUGIN_ENTRY(kind_, name_, create_, destroy_)             \
    extern "C" const rt::PluginDescriptor* raytracer_plugin_entry() {      \
        static const rt::PluginDescriptor kDesc = {                         \
            rt::RAYTRACER_ABI_VERSION, (kind_), (name_),                    \
            (create_), (destroy_),                                          \
        };                                                                  \
        return &kDesc;                                                      \
    }

#endif  // RAYTRACER_INTERFACES_PLUGIN_ABI_HPP
