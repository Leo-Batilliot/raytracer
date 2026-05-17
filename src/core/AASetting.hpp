#ifndef RAYTRACER_CORE_AASETTING_HPP
#define RAYTRACER_CORE_AASETTING_HPP

namespace rt {

struct AASetting {
    int   samples   = 1;     // samples per side: 1=off, 2=4spp, 3=9spp, …
    bool  adaptive  = false; // enable adaptive refinement on high-variance pixels
    float threshold = 0.05f; // max channel diff [0,1] before adaptive triggers
};

}  // namespace rt

#endif  // RAYTRACER_CORE_AASETTING_HPP
