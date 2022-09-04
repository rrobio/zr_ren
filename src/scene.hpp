#pragma once

#include <array>
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
  void add_light(Object &&);
  // Scene(std::vector<Object> l) : m_objects(l) {}

  auto *object_at(size_t index) { return &m_objects.at(index); }
  auto &objects() const { return m_objects; }
  auto &lights() const { return m_lights; }
  auto *light_at(size_t index) { return &m_lights.at(index); }
  auto size() const { return m_objects.size(); }

private:
  std::vector<Object> m_objects;
  std::vector<Object> m_lights;
};

} // namespace ren
