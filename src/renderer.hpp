#pragma once

#include "glm/glm.hpp"
#include "imgui.h"
#include "scene.hpp"

#include "camera.hpp"
#include "shader.hpp"

#include <filesystem>

namespace ren {

class Renderer {
public:
  struct Transformations {
    int screen_width{};
    int screen_height{};
    float screen_aspect{};
    glm::mat4 projection{};
    std::shared_ptr<Camera> cam{};
  };

  Renderer() = default;
  virtual ~Renderer(){};
  virtual void render(Scene const &, Transformations const &, double ticks) = 0;
  virtual void draw_dialog() = 0;

private:
};

} // namespace ren