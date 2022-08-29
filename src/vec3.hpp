#pragma once

#include <cmath>
#include <iostream>

#include "util.hpp"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

using std::sqrt;

using vec3 = glm::vec3;

// // Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3;  // RGB color

inline static vec3 random_vec3(float min, float max) {
  return vec3(random_float(min, max), random_float(min, max),
              random_float(min, max));
}

namespace {
vec3 random_unit_vector() {
  auto a = random_float(0, 2 * pi);
  auto z = random_float(-1, 1);
  auto r = sqrt(1 - z * z);
  return vec3(r * cos(a), r * sin(a), z);
}

vec3 random_in_unit_sphere() {
  while (true) {
    auto p = random_vec3(-1, 1);
    if (glm::length2(p) >= 1)
      continue;
    return p;
  }
}

vec3 random_in_hemisphere(vec3 const &normal) {
  vec3 in_unit_sphere = random_in_unit_sphere();
  if (glm::dot(in_unit_sphere, normal) > 0.0)
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

vec3 reflect(vec3 const &v, vec3 const &n) {
  return v - 2 * glm::dot(v, n) * n;
}
vec3 refract(vec3 const &uv, vec3 const &n, float etai_over_etat) {
  auto cos_theta = glm::dot(-uv, n);
  vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  vec3 r_out_parallel = (float)-sqrt(fabs(1.0 - glm::length2(r_out_perp))) * n;
  return r_out_perp + r_out_parallel;
}

} // namespace ren
