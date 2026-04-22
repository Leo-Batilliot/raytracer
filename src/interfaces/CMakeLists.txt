# =====================================================================
# src/interfaces/CMakeLists.txt
#
# INTERFACE library: rt_interfaces
#
# Header-only public API shared by the core executable AND every
# plugin .so. No source files to compile. Consumers (raytracer,
# raytracer_sphere, …) link against this target and automatically
# get:
#   - the correct include path (src/ root, so
#     #include "interfaces/IPrimitive.hpp" resolves correctly)
#   - any future compile definitions we add here
#
# Usage:
#   target_link_libraries(my_target PRIVATE rt_interfaces)
# =====================================================================

add_library(rt_interfaces INTERFACE)

# Expose src/ as the include root so both the executable and
# plugins can use:   #include "interfaces/IPrimitive.hpp"
#                    #include "math/Vector3.hpp"
# etc. with the same path regardless of where they live in the tree.
target_include_directories(rt_interfaces INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
