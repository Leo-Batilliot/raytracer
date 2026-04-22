# plugins/ambient/CMakeLists.txt — ambient light plugin (T1.3)
# Produces: ./plugins/raytracer_ambient.so

add_library(raytracer_ambient SHARED AmbientPlugin.cpp)

target_link_libraries(raytracer_ambient PRIVATE rt_interfaces config++)

set_target_properties(raytracer_ambient PROPERTIES PREFIX "")
