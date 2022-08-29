#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "mesh.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "hittable.hpp"
#include "material.hpp"

namespace ren {
class Object {
public:
  using hit_function =
      std::function<bool(Object const &obj, ray const &r, float t_min,
                         float t_max, hit_record &rec)>;

  enum class Type {
    sphere,
    plane,
    custom,
  };

  Object() = default;
  Object(Object &&o) noexcept
      : m_mesh(std::move(o.m_mesh)), m_material(o.m_material),
        m_model(o.m_model) {}
  Object(Object &o) = delete;
  Object(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
    m_mesh = Mesh::construct(vertices, indices);
  }
  Object(std::vector<Vertex> vertices) { m_mesh = Mesh::construct(vertices); }
  Object &operator=(Object &&o) {
    m_mesh = std::move(o.m_mesh);
    m_hit = o.m_hit;
    m_material = o.m_material;
    m_model = o.m_model;
    m_translation = o.m_translation;
    m_scale = o.m_scale;
    m_type = o.m_type;

    return *this;
  }

  void draw() const {
    assert(m_mesh != nullptr);
    m_mesh->draw();
  }
  bool is_valid() const { return m_mesh != nullptr; }

  auto model() const { return m_model; }
  void set_model(glm::mat4 m) { m_model = m; }
  // auto model() const { return m_model; }
  // void set_model(glm::mat4 m) { m_model = m; }
  auto translation() const { return m_translation; }
  void set_translation(vec3 t) { m_translation = t; }
  auto scale() const { return m_scale; }
  void set_scale(vec3 s) { m_scale = s; }

  auto material() const -> std::shared_ptr<Material> { return m_material; }
  void set_material(std::shared_ptr<Material> m) { m_material = m; }

  auto type() const { return m_type; }
  auto set_type(Type t) { m_type = t; }

  auto hit() const -> hit_function { return m_hit; }
  auto set_hit_function(hit_function h) { m_hit = h; }

private:
  hit_function m_hit;
  std::unique_ptr<Mesh> m_mesh{};
  std::shared_ptr<Material> m_material{};
  glm::mat4 m_model{};
  // glm::mat4 m_model{};
  vec3 m_translation;
  vec3 m_scale;
  Type m_type{Type::custom};
};

Object create_sphere();
Object create_sphere(glm::vec3 cen, float r, std::shared_ptr<Material> m);
Object create_cube();
Object create_textured_cube();
Object create_plane();
Object create_skybox();

} // namespace ren
