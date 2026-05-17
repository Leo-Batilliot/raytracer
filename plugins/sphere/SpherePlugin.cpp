#include <cmath>
#include <numbers>

#include <libconfig.h++>

#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

double readScalar(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<double>(static_cast<int>(s));
    return static_cast<double>(s);
}

class Sphere final : public rt::IPrimitive {
public:
    Sphere(rt::Vector3 center, double radius)
        : center_(center), radius_(radius)
    {
    }

    bool intersect(const rt::Ray& ray, rt::HitRecord& rec) const override
    {
        const rt::Vector3 oc     = ray.origin - center_;
        const double      a      = ray.direction.dot(ray.direction);
        const double      half_b = oc.dot(ray.direction);
        const double      c      = oc.dot(oc) - radius_ * radius_;
        const double      disc   = half_b * half_b - a * c;

        if (disc < 0.0) return false;

        const double sqrtd = std::sqrt(disc);

        double t = (-half_b - sqrtd) / a;
        if (t < ray.tMin || t > ray.tMax) {
            t = (-half_b + sqrtd) / a;
            if (t < ray.tMin || t > ray.tMax)
                return false;
        }

        rec.t     = t;
        rec.point = ray.at(t);

        const rt::Vector3 outward = (rec.point - center_) / radius_;
        rec.frontFace = ray.direction.dot(outward) < 0.0;
        rec.normal    = rec.frontFace ? outward : -outward;
        rec.uv        = uvAt(outward);
        rec.material  = nullptr;
        return true;
    }

    rt::AABB bounds() const override
    {
        const rt::Vector3 r{radius_, radius_, radius_};
        return rt::AABB{center_ - r, center_ + r};
    }

    rt::Vector2 uvAt(const rt::Vector3& p) const override
    {
        // Spherical UV: theta = acos(-p.y), phi = atan2(-p.z, p.x) + pi.
        const double theta = std::acos(-p.y);
        const double phi   = std::atan2(-p.z, p.x) + std::numbers::pi;
        return rt::Vector2{
            phi   / (2.0 * std::numbers::pi),
            theta / std::numbers::pi,
        };
    }

private:
    rt::Vector3 center_;
    double      radius_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Vector3 center{
            readScalar(cfg["center"]["x"]),
            readScalar(cfg["center"]["y"]),
            readScalar(cfg["center"]["z"]),
        };
        const double radius = readScalar(cfg["radius"]);
        return new Sphere(center, radius);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<Sphere*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "sphere", &create, &destroy)
