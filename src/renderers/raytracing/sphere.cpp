#include "sphere.hpp"

namespace ren {
bool sphere::hit(ray const &r, float t_min, float t_max,
                 hit_record &rec) const {
  vec3 oc = r.origin() - center;
  auto a = glm::length2(r.direction());
  auto half_b = dot(oc, r.direction());
  auto c = glm::length2(oc) - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  if (discriminant > 0) {
    auto root = sqrt(discriminant);
    auto temp = (-half_b - root) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.at(rec.t);
      vec3 outward_normal = (rec.p - center) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-half_b + root) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.at(rec.t);
      vec3 outward_normal = (rec.p - center) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

} // namespace ren