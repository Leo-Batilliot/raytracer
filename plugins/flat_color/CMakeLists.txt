# plugins/flat_color/CMakeLists.txt — flat colour material plugin (T1.5)
# Produces: ./plugins/raytracer_flat_color.so

add_library(raytracer_flat_color SHARED FlatColorPlugin.cpp)

target_link_libraries(raytracer_flat_color PRIVATE rt_interfaces config++)

set_target_properties(raytracer_flat_color PROPERTIES PREFIX "")
