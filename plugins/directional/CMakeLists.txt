# plugins/directional/CMakeLists.txt — directional light plugin (T1.4)
# Produces: ./plugins/raytracer_directional.so

add_library(raytracer_directional SHARED DirectionalPlugin.cpp)

target_link_libraries(raytracer_directional PRIVATE rt_interfaces config++)

set_target_properties(raytracer_directional PROPERTIES PREFIX "")
