# plugins/cone/CMakeLists.txt — finite cone plugin (T2.2)
# Produces: ./plugins/raytracer_cone.so

add_library(raytracer_cone SHARED ConePlugin.cpp)

target_link_libraries(raytracer_cone PRIVATE rt_interfaces config++)

set_target_properties(raytracer_cone PROPERTIES PREFIX "")
