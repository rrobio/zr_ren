#pragma once

#include "../renderer.hpp"

#include <vector>

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

namespace ren {

class ShadowMappingRenderer final : public Renderer {
public:
  ShadowMappingRenderer(std::filesystem::path root_dir,
                        int const a_shadow_width, int const a_shadow_height);
  ~ShadowMappingRenderer() = default;
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override { ImGui::Text("Simple Shadow Maps"); }

private:
  Shader m_solid_shader;
  Shader m_depth_shader;
  Shader m_shadow_shader;

  GLuint m_depth_map_FBO;
  GLuint m_depth_cubemap;

  float near_plane = 1.0f;
  float far_plane = 25.0f;

  int const shadow_width{};
  int const shadow_height{};

  glm::mat4 m_shadow_proj;

  std::vector<glm::mat4> m_shadow_transforms;
};

} // namespace ren