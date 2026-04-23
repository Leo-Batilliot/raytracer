# plugins/phong/CMakeLists.txt — Phong material plugin (T4.1)
# Produces: ./plugins/raytracer_phong.so

add_library(raytracer_phong SHARED PhongPlugin.cpp)

target_link_libraries(raytracer_phong PRIVATE rt_interfaces config++)

set_target_properties(raytracer_phong PROPERTIES PREFIX "")
