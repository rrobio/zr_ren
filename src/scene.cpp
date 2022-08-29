#include "scene.hpp"
#include "object.hpp"
#include "shader.hpp"

namespace ren {
void Scene::add_object(Object &&obj) { m_objects.push_back(std::move(obj)); }
} // namespace ren
