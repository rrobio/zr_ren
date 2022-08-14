#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "object.hpp"

namespace ren {

class Shader;

class Scene {
public:
  Scene() = default;
  // Scene(std::initializer_list<Object&&> l) : m_objects(std::move(l)) {}
  void add_object(Object &&);
  // Scene(std::vector<Object> l) : m_objects(l) {}

  void render(Shader &);
  auto *object_at(size_t index) { return &m_objects.at(index); }
  auto &objects() { return m_objects; }
  auto size() { return m_objects.size(); }

private:
  std::vector<Object> m_objects;
};

} // namespace ren
