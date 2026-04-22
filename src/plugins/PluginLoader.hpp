// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// PluginLoader: sole owner of every dl handle in the process.
//
// Responsibilities:
//   - Walk a directory for *.so files at construction time.
//   - dlopen each one, resolve raytracer_plugin_entry, validate
//     the PluginDescriptor (ABI version, non-null fields).
//   - Index valid descriptors by (PluginKind, name).
//   - dlclose all handles in the destructor — zero leaks.
//
// Errors are NON-FATAL: a bad .so emits a warning to stderr and is
// skipped. Only a truly unrecoverable situation (e.g. OOM) throws.
//
// <dlfcn.h> is NOT included from this header — it stays confined
// to PluginLoader.cpp so consumers don't pull POSIX headers.
//
// Ticket: T0.9.

#ifndef RAYTRACER_PLUGINS_PLUGINLOADER_HPP
#define RAYTRACER_PLUGINS_PLUGINLOADER_HPP

#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "interfaces/plugin_abi.hpp"

namespace rt {

class PluginLoader {
public:
    // Scan `pluginDir` for *.so files and load valid plugins.
    // Missing or empty directory is not an error.
    explicit PluginLoader(const std::filesystem::path& pluginDir = "./plugins");

    // Closes all dl handles. Defined in .cpp (needs dlclose).
    ~PluginLoader();

    // Non-copyable — dl handles are unique resources.
    PluginLoader(const PluginLoader&)            = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;

    // Movable.
    PluginLoader(PluginLoader&&) noexcept;
    PluginLoader& operator=(PluginLoader&&) noexcept;

    // Look up a descriptor. Returns nullptr if no plugin matches.
    const PluginDescriptor* find(PluginKind kind, std::string_view name) const;

    // Number of successfully loaded plugins.
    std::size_t count() const noexcept { return registry_.size(); }

private:
    void loadOne(const std::filesystem::path& soPath);

    // Raw dl handles. Stored separately from the registry so we can
    // dlclose them in order on destruction, even if a descriptor is
    // subsequently removed from the registry (e.g. duplicate).
    std::vector<void*> handles_;

    // Primary lookup table: (kind, lowercase name) → descriptor.
    // The PluginDescriptor* points into the .so's read-only segment
    // (static storage) — valid for the lifetime of the handle.
    std::map<std::pair<PluginKind, std::string>, const PluginDescriptor*> registry_;
};

}  // namespace rt

#endif  // RAYTRACER_PLUGINS_PLUGINLOADER_HPP
