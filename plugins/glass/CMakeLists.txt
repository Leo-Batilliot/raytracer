# plugins/glass/CMakeLists.txt — Glass/dielectric material plugin (T4.3)
# Produces: ./plugins/raytracer_glass.so

add_library(raytracer_glass SHARED GlassPlugin.cpp)

target_link_libraries(raytracer_glass PRIVATE rt_interfaces config++)

set_target_properties(raytracer_glass PROPERTIES PREFIX "")
