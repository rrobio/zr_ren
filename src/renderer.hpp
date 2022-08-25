#pragma once

#include "glm/glm.hpp"
#include "imgui.h"

namespace ren {

class Scene;

class Renderer {
public:
  struct Transformations {
    int screen_width{};
    int screen_height{};
    float screen_aspect{};
    glm::mat4 projection{};
    glm::vec3 camera_position{};
    glm::mat4 view{};
  };

  Renderer() = default;
  virtual ~Renderer(){};
  virtual void render(Scene const &, Transformations const &, double ticks) = 0;
  virtual void draw_dialog() = 0;

private:
};

} // namespace ren