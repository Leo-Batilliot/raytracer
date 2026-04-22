# plugins/plane/CMakeLists.txt — plane primitive plugin (T1.2)
# Produces: ./plugins/raytracer_plane.so

add_library(raytracer_plane SHARED PlanePlugin.cpp)

target_link_libraries(raytracer_plane PRIVATE rt_interfaces config++)

set_target_properties(raytracer_plane PROPERTIES PREFIX "")
