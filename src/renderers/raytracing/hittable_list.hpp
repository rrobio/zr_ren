#pragma once

#include <memory>
#include <vector>

#include "hittable.hpp"

namespace ren {

class hittable_list : public hittable {
public:
  hittable_list() {}
  hittable_list(std::shared_ptr<hittable> object) { add(object); }

  void clear() { objects.clear(); }
  void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

  virtual bool hit(ray const &r, double t_min, double t_max,
                   hit_record &rec) const override;

public:
  std::vector<std::shared_ptr<hittable>> objects;
};

} // namespace ren
