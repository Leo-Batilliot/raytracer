// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// plugin_abi.hpp — stable C-linkage contract between the core and
// every dynamically-loaded plugin (.so).
//
// RULES FOR PLUGIN AUTHORS
//   1. Include this header (and only this header from the core) in
//      your plugin.
//   2. Define ONE global rt::PluginDescriptor and return a pointer
//      to it from `raytracer_plugin_entry()`.
//   3. NEVER change the ABI_VERSION field — it is set by the macro
//      RAYTRACER_PLUGIN_ENTRY below. If the version mismatch, the
//      core rejects the plugin at load time with a warning.
//   4. `create` must return a heap-allocated object; `destroy` must
//      call the matching `delete`. Never throw from either.
//   5. Exceptions must NOT cross the plugin boundary. Catch everything
//      inside `create` / `destroy` and handle locally or return nullptr.
//
// STABILITY CONTRACT
//   Changing this header is a BREAKING ABI change.  Bump
//   RAYTRACER_ABI_VERSION and update the PluginLoader accordingly.
//
// Ticket: T0.8.

#ifndef RAYTRACER_INTERFACES_PLUGIN_ABI_HPP
#define RAYTRACER_INTERFACES_PLUGIN_ABI_HPP

#include <cstdint>

// Forward-declare libconfig::Setting so this header stays lightweight.
// Files that call create() must #include <libconfig.h++> themselves.
namespace libconfig { class Setting; }

namespace rt {

// ----------------------------------------------------------------
// ABI version — increment on any breaking change to this header
// (new fields in PluginDescriptor, changed PluginKind values, etc.)
// The PluginLoader (T0.9) hard-rejects any plugin with a different
// version rather than crashing with undefined behaviour.
// ----------------------------------------------------------------
inline constexpr uint32_t RAYTRACER_ABI_VERSION = 1;

// ----------------------------------------------------------------
// PluginKind — what a plugin provides.
// Fixed underlying type (uint32_t) for layout stability across
// compiler versions and toolchains.
// ----------------------------------------------------------------
enum class PluginKind : uint32_t {
    Primitive = 0,
    Light     = 1,
    Material  = 2,
};

// ----------------------------------------------------------------
// PluginDescriptor — the sole data structure exported by a plugin.
// Must be trivially copyable (no virtuals, no smart pointers).
// ----------------------------------------------------------------
struct PluginDescriptor {
    uint32_t    abi_version;  // must equal rt::RAYTRACER_ABI_VERSION
    PluginKind  kind;         // what this plugin provides

    // Stable, lowercase ASCII identifier used as the key in the
    // scene config (e.g. "sphere", "directional", "flat_color").
    const char* name;

    // Factory: allocates and returns the concrete implementation.
    // The PluginLoader casts the void* to IPrimitive*/ILight*/
    // IMaterial* based on `kind` after the ABI version check.
    // Return nullptr to signal a creation error (do not throw).
    void* (*create)(const libconfig::Setting& cfg);

    // Destructor: paired with create(). Called when the Scene is
    // torn down.  Must not throw.  Must call the correct delete.
    void (*destroy)(void* obj);
};

}  // namespace rt

// ----------------------------------------------------------------
// Entry-point symbol every plugin must export.
// extern "C" prevents C++ name-mangling so that dlsym("raytracer_
// plugin_entry") finds it regardless of the compiler used.
//
// Convention: return a pointer to a *static* PluginDescriptor
// (static storage duration = valid for the lifetime of the .so).
//
// Usage (in your plugin .cpp):
//
//   extern "C" const rt::PluginDescriptor* raytracer_plugin_entry() {
//       static const rt::PluginDescriptor kDesc = {
//           rt::RAYTRACER_ABI_VERSION,
//           rt::PluginKind::Primitive,
//           "sphere",
//           &SpherePlugin::create,
//           &SpherePlugin::destroy,
//       };
//       return &kDesc;
//   }
//
// ----------------------------------------------------------------
extern "C" const rt::PluginDescriptor* raytracer_plugin_entry();

// ----------------------------------------------------------------
// Convenience macro — fills abi_version automatically so plugin
// authors can't accidentally hardcode the wrong number.
//
//   RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "sphere",
//                          &SpherePlugin::create,
//                          &SpherePlugin::destroy)
// ----------------------------------------------------------------
#define RAYTRACER_PLUGIN_ENTRY(kind_, name_, create_, destroy_)             \
    extern "C" const rt::PluginDescriptor* raytracer_plugin_entry() {      \
        static const rt::PluginDescriptor kDesc = {                         \
            rt::RAYTRACER_ABI_VERSION, (kind_), (name_),                    \
            (create_), (destroy_),                                          \
        };                                                                  \
        return &kDesc;                                                      \
    }

#endif  // RAYTRACER_INTERFACES_PLUGIN_ABI_HPP
