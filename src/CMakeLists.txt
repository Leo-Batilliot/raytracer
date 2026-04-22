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
    output/PPMWriter.cpp
    plugins/PluginLoader.cpp
    parser/SceneParser.cpp
    core/Scene.cpp
    core/SceneBuilder.cpp
)

target_link_libraries(raytracer
    PRIVATE
        rt_interfaces
        dl          # dlopen / dlsym / dlclose (T0.9)
        config++    # libconfig++ scene parsing (T0.10)
)

# The executable must NOT be linked against any plugin .so.
