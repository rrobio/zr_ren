#pragma once

#include "../renderer.hpp"

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"

namespace ren {

class MaterialRenderer final : public Renderer {
public:
  MaterialRenderer(std::filesystem::path root_dir);
  ~MaterialRenderer() = default;
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override {}

private:
  Shader m_material_shader;
  Shader m_solid_shader;
};

} // namespace ren