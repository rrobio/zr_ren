#pragma once

#include "vec3.hpp"

namespace ren {

class ray {
public:
  ray() {}
  ray(point3 const &origin, vec3 const &direction)
      : orig(origin), dir(direction) {}

  point3 origin() const { return orig; }
  vec3 direction() const { return dir; }

  point3 at(float t) const { return orig + t * dir; }

public:
  point3 orig;
  vec3 dir;
};

} // namespace ren
