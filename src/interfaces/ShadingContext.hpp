// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// ShadingContext: bundle of immutable references handed to
// `IMaterial::shade()` so a material can compute its outgoing colour.
//
// Holds CONST references — a ShadingContext is ALWAYS built fresh on
// the stack at the shade call site, never stored. That's why no
// default constructor is provided and no copy / assignment is needed.
//
// `depth` counts how many recursive bounces have occurred (0 for a
// primary ray, 1 for the first reflection / refraction, …). Materials
// use it to stop recursion (e.g. don't cast reflection rays past a
// max depth).
//
// `Scene` is forward-declared because its full definition lands in
// T0.11 (SceneBuilder builds an immutable Scene). Only a reference is
// needed here, so the forward decl keeps this header self-contained.
//
// Ticket: T0.6.

#ifndef RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP
#define RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP

#include "interfaces/HitRecord.hpp"
#include "interfaces/Ray.hpp"

namespace rt {

class Scene;  // see T0.11

struct ShadingContext {
    const Scene& scene;
    const HitRecord& hit;
    const Ray& incomingRay;
    int depth = 0;
};

}  // namespace rt

#endif  // RAYTRACER_INTERFACES_SHADINGCONTEXT_HPP
