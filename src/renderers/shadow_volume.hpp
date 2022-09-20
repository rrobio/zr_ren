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

  void render_into_depth(Scene const &scene, Transformations const &trans);
  void render_shadow_volume_into_stencil(Scene const &scene,
                                         Transformations const &trans);
  void render_shadowed_scene(Scene const &scene, Transformations const &trans);
  void render_ambient(Scene const &scene, Transformations const &trans);
  void render_lights(Scene const &, Transformations const &);
};
} // namespace ren