#include "core/Scene.hpp"

#include "interfaces/IMaterial.hpp"

namespace rt {

Scene::~Scene() = default;

const IMaterial* Scene::findMaterial(std::string_view name) const
{
    auto it = materials_.find(std::string(name));
    return it != materials_.end() ? it->second.get() : nullptr;
}

bool Scene::intersect(const Ray& ray, HitRecord& rec) const
{
    if (!root_) return false;
    return root_->intersect(ray, rec);
}

bool Scene::occluded(const Ray& shadowRay) const
{
    if (!root_) return false;
    HitRecord dummy;
    return root_->intersect(shadowRay, dummy);
}

}  // namespace rt
