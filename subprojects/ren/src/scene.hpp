#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "object.hpp"
#include "shader.hpp"

namespace ren {

class Scene {
public:
  Scene(std::initializer_list<Object> l) : m_objects(l) {}

  void render_scene(Shader const &);

private:
  std::vector<Object> m_objects;
};

} // namespace ren
