# plugins/perlin/CMakeLists.txt — Perlin noise procedural material (T4.7)
# Produces: ./plugins/raytracer_perlin.so

add_library(raytracer_perlin SHARED PerlinPlugin.cpp)

target_link_libraries(raytracer_perlin PRIVATE rt_interfaces config++)

set_target_properties(raytracer_perlin PROPERTIES PREFIX "")
