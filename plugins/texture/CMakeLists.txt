# plugins/texture/CMakeLists.txt — File texture material (T4.8)
# Produces: ./plugins/raytracer_texture.so

add_library(raytracer_texture SHARED TexturePlugin.cpp)

target_link_libraries(raytracer_texture PRIVATE rt_interfaces config++)

set_target_properties(raytracer_texture PROPERTIES PREFIX "")
