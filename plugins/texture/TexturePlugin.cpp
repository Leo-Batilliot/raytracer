// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// File texture material plugin (T4.8).
// Loads a PPM image (P3 ASCII or P6 binary, any maxval) and samples
// it at the surface's UV coordinates with bilinear interpolation and
// tiling (repeat wrapping). Supports UV scaling for tile density control.
//
// The UV coordinates come from ctx.hit.uv:
//   - Sphere  : u ∈ [0,1] (azimuth), v ∈ [0,1] (polar, 0 = top)
//   - Cylinder: u = azimuth [0,1], v = height [0,1]
//   - Cone    : similar to cylinder
//   - Plane   : u = dot(point, tangent), v = dot(point, bitangent) in world
//               units — use scale_u / scale_v to control tile density.
//
// Config block:
//   type    = "texture";
//   file    = "textures/brick.ppm";  // path relative to cwd
//   scale_u = 1.0;   // UV tiling frequency on u axis (default 1.0)
//   scale_v = 1.0;   // UV tiling frequency on v axis (default 1.0)
//   ka      = 0.12;  // ambient  (default 0.12)
//   kd      = 0.80;  // diffuse  (default 0.80)
//   ks      = 0.10;  // specular (default 0.10)
//   shininess = 16.0;
//
// Ticket: T4.8.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <vector>

#include <libconfig.h++>

#include "interfaces/IMaterial.hpp"
#include "interfaces/plugin_abi.hpp"
#include "math/Color.hpp"

namespace {

// ----------------------------------------------------------------
// PPM image loader + bilinear sampler
// ----------------------------------------------------------------

class TextureImage {
public:
    bool load(const std::string& path)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;

        std::string magic;
        f >> magic;
        if (magic != "P3" && magic != "P6") return false;

        // Skip comment lines and whitespace before reading integers.
        auto skip = [&]() {
            while (f) {
                int c = f.peek();
                if (c == '#') { std::string l; std::getline(f, l); }
                else if (std::isspace(c)) { f.get(); }
                else break;
            }
        };

        skip(); int w; f >> w;
        skip(); int h; f >> h;
        skip(); int maxval; f >> maxval;
        f.get();  // consume the single whitespace that precedes pixel data

        if (w <= 0 || h <= 0 || maxval <= 0) return false;

        const float scale = 1.0f / static_cast<float>(maxval);
        pixels_.resize(w * h);

        if (magic == "P6") {
            for (int i = 0; i < w * h; ++i) {
                unsigned char rgb[3];
                f.read(reinterpret_cast<char*>(rgb), 3);
                pixels_[i] = rt::Color{rgb[0]*scale, rgb[1]*scale, rgb[2]*scale};
            }
        } else {
            for (int i = 0; i < w * h; ++i) {
                int r, g, b;
                f >> r >> g >> b;
                pixels_[i] = rt::Color{r*scale, g*scale, b*scale};
            }
        }

        if (!f && magic == "P6") return false;

        width_  = w;
        height_ = h;
        return true;
    }

    bool empty() const { return pixels_.empty(); }

    // Bilinear sample with tiling (UV wrapping).
    rt::Color sample(float u, float v) const
    {
        // Tile: wrap to [0, 1) by subtracting floor.
        u -= std::floor(u);
        v -= std::floor(v);

        // Map to pixel space [0, w-1] × [0, h-1].
        const float px = u * static_cast<float>(width_  - 1);
        const float py = v * static_cast<float>(height_ - 1);

        const int x0 = static_cast<int>(px);
        const int y0 = static_cast<int>(py);
        const int x1 = std::min(x0 + 1, width_  - 1);
        const int y1 = std::min(y0 + 1, height_ - 1);

        const float dx = px - static_cast<float>(x0);
        const float dy = py - static_cast<float>(y0);

        const rt::Color c00 = pixels_[y0 * width_ + x0];
        const rt::Color c10 = pixels_[y0 * width_ + x1];
        const rt::Color c01 = pixels_[y1 * width_ + x0];
        const rt::Color c11 = pixels_[y1 * width_ + x1];

        // Bilinear interpolation.
        const rt::Color top    = c00 * (1.0f - dx) + c10 * dx;
        const rt::Color bottom = c01 * (1.0f - dx) + c11 * dx;
        return top * (1.0f - dy) + bottom * dy;
    }

private:
    int width_  = 0;
    int height_ = 0;
    std::vector<rt::Color> pixels_;
};

// ----------------------------------------------------------------
// TextureMaterial
// ----------------------------------------------------------------

class TextureMaterial final : public rt::IMaterial {
public:
    TextureMaterial(TextureImage img, float scaleU, float scaleV,
        float ka, float kd, float ks, float shininess)
        : image_(std::move(img))
        , scaleU_(scaleU), scaleV_(scaleV)
        , ka_(ka), kd_(kd), shininess_(shininess)
        , specCol_(rt::Color{ks, ks, ks})
    {
    }

    rt::Color shade(const rt::ShadingContext& ctx) const override
    {
        const float u = static_cast<float>(ctx.hit.uv.x) * scaleU_;
        const float v = static_cast<float>(ctx.hit.uv.y) * scaleV_;
        return image_.sample(u, v);
    }

    float     ka()            const override { return ka_;        }
    float     kd()            const override { return kd_;        }
    float     shininess()     const override { return shininess_;  }
    rt::Color specularColor() const override { return specCol_;    }

private:
    TextureImage image_;
    float        scaleU_;
    float        scaleV_;
    float        ka_;
    float        kd_;
    float        shininess_;
    rt::Color    specCol_;
};

// ----------------------------------------------------------------
// Plugin factory
// ----------------------------------------------------------------

float readFloat(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<float>(static_cast<int>(s));
    return static_cast<float>(static_cast<double>(s));
}

void* create(const libconfig::Setting& cfg)
{
    try {
        if (!cfg.exists("file")) return nullptr;
        const std::string path = static_cast<const char*>(cfg["file"]);

        TextureImage img;
        if (!img.load(path)) {
            // load() failed — warn via stderr, return null
            return nullptr;
        }

        const float scaleU    = cfg.exists("scale_u")   ? readFloat(cfg["scale_u"])   : 1.0f;
        const float scaleV    = cfg.exists("scale_v")   ? readFloat(cfg["scale_v"])   : 1.0f;
        const float ka        = cfg.exists("ka")        ? readFloat(cfg["ka"])        : 0.12f;
        const float kd        = cfg.exists("kd")        ? readFloat(cfg["kd"])        : 0.80f;
        const float ks        = cfg.exists("ks")        ? readFloat(cfg["ks"])        : 0.10f;
        const float shininess = cfg.exists("shininess") ? readFloat(cfg["shininess"]) : 16.0f;

        return new TextureMaterial(std::move(img), scaleU, scaleV,
            ka, kd, ks, shininess);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<TextureMaterial*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Material, "texture", &create, &destroy)
