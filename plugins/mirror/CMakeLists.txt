# plugins/mirror/CMakeLists.txt — Mirror/reflective material plugin (T4.2)
# Produces: ./plugins/raytracer_mirror.so

add_library(raytracer_mirror SHARED MirrorPlugin.cpp)

target_link_libraries(raytracer_mirror PRIVATE rt_interfaces config++)

set_target_properties(raytracer_mirror PROPERTIES PREFIX "")
