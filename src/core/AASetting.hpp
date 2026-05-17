// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// AASetting: antialiasing configuration passed to the Renderer (T4.9).
//
// Two modes:
//   Uniform supersampling  (T4.9 basic,    0.5 pt) : samples² rays per pixel
//   Adaptive supersampling (T4.9 adaptive, 1.0 pt) : starts at 2×2, upgrades
//     to samples×samples only for pixels whose 2×2 color variance exceeds
//     `threshold` — saves computation in flat areas.
//
// Configured via the optional `antialiasing {}` block in the scene file.

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
