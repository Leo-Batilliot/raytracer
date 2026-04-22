// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Cone primitive plugin (T2.2).
//
// Canonical finite cone: apex at y = +halfH, base disk at y = -halfH,
// base radius = `radius`, axis aligned with Y. Config `center` = midpoint
// between apex and base.
//
// Implicit surface: f(x,y,z) = x² + z² − k²·(halfH−y)² = 0
//   where k = radius / height
//
// Quadratic: a·t² + 2·half_b·t + c = 0
//   a      = Dx² + Dz² − k²·Dy²
//   half_b = Ox·Dx + Oz·Dz + k²·α·Dy    (α = halfH − Oy)
//   c      = Ox² + Oz² − k²·α²
//
// Body normal: ∇f = (hx, k²·(halfH−hy), hz)  (normalize before use).
// Base cap at y = −halfH, normal = (0,−1,0).
// No top cap (apex is a point).

#include <algorithm>
#include <cmath>
#include <numbers>

#include <libconfig.h++>

#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

constexpr double kEps = 1e-8;

// ----------------------------------------------------------------
// Cone
// ----------------------------------------------------------------

class Cone final : public rt::IPrimitive {
public:
    Cone(rt::Vector3 center, double radius, double height)
        : center_(center)
        , radius_(radius)
        , halfH_(height * 0.5)
        , k_(radius / height)
    {
    }

    bool intersect(const rt::Ray& ray, rt::HitRecord& rec) const override
    {
        // Shift ray to object space (centre at origin).
        const rt::Vector3 orig = ray.origin - center_;
        const rt::Vector3 dir  = ray.direction;

        const double alpha = halfH_ - orig.y;   // Y distance from apex at t=0
        const double k2    = k_ * k_;

        // ── Body quadratic ────────────────────────────────────────────────
        const double a      = dir.x*dir.x + dir.z*dir.z - k2*dir.y*dir.y;
        const double half_b = orig.x*dir.x + orig.z*dir.z + k2*alpha*dir.y;
        const double c      = orig.x*orig.x + orig.z*orig.z - k2*alpha*alpha;

        double      tBest     = ray.tMax;
        bool        hit       = false;
        rt::Vector3 bestNormal{};
        rt::Vector2 bestUV{};
        bool        bestFront = true;

        if (std::abs(a) > kEps) {
            const double disc = half_b*half_b - a*c;
            if (disc >= 0.0) {
                const double sqrtD = std::sqrt(disc);
                // Always try the smaller t first (correct for both a>0 and a<0).
                const double tA = (-half_b - sqrtD) / a;
                const double tB = (-half_b + sqrtD) / a;
                const double tNear = std::min(tA, tB);
                const double tFar  = std::max(tA, tB);

                for (double t : {tNear, tFar}) {
                    if (t < ray.tMin || t >= tBest) continue;
                    const double hy = orig.y + t * dir.y;
                    if (hy < -halfH_ || hy > halfH_) continue;

                    const double hx = orig.x + t * dir.x;
                    const double hz = orig.z + t * dir.z;

                    // Outward normal = ∇f normalised.
                    rt::Vector3  outward{hx, k2*(halfH_ - hy), hz};
                    const double len = outward.length();
                    if (len < kEps) continue;
                    outward = outward / len;

                    const bool front = dir.dot(outward) < 0.0;

                    const double phi = std::atan2(hz, hx);
                    const double u   = phi / (2.0 * std::numbers::pi) + 0.5;
                    const double v   = (hy + halfH_) / (2.0 * halfH_);

                    tBest      = t;
                    bestNormal = front ? outward : -outward;
                    bestUV     = {u, v};
                    bestFront  = front;
                    hit        = true;
                    break;  // tNear is smallest valid — stop
                }
            }
        }

        // ── Base cap (y = −halfH) ────────────────────────────────────────
        if (std::abs(dir.y) > kEps) {
            const double t = (-halfH_ - orig.y) / dir.y;
            if (t >= ray.tMin && t < tBest) {
                const double hx = orig.x + t * dir.x;
                const double hz = orig.z + t * dir.z;
                if (hx*hx + hz*hz <= radius_*radius_) {
                    const rt::Vector3 outward{0.0, -1.0, 0.0};
                    const bool        front = dir.dot(outward) < 0.0;

                    tBest      = t;
                    bestNormal = front ? outward : -outward;
                    bestUV     = {hx/radius_*0.5 + 0.5, hz/radius_*0.5 + 0.5};
                    bestFront  = front;
                    hit        = true;
                }
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

    rt::Vector2 uvAt(const rt::Vector3& /*p*/) const override { return {}; }

private:
    rt::Vector3 center_;
    double      radius_;
    double      halfH_;
    double      k_;     // radius / height = half-angle tangent
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
        return new Cone(center, radius, height);
    } catch (...) {
        return nullptr;
    }
}

static void destroy(void* obj)
{
    delete static_cast<Cone*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "cone", &create, &destroy)
