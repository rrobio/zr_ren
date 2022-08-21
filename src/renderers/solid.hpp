#pragma once

#include "../renderer.hpp"

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"

namespace ren {

class SolidRenderer final : public Renderer {
public:
  SolidRenderer(std::filesystem::path root_dir);
  ~SolidRenderer() = default;
  void render(const Scene &, double ticks) override;

private:
  Shader m_solid_shader;
};

} // namespace ren