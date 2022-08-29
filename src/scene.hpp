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
  Scene() : m_lights({ren::create_sphere()}) {}
  // Scene(std::initializer_list<Object&&> l) : m_objects(std::move(l)) {}
  void add_object(Object &&);
  void set_lights(std::array<Object, 1> lights) {
    m_lights = std::move(lights);
  }
  // Scene(std::vector<Object> l) : m_objects(l) {}

  auto *object_at(size_t index) { return &m_objects.at(index); }
  auto &objects() const { return m_objects; }
  auto &lights() const { return m_lights; }
  auto *light_at(size_t index) { return &m_lights.at(index); }
  auto size() const { return m_objects.size(); }

private:
  std::vector<Object> m_objects;
  std::array<Object, 1> m_lights{}; // currently limited only to one light per
                                    // scene for simplicity
};

} // namespace ren
