# =====================================================================
# plugins/CMakeLists.txt
#
# Each plugin will be its own subdirectory with its own CMakeLists.txt
# producing a SHARED library written to ./plugins/ (see root CMakeLists
# LIBRARY_OUTPUT_DIRECTORY).
#
# Naming convention: raytracer_<name>.so
#
# Plugins will be added progressively:
#   - T0.15 : sphere (dummy factory, end-to-end plugin loading test)
#   - T1.1  : sphere (real intersection)
#   - T1.2  : plane
#   - T1.3  : ambient light
#   - T1.4  : directional light
#   - T1.5  : flat color material
#   - T2.1  : cylinder
#   - T2.2  : cone
#   - T4.x  : phong, reflective, textured, ...
# =====================================================================

add_subdirectory(sphere)      # T1.1
add_subdirectory(plane)       # T1.2
add_subdirectory(ambient)     # T1.3
add_subdirectory(directional) # T1.4
add_subdirectory(flat_color)  # T1.5
add_subdirectory(cylinder)    # T2.1
