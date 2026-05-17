# plugins/checkerboard/CMakeLists.txt — Checkerboard procedural material (T4.6)
# Produces: ./plugins/raytracer_checkerboard.so

add_library(raytracer_checkerboard SHARED CheckerboardPlugin.cpp)

target_link_libraries(raytracer_checkerboard PRIVATE rt_interfaces config++)

set_target_properties(raytracer_checkerboard PROPERTIES PREFIX "")
