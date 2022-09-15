#pragma once

#include "../renderer.hpp"
#include "../texture.hpp"

namespace ren {
class ShadowVolumeRenderer final : public Renderer {
public:
  ShadowVolumeRenderer(std::filesystem::path root_dir, Transformations const &);
  ~ShadowVolumeRenderer();
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override;

private:
  Shader m_shadow_volume;
  Shader m_complete;
  Shader m_solid_shader;
  Shader m_first_pass;
  Shader m_material;

  std::vector<Texture> m_textures;

  bool showEdge {false};
  bool showFrustum {false};
	GLuint depthBuf;
	GLuint ambBuf;
	GLuint diffSpecTex;
	GLuint colorDepthFBO = 0;
	GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  
	GLuint bufHandle;
	GLuint fsQuad = 1;
};
} // namespace ren