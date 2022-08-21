#include "scene.hpp"
#include "object.hpp"
#include "shader.hpp"

namespace ren {
void Scene::add_object(Object &&obj) { m_objects.push_back(std::move(obj)); }
void Scene::render(Shader &shader) const {
  shader.use();
  for (auto const &obj : m_objects) {
    shader.set("model", obj.model());
    obj.draw();
  }
}

} // namespace ren
