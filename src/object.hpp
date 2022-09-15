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
        m_model(o.m_model), m_translation(o.m_translation), m_scale(o.m_scale),
        m_type(o.m_type), m_hit(o.m_hit) {}
  Object(Object &o) = delete;
  Object(std::vector<Vertex> vertices, std::vector<GLuint> indices, bool adjacency = false) {
    m_mesh = Mesh::construct(vertices, indices, adjacency);
  }
  Object(std::vector<Vertex> vertices) { m_mesh = Mesh::construct(vertices, false); }
  Object &operator=(Object &&o) {
    m_mesh = std::move(o.m_mesh);
    m_material = o.m_material;
    m_model = o.m_model;
    m_translation = o.m_translation;
    m_scale = o.m_scale;
    m_type = o.m_type;
    m_hit = o.m_hit;

    return *this;
  }

  void draw() const {
    assert(m_mesh != nullptr);
    m_mesh->draw();
  }
  bool is_valid() const { return m_mesh != nullptr; }

  auto model() const { return m_model; }
  void set_model(glm::mat4 m) { m_model = m; }
  void update_model() {
    auto rotated = glm::rotate(glm::mat4(1.f), glm::radians(m_rotation_scale), m_rotation_vector);
    auto scaled = glm::scale(rotated, m_scale);
    auto scaled_and_translated = glm::translate(scaled, m_translation);
    m_model = scaled_and_translated;
  }
  // auto model() const { return m_model; }
  // void set_model(glm::mat4 m) { m_model = m; }
  auto translation() const { return m_translation; }
  void set_translation(vec3 t) { m_translation = t; }
  auto scale() const { return m_scale; }
  void set_scale(vec3 s) { m_scale = s; }
  auto rotation_vector() const { return m_rotation_vector; }
  auto rotation_scale() const { return m_rotation_scale; }
  void set_rotation_vector(vec3 rt) { m_rotation_vector = rt; }
  void set_rotation_scale(float deg) { m_rotation_scale = deg; }
 
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
  glm::mat4 m_model{1.f};
  vec3 m_translation{0.f};
  vec3 m_scale{1.f};
  vec3 m_rotation_vector{1.f, 1.f, 1.f};
  float m_rotation_scale{0};
  Type m_type{Type::custom};
};

Object create_sphere();
Object create_sphere(glm::vec3 cen, float r, std::shared_ptr<Material> m);
Object create_cube();
Object create_textured_cube();
Object create_plane();
Object create_plane(glm::vec3 cen, vec3 scale, std::shared_ptr<Material> m);
Object create_skybox();

} // namespace ren
