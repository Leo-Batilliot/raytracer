#ifndef RAYTRACER_CORE_RENDERER_HPP
#define RAYTRACER_CORE_RENDERER_HPP

#include <atomic>
#include "core/AASetting.hpp"
#include "core/Camera.hpp"
#include "core/Image.hpp"
#include "core/Scene.hpp"
#include "math/Color.hpp"

namespace rt {

class Renderer {
public:
    Renderer(const Scene& scene, const Camera& camera,
        AASetting aa = {});

    // Render pixels in [x0,x1) × [y0,y1) into img.
    void renderTile(int x0, int y0, int x1, int y1, Image& img) const;

    // Render every tile in img (row-major tile order).
    // If cancel is set to true by the caller, workers stop after the current tile.
    void renderAll(Image& img, std::atomic<bool>* cancel = nullptr) const;

private:
    Color shade(const Ray& ray, int depth = 0) const;
    Color shadePixel(int px, int py) const;
    Color uniformSample(int px, int py, int n) const;

    const Scene&  scene_;
    const Camera& camera_;
    AASetting     aa_;

    static constexpr int kTileSize   = 64;
    static constexpr int kMaxBounces = 8;
};

}  // namespace rt

#endif  // RAYTRACER_CORE_RENDERER_HPP
