# plugins/cylinder/CMakeLists.txt — finite cylinder plugin (T2.1)
# Produces: ./plugins/raytracer_cylinder.so

add_library(raytracer_cylinder SHARED CylinderPlugin.cpp)

target_link_libraries(raytracer_cylinder PRIVATE rt_interfaces config++)

set_target_properties(raytracer_cylinder PROPERTIES PREFIX "")
