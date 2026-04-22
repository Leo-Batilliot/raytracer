# plugins/sphere/CMakeLists.txt — dummy sphere plugin (T0.15)
# Produces: ./plugins/raytracer_sphere.so (no "lib" prefix via PREFIX "").

add_library(raytracer_sphere SHARED SpherePlugin.cpp)

target_link_libraries(raytracer_sphere PRIVATE rt_interfaces)

set_target_properties(raytracer_sphere PROPERTIES PREFIX "")
