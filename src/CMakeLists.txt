# =====================================================================
# src/CMakeLists.txt
# Builds the raytracer executable (placed at the repository root).
#
# For now this is a minimal skeleton: main.cpp returns 0.
# Core modules (math, interfaces, core, parser, output, plugin loader)
# will be added as INTERFACE or STATIC libraries in upcoming tickets.
# =====================================================================

add_executable(raytracer
    main.cpp
    math/Matrix4.cpp
    math/Transform.cpp
)

target_include_directories(raytracer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# The executable must NOT be linked against any plugin.
# It will eventually link to:
#   - libdl          (for dlopen/dlsym, via a PluginLoader wrapper)
#   - libconfig++    (for scene parsing)
# Both will be added when their respective modules land.
