#pragma once

#include "glm/glm.hpp"

namespace ren {

class Scene;


class Renderer {
public:
  Renderer() = default;
  virtual ~Renderer() {};
  virtual void render(Scene const&, double ticks, glm::vec3 light_pos) = 0;

private:
};

} // namespace ren