#pragma once

#include "../../util.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "../../ray.hpp"
#include "../../vec3.hpp"

namespace ren {

struct hit_record;

double schlick(double cosine, double ref_idx) {
  auto r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class material {
public:
  virtual ~material() = default;
  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered) const = 0;
};

class lambertian : public material {
public:
  lambertian(color const &a) : albedo(a) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered) const override {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }

public:
  color albedo;
};

class metal : public material {
public:
  metal(color const &a, float f) : albedo(a), fuzz(f) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered) const override {
    vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  color albedo;
  float fuzz;
};

class dielectric : public material {
public:
  dielectric(double ri) : ref_idx(ri) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered) const override {
    attenuation = color(1.0, 1.0, 1.0);
    double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

    vec3 unit_direction = glm::normalize(r_in.direction());
    double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    if (etai_over_etat * sin_theta > 1.0) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    double reflect_prob = schlick(cos_theta, etai_over_etat);
    if (random_float() < reflect_prob) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
    scattered = ray(rec.p, refracted);
    return true;
  }

  double ref_idx;
};

} // namespace ren
