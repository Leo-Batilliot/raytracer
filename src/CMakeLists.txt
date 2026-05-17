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
    core/Camera.cpp
    core/Renderer.cpp
    core/Scene.cpp
    core/SceneBuilder.cpp
)

target_link_libraries(raytracer
    PRIVATE
        rt_interfaces
        dl
        config++
)

# Optional SFML live preview (T4.18)
# Optional SFML live preview (T4.18) — detected via pkg-config
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SFML sfml-graphics QUIET)
endif()
if(SFML_FOUND)
    message(STATUS "SFML ${SFML_VERSION} found — building with live display (static)")
    target_sources(raytracer PRIVATE output/SFMLDisplay.cpp)
    target_include_directories(raytracer PRIVATE ${SFML_INCLUDE_DIRS})
    # Link against SFML 3 static libraries (shared .so are still SFML 2.5)
    target_link_libraries(raytracer PRIVATE
        /usr/local/lib/libsfml-graphics-s.a
        /usr/local/lib/libsfml-window-s.a
        /usr/local/lib/libsfml-system-s.a
        freetype GL GLU X11 Xrandr udev pthread Xi Xcursor
    )
    target_compile_definitions(raytracer PRIVATE RAYTRACER_SFML)
    target_compile_options(raytracer PRIVATE ${SFML_CFLAGS_OTHER})
else()
    message(STATUS "SFML not found — display disabled (PPM stdout only)")
endif()
