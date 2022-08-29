#pragma once

#include <memory>

#include "util.hpp"
#include "ray.hpp"

namespace ren {
  class Material;


struct hit_record {
  point3 p;
  vec3 normal;
  std::shared_ptr<Material> mat_ptr;
  float t;
  bool front_face;

  inline void set_face_normal(ray const &r, vec3 const &outward_normal) {
    front_face = glm::dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
public:
  virtual ~hittable() = default;
  virtual bool hit(ray const &r, float t_min, float t_max,
                   hit_record &rec) const = 0;
};
} // namespace ren

