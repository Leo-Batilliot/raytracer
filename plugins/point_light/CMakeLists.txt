# plugins/point_light/CMakeLists.txt — Point light plugin (T4.10)
# Produces: ./plugins/raytracer_point_light.so

add_library(raytracer_point_light SHARED PointLightPlugin.cpp)

target_link_libraries(raytracer_point_light PRIVATE rt_interfaces config++)

set_target_properties(raytracer_point_light PROPERTIES PREFIX "")
