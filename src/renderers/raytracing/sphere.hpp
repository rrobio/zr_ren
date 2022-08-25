#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

namespace ren {

class sphere : public hittable {
public:
  sphere() {}
  sphere(point3 cen, double r, std::shared_ptr<material> m)
      : center(cen), radius(r), mat_ptr(m){};

  virtual bool hit(ray const &r, double t_min, double t_max,
                   hit_record &rec) const override;

public:
  point3 center;
  double radius;
  std::shared_ptr<material> mat_ptr;
};

} // namespace ren
