# =====================================================================
# src/CMakeLists.txt
# Builds the raytracer executable (placed at the repository root).
#
# Core modules (math, interfaces, core, parser, output, plugin loader)
# are added as INTERFACE or STATIC libraries in their own directories.
# =====================================================================

# Public header-only API shared by core + all plugins (T0.7).
add_subdirectory(interfaces)

add_executable(raytracer
    main.cpp
    math/Matrix4.cpp
    math/Transform.cpp
)

target_link_libraries(raytracer
    PRIVATE
        rt_interfaces
)

# The executable must NOT be linked against any plugin.
# It will eventually link to:
#   - libdl          (for dlopen/dlsym, via a PluginLoader wrapper)
#   - libconfig++    (for scene parsing)
# Both will be added when their respective modules land.
