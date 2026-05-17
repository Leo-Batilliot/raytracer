#include <cmath>
#include <limits>

#include <libconfig.h++>

#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

constexpr double kParallelEps = 1e-8;

double readScalar(const libconfig::Setting& s)
{
    if (s.getType() == libconfig::Setting::TypeInt)
        return static_cast<double>(static_cast<int>(s));
    return static_cast<double>(s);
}

class Plane final : public rt::IPrimitive {
public:
    Plane(rt::Vector3 normal, double distance)
        : normal_(normal), distance_(distance)
    {
        const rt::Vector3 helper =
            (std::abs(normal_.x) > 0.9)
                ? rt::Vector3{0.0, 1.0, 0.0}
                : rt::Vector3{1.0, 0.0, 0.0};
        tangent_   = normal_.cross(helper).normalized();
        bitangent_ = normal_.cross(tangent_);
    }

    bool intersect(const rt::Ray& ray, rt::HitRecord& rec) const override
    {
        const double denom = normal_.dot(ray.direction);
        if (std::abs(denom) < kParallelEps) return false;

        const double t =
            (distance_ - normal_.dot(ray.origin)) / denom;
        if (t < ray.tMin || t > ray.tMax) return false;

        rec.t        = t;
        rec.point    = ray.at(t);
        rec.frontFace = denom < 0.0;
        rec.normal   = rec.frontFace ? normal_ : -normal_;
        rec.uv       = uvAt(rec.point - distance_ * normal_);
        rec.material = nullptr;
        return true;
    }

    rt::AABB bounds() const override
    {
        constexpr double inf = std::numeric_limits<double>::infinity();
        return rt::AABB{
            rt::Vector3{-inf, -inf, -inf},
            rt::Vector3{ inf,  inf,  inf},
        };
    }

    rt::Vector2 uvAt(const rt::Vector3& localPoint) const override
    {
        return rt::Vector2{
            localPoint.dot(tangent_),
            localPoint.dot(bitangent_),
        };
    }

private:
    rt::Vector3 normal_;
    double      distance_;
    rt::Vector3 tangent_;
    rt::Vector3 bitangent_;
};

void* create(const libconfig::Setting& cfg)
{
    try {
        const rt::Vector3 normal{
            readScalar(cfg["normal"]["x"]),
            readScalar(cfg["normal"]["y"]),
            readScalar(cfg["normal"]["z"]),
        };
        const double distance = readScalar(cfg["distance"]);
        return new Plane(normal.normalized(), distance);
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<Plane*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "plane", &create, &destroy)
