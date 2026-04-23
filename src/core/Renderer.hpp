// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Renderer: tile-based rendering pipeline (T0.14).
//
// renderAll() partitions the image into kTileSize×kTileSize tiles and
// calls renderTile() for each. This layout is ready for parallelism
// (T3.2): each tile is an independent unit of work with no shared writes.
//
// Shading policy (grows across phases):
//   T0.14 : sky gradient only (no geometry).
//   T0.15+: scene_.intersect() drives material shade or sky fallback.

#ifndef RAYTRACER_CORE_RENDERER_HPP
#define RAYTRACER_CORE_RENDERER_HPP

#include "core/Camera.hpp"
#include "core/Image.hpp"
#include "core/Scene.hpp"
#include "math/Color.hpp"

namespace rt {

class Renderer {
public:
    Renderer(const Scene& scene, const Camera& camera);

    // Render pixels in [x0,x1) × [y0,y1) into img.
    void renderTile(int x0, int y0, int x1, int y1, Image& img) const;

    // Render every tile in img (row-major tile order).
    void renderAll(Image& img) const;

private:
    Color shade(const Ray& ray, int depth = 0) const;

    const Scene&  scene_;
    const Camera& camera_;

    static constexpr int kTileSize   = 64;
    static constexpr int kMaxBounces = 8;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_RENDERER_HPP
