// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
// PluginLoader implementation — the ONLY file in the project
// that is allowed to call dlopen / dlsym / dlclose directly.

#include "plugins/PluginLoader.hpp"

#include <cstring>   // memcpy (POSIX function-pointer cast workaround)
#include <iostream>

#include <dlfcn.h>

namespace rt {

// ----------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------

namespace {

// POSIX specifies dlsym() returning void*, but ISO C++ forbids
// casting a data pointer to a function pointer via reinterpret_cast
// (which -Wpedantic/-Werror flags). memcpy avoids the UB while
// remaining valid on every POSIX platform where sizeof(void*) ==
// sizeof(void(*)()).
template <typename FnPtr>
FnPtr sym_to_fn(void* sym) noexcept
{
    static_assert(sizeof(FnPtr) == sizeof(void*),
                  "function pointer size != data pointer size on this platform");
    FnPtr fn;
    std::memcpy(&fn, &sym, sizeof(fn));
    return fn;
}

const char* kindName(PluginKind k) noexcept
{
    switch (k) {
        case PluginKind::Primitive: return "Primitive";
        case PluginKind::Light:     return "Light";
        case PluginKind::Material:  return "Material";
    }
    return "Unknown";
}

}  // anonymous namespace

// ----------------------------------------------------------------
// Constructor / destructor / move
// ----------------------------------------------------------------

PluginLoader::PluginLoader(const std::filesystem::path& pluginDir)
{
    namespace fs = std::filesystem;

    std::error_code ec;
    if (!fs::is_directory(pluginDir, ec)) {
        // Not an error — directory may simply not exist yet (Phase 0).
        return;
    }

    for (const auto& entry : fs::directory_iterator(pluginDir, ec)) {
        if (ec) break;
        const auto& p = entry.path();
        if (p.extension() == ".so") {
            loadOne(p);
        }
    }
}

PluginLoader::~PluginLoader()
{
    // Close in reverse-load order — mirrors typical LIFO unload expectations.
    for (auto it = handles_.rbegin(); it != handles_.rend(); ++it) {
        if (*it) ::dlclose(*it);
    }
}

PluginLoader::PluginLoader(PluginLoader&& o) noexcept
    : handles_(std::move(o.handles_))
    , registry_(std::move(o.registry_))
{
    // Nullify source handles so its destructor doesn't dlclose them.
    for (auto& h : o.handles_) h = nullptr;
}

PluginLoader& PluginLoader::operator=(PluginLoader&& o) noexcept
{
    if (this != &o) {
        // Close our current handles first.
        for (auto it = handles_.rbegin(); it != handles_.rend(); ++it)
            if (*it) ::dlclose(*it);

        handles_  = std::move(o.handles_);
        registry_ = std::move(o.registry_);

        for (auto& h : o.handles_) h = nullptr;
    }
    return *this;
}

// ----------------------------------------------------------------
// find
// ----------------------------------------------------------------

const PluginDescriptor* PluginLoader::find(PluginKind kind,
                                           std::string_view name) const
{
    auto key = std::make_pair(kind, std::string(name));
    auto it  = registry_.find(key);
    return it != registry_.end() ? it->second : nullptr;
}

// ----------------------------------------------------------------
// loadOne — the core of the loading logic
// ----------------------------------------------------------------

void PluginLoader::loadOne(const std::filesystem::path& soPath)
{
    const std::string soName = soPath.filename().string();

    // ── 1. dlopen ───────────────────────────────────────────────
    ::dlerror();  // clear stale errors
    void* handle = ::dlopen(soPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        std::cerr << "raytracer: warning: " << soName
                  << ": dlopen failed: " << ::dlerror() << '\n';
        return;
    }

    // ── 2. Resolve entry-point symbol ───────────────────────────
    ::dlerror();
    void* sym = ::dlsym(handle, "raytracer_plugin_entry");
    const char* err = ::dlerror();
    if (err) {
        std::cerr << "raytracer: warning: " << soName
                  << ": no raytracer_plugin_entry symbol: " << err << '\n';
        ::dlclose(handle);
        return;
    }

    using EntryFn = const PluginDescriptor*(*)();
    EntryFn entry = sym_to_fn<EntryFn>(sym);

    // ── 3. Call entry point ──────────────────────────────────────
    const PluginDescriptor* desc = entry();
    if (!desc) {
        std::cerr << "raytracer: warning: " << soName
                  << ": raytracer_plugin_entry() returned null\n";
        ::dlclose(handle);
        return;
    }

    // ── 4. Validate descriptor fields ───────────────────────────
    if (!desc->name || desc->name[0] == '\0') {
        std::cerr << "raytracer: warning: " << soName
                  << ": descriptor has empty name — skipping\n";
        ::dlclose(handle);
        return;
    }
    if (!desc->create || !desc->destroy) {
        std::cerr << "raytracer: warning: " << soName
                  << ": descriptor has null create/destroy — skipping\n";
        ::dlclose(handle);
        return;
    }

    // ── 5. ABI version check ─────────────────────────────────────
    if (desc->abi_version != RAYTRACER_ABI_VERSION) {
        std::cerr << "raytracer: warning: " << soName
                  << ": ABI version mismatch (core expects "
                  << RAYTRACER_ABI_VERSION
                  << ", plugin reports " << desc->abi_version
                  << ") — skipping\n";
        ::dlclose(handle);
        return;
    }

    // ── 6. Duplicate check ───────────────────────────────────────
    auto key = std::make_pair(desc->kind, std::string(desc->name));
    if (registry_.count(key)) {
        std::cerr << "raytracer: warning: " << soName
                  << ": duplicate " << kindName(desc->kind)
                  << " plugin \"" << desc->name
                  << "\" — first registration wins, skipping\n";
        ::dlclose(handle);
        return;
    }

    // ── 7. Register ──────────────────────────────────────────────
    handles_.push_back(handle);
    registry_[key] = desc;
}

}  // namespace rt
