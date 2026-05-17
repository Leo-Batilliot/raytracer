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
    // Scans `pluginDir` for *.so files and loads valid plugins.
    explicit PluginLoader(const std::filesystem::path& pluginDir = "./plugins");

    ~PluginLoader();

    PluginLoader(const PluginLoader&)            = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;

    PluginLoader(PluginLoader&&) noexcept;
    PluginLoader& operator=(PluginLoader&&) noexcept;

    const PluginDescriptor* find(PluginKind kind, std::string_view name) const;

    std::size_t count() const noexcept { return registry_.size(); }

private:
    void loadOne(const std::filesystem::path& soPath);

    std::vector<void*> handles_;
    std::map<std::pair<PluginKind, std::string>, const PluginDescriptor*> registry_;
};

}  // namespace rt

#endif  // RAYTRACER_PLUGINS_PLUGINLOADER_HPP
