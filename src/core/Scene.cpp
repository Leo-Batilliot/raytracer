// Raytracer — Epitech G-OOP-400-LIL-4-1-raytracer-16

#include "core/Scene.hpp"

#include "interfaces/IMaterial.hpp"  // full def needed by ~shared_ptr<IMaterial>

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

}  // namespace rt
