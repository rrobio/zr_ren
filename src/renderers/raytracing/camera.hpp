#pragma once

#include "../../util.hpp"
#include "../../ray.hpp"

namespace ren {

class camera {
public:
  camera(point3 lookfrom, point3 lookat, vec3 vup,
         float vfov, // vertical field-of-view in degrees
         float aspect_ratio) {
    auto theta = glm::radians(vfov);
    auto h = tan(theta / 2);
    float viewport_height = 2.0 * h;
    float viewport_width = aspect_ratio * viewport_height;

    auto w = glm::normalize(lookfrom - lookat);
    auto u = glm::normalize(cross(vup, w));
    auto v = cross(w, u);

    origin = lookfrom;
    horizontal = viewport_width * u;
    vertical = viewport_height * v;
    lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - w;
  }

  ray get_ray(float s, float t) const {
    return ray(origin,
               lower_left_corner + s * horizontal + t * vertical - origin);
  }

private:
  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
};

} // namespace ren
