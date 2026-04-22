// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16
//
// PrimitiveNode: INode adapter that wraps a single IPrimitive (T1.1).
// Bridges the scene-graph layer (INode) and the geometry layer (IPrimitive).
//
// T1.7 additions:
//   material_  — optional IMaterial* assigned by the parser (non-owning borrow
//                from the Scene's materials_ map). Set on every HitRecord so
//                the renderer can call material->shade().
//   translate_ — world-space offset applied via ray shifting: ray.origin is
//                moved to object space before intersect(), then rec.point is
//                moved back to world space. Normal is unchanged (no rotation).

#ifndef RAYTRACER_CORE_PRIMITIVENODE_HPP
#define RAYTRACER_CORE_PRIMITIVENODE_HPP

#include <memory>

#include "interfaces/INode.hpp"
#include "interfaces/IPrimitive.hpp"
#include "math/Vector3.hpp"

namespace rt {

class PrimitiveNode final : public INode {
public:
    PrimitiveNode(std::shared_ptr<IPrimitive> prim,
                  const IMaterial* mat,
                  Vector3 translate)
        : prim_(std::move(prim)), material_(mat), translate_(translate)
    {
    }

    bool intersect(const Ray& ray, HitRecord& rec) const override
    {
        // Shift ray to object space.
        Ray localRay    = ray;
        localRay.origin = ray.origin - translate_;

        if (!prim_->intersect(localRay, rec)) return false;

        // Shift hit point back to world space.
        rec.point = rec.point + translate_;

        // Assign material (nullptr means "keep whatever the primitive set",
        // which is also nullptr — the renderer falls back to white albedo).
        if (material_) rec.material = material_;

        return true;
    }

    AABB bounds() const override { return prim_->bounds(); }

private:
    std::shared_ptr<IPrimitive> prim_;
    const IMaterial*            material_  = nullptr;
    Vector3                     translate_{};
};

}  // namespace rt

#endif  // RAYTRACER_CORE_PRIMITIVENODE_HPP
