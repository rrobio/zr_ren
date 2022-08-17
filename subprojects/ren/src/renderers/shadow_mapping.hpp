#pragma once

#include "../renderer.hpp"

#include <filesystem>
#include <vector>

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"

namespace ren {

class ShadowMappingRenderer : public Renderer {
public:
  ShadowMappingRenderer(std::filesystem::path root_dir,
                        int const a_shadow_width, int const a_shadow_height);
  ~ShadowMappingRenderer() = default;
  void render(const Scene &, double ticks) override;

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