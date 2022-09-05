#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "ray.hpp"
#include "vec3.hpp"

#include "hittable.hpp"

namespace ren {
float schlick(float cosine, float ref_idx);

class Scatter {
public:
  virtual ~Scatter() = default;
  virtual color emitted() const { return color(0, 0, 0); }
  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered, float &pdf) const {
    return false;
  }
  virtual float scattering_pdf(ray const &r_in, hit_record const &rec,
                                ray const &scatterd) const {
    return 0;
  }
};

class diffuse_light : public Scatter {
public:
  diffuse_light(color c) : emit(c) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       color &attenuation, ray &scattered,
                       float &pdf) const override {
    return false;
  }

  virtual color emitted() const override { return emit; }

  color emit;
};

class lambertian : public Scatter {
public:
  lambertian(color const &a) : albedo(a) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered,
                       float &pdf) const override {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    pdf = glm::dot(rec.normal, scattered.direction()) / pi;
    return true;
  }
  float scattering_pdf(ray const &r_in, hit_record const &rec,
                       ray const &scattered) const override {
    float cosine = dot(rec.normal, glm::normalize(scattered.direction()));
    return cosine < 0 ? 0 : cosine / pi;
  }

public:
  color albedo;
};

class metal : public Scatter {
public:
  metal(color const &a, float f) : albedo(a), fuzz(f) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered,
                       float &pdf) const override {
    vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

public:
  color albedo;
  float fuzz;
};

class dielectric : public Scatter {
public:
  dielectric(float ri) : ref_idx(ri) {}

  virtual bool scatter(ray const &r_in, hit_record const &rec,
                       color &attenuation, ray &scattered,
                       float &pdf) const override {
    attenuation = color(1.0, 1.0, 1.0);
    float etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

    vec3 unit_direction = glm::normalize(r_in.direction());
    float cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    if (etai_over_etat * sin_theta > 1.0) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    float reflect_prob = schlick(cos_theta, etai_over_etat);
    if (random_float() < reflect_prob) {
      vec3 reflected = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, reflected);
      return true;
    }
    vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
    scattered = ray(rec.p, refracted);
    return true;
  }

  float ref_idx;
};

struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  std::shared_ptr<Scatter> scatter;

  template <typename T, typename... Ts>
  static auto create_material_from_scatter(Ts... args)
      -> std::shared_ptr<ren::Material> {
    auto scatter = std::make_shared<T>(args...);
    return std::make_shared<ren::Material>(scatter);
  }

  Material() = default;
  Material(std::shared_ptr<Scatter> s) : scatter(s) {}
  Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin)
      : ambient(amb), diffuse(diff), specular(spec), shininess(shin) {}
};
} // namespace ren
