#include "material.hpp"

namespace ren {
float schlick(float cosine, float ref_idx) {
  auto r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}
} // namespace ren
