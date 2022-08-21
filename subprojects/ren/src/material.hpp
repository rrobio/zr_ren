#pragma once

#include <glm/glm.hpp>

namespace ren {
struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  Material() = default;
  Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin)
      : ambient(amb), diffuse(diff), specular(spec), shininess(shin) {}
};
} // namespace ren
