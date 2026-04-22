// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// Dummy sphere plugin (T0.15) — proves the end-to-end plugin chain.
// intersect() always returns false; replaced by the real algorithm in T1.1.

#include "interfaces/IPrimitive.hpp"
#include "interfaces/plugin_abi.hpp"

namespace {

class DummySphere final : public rt::IPrimitive {
public:
    bool intersect(const rt::Ray& /*ray*/,
                   rt::HitRecord& /*rec*/) const override
    {
        return false;
    }

    rt::AABB bounds() const override
    {
        return rt::AABB{
            rt::Vector3{-1.0, -1.0, -1.0},
            rt::Vector3{ 1.0,  1.0,  1.0},
        };
    }

    rt::Vector2 uvAt(const rt::Vector3& /*localPoint*/) const override
    {
        return {};
    }
};

void* create(const libconfig::Setting& /*cfg*/)
{
    try {
        return new DummySphere;
    } catch (...) {
        return nullptr;
    }
}

void destroy(void* obj)
{
    delete static_cast<DummySphere*>(obj);
}

}  // namespace

RAYTRACER_PLUGIN_ENTRY(rt::PluginKind::Primitive, "sphere", &create, &destroy)
