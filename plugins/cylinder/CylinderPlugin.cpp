// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Cylinder primitive plugin (T2.1).
//
// Canonical finite cylinder: axis aligned with Y, centre at `center`,
// radius `radius`, total height `height` (extends ±height/2 from centre).
//
// Intersection in two steps:
//   1. Infinite-cylinder body (solve quadratic in xz-plane).
//   2. Flat caps at y = ±halfH (disk test).
// The closest valid t in [tMin, tMax] wins.

#include <cmath>
#include <numbers>

#include <libconfig.h++>

#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

constexpr double kEps = 1e-8;

// ----------------------------------------------------------------
// Cylinder
// ----------------------------------------------------------------

class Cylinder final : public rt::IPrimitive {
public:
    Cylinder(rt::Vector3 center, double radius, double height)
        : center_(center), radius_(radius), halfH_(height * 0.5)
    {
    }

    bool intersect(const rt::Ray& ray, rt::HitRecord& rec) const override
    {
        // Shift ray to cylinder's local frame (centre at origin).
        const rt::Vector3 orig = ray.origin  - center_;
        const rt::Vector3 dir  = ray.direction;

        // ── Body (infinite cylinder, axis Y) ─────────────────────────────
        const double a      = dir.x * dir.x + dir.z * dir.z;
        const double half_b = orig.x * dir.x + orig.z * dir.z;
        const double c      = orig.x * orig.x + orig.z * orig.z
                              - radius_ * radius_;

        double      tBest = ray.tMax;
        bool        hit   = false;
        rt::Vector3 bestNormal{};
        rt::Vector2 bestUV{};
        bool        bestFront = true;

        if (a > kEps) {
            const double disc = half_b * half_b - a * c;
            if (disc >= 0.0) {
                const double sqrtD = std::sqrt(disc);
                // Near root first, then far.
                for (int sign : {-1, 1}) {
                    const double t = (-half_b + sign * sqrtD) / a;
                    if (t < ray.tMin || t >= tBest) continue;
                    const double hy = orig.y + t * dir.y;
                    if (hy < -halfH_ || hy > halfH_) continue;

                    const double hx = orig.x + t * dir.x;
                    const double hz = orig.z + t * dir.z;
                    rt::Vector3  outward{hx / radius_, 0.0, hz / radius_};
                    const bool   front = dir.dot(outward) < 0.0;

                    const double phi = std::atan2(hz, hx);
                    const double u   = phi / (2.0 * std::numbers::pi) + 0.5;
                    const double v   = (hy + halfH_) / (2.0 * halfH_);

                    tBest      = t;
                    bestNormal = front ? outward : -outward;
                    bestUV     = {u, v};
                    bestFront  = front;
                    hit        = true;
                    break;  // near root is closest — stop
                }
            }
        }

        // ── Caps (top: y = +halfH, bottom: y = −halfH) ───────────────────
        if (std::abs(dir.y) > kEps) {
            for (int side : {-1, 1}) {
                const double capY = static_cast<double>(side) * halfH_;
                const double t    = (capY - orig.y) / dir.y;
                if (t < ray.tMin || t >= tBest) continue;
                const double hx = orig.x + t * dir.x;
                const double hz = orig.z + t * dir.z;
                if (hx * hx + hz * hz > radius_ * radius_) continue;

                const rt::Vector3 outward{0.0, static_cast<double>(side), 0.0};
                const bool        front = dir.dot(outward) < 0.0;

                tBest      = t;
                bestNormal = front ? outward : -outward;
                bestUV     = {hx / radius_ * 0.5 + 0.5,
                              hz / radius_ * 0.5 + 0.5};
                bestFront  = front;
                hit        = true;
            }
        }

        if (!hit) return false;

        rec.t         = tBest;
        rec.point     = ray.at(tBest);
        rec.normal    = bestNormal;
        rec.uv        = bestUV;
        rec.frontFace = bestFront;
        rec.material  = nullptr;
        return true;
    }

    rt::AABB bounds() const override
    {
        const rt::Vector3 r{radius_, halfH_, radius_};
        return rt::AABB{center_ - r, center_ + r};
    }

    rt::Vector2 uvAt(const rt::Vector3& /*p*/) const override
    {
        return {};
    }

private:
    rt::Vector3 center_;
    double      radius_;
    double      halfH_;
};

// ----------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------

static double readScalar(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<double>(static_cast<int>(s));
    return static_cast<double>(s);
}

// ----------------------------------------------------------------
// Plugin ABI
// ----------------------------------------------------------------

static void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Vector3 center{
            readScalar(cfg["center"]["x"]),
            readScalar(cfg["center"]["y"]),
            readScalar(cfg["center"]["z"]),
        };
        const double radius = readScalar(cfg["radius"]);
        const double height = readScalar(cfg["height"]);
        return new Cylinder(center, radius, height);
    } catch (...) {
        return nullptr;
    }
}

static void destroy(void* obj)
{
    delete static_cast<Cylinder*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "cylinder", &create, &destroy)
