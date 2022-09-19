#include "shadow_volume.hpp"
#include <glm/gtx/quaternion.hpp>

namespace ren {
ShadowVolumeRenderer::ShadowVolumeRenderer(
    std::filesystem::path root_dir, Transformations const &transformations) {
  m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
                               root_dir / "shaders/solid_color.frag");
  assert(m_solid_shader.success());
  m_first_pass = ren::Shader(root_dir / "shaders/vs_first_pass.vert",
                             root_dir / "shaders/vs_first_pass.frag");
  assert(m_first_pass.success());
  m_complete = ren::Shader(root_dir / "shaders/vs_complete.vert",
                           root_dir / "shaders/vs_complete.frag");
  assert(m_complete.success());
  m_shadow_volume = ren::Shader(root_dir / "shaders/shadow_volume.vert",
                                root_dir / "shaders/shadow_volume.frag",
                                root_dir / "shaders/shadow_volume.geom");
  assert(m_shadow_volume.success());

  m_material = ren::Shader(root_dir / "shaders/material.vert",
                           root_dir / "shaders/material.frag");
  assert(m_material.success());
}
ShadowVolumeRenderer::~ShadowVolumeRenderer() {}
void ShadowVolumeRenderer::render(const Scene &scene,
                                  Transformations const &trans, double ticks) {

  auto &light = scene.lights().at(0);
  auto const view = trans.cam->view();
  auto const screen_width = trans.screen_width;
  auto const screen_height = trans.screen_height;
  glDepthMask(GL_TRUE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glClearStencil(0);
  glDisable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);

  // Render scene into depth
  glDrawBuffer(GL_NONE);

  m_first_pass.use();

  m_first_pass.set("projection", trans.projection);
  m_first_pass.set("view", view);
  for (auto const &obj : scene.objects()) {
    m_first_pass.set("model", obj.model());
    obj.draw();
  }

  glEnable(GL_STENCIL_TEST);

  // Render shadow volume into stencil

  glDepthMask(GL_FALSE);
  glEnable(GL_DEPTH_CLAMP);
  glDisable(GL_CULL_FACE);

  // We need the stencil test to be enabled but we want it
  // to succeed always. Only the depth test matters.
  glStencilFunc(GL_ALWAYS, 0, 0xFF);

  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

  m_shadow_volume.use();
  m_shadow_volume.set<vec3>("gLightPos", light.translation());
  // Render the occluder
  m_shadow_volume.set("projection", trans.projection);
  m_shadow_volume.set("view", trans.cam->view());
  for (auto const &obj : scene.objects()) {
    m_shadow_volume.set("model", obj.model());
    obj.draw();
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // Restore local stuff
  glDisable(GL_DEPTH_CLAMP);
  glEnable(GL_CULL_FACE);

  // Render shadowed scene
  glDrawBuffer(GL_BACK);

  // Draw only if the corresponding stencil value is zero
  glStencilFunc(GL_EQUAL, 0x0, 0xFF);

  // prevent update to the stencil buffer
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  m_complete.use();
  m_complete.set<vec3>("lightPos", light.translation());

  m_complete.set<vec3>("lightColor", vec3(1.f));

  m_complete.set<glm::mat4>("projection", trans.projection);
  m_complete.set<glm::mat4>("view", trans.cam->view());
  for (auto const &obj : scene.objects()) {
    m_complete.set<glm::mat4>("model", obj.model());
    m_complete.set<vec3>("objectColor", vec3(0.4f));
    obj.draw();
  }

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  m_complete.use();
  m_complete.set<vec3>("lightPos", light.translation());

  m_complete.set<vec3>("lightColor", vec3(1.f));

  m_complete.set<glm::mat4>("projection", trans.projection);
  m_complete.set<glm::mat4>("view", trans.cam->view());
  for (auto const &obj : scene.objects()) {
    m_complete.set<glm::mat4>("model", obj.model());
    m_complete.set<vec3>("objectColor", vec3(0.4f));
    obj.draw();
  }

  glDisable(GL_BLEND);

  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  glDisable(GL_STENCIL_TEST);

  m_solid_shader.use();
  m_solid_shader.set<glm::mat4>("projection", trans.projection);
  m_solid_shader.set<glm::mat4>("view", trans.cam->view());
  m_solid_shader.set<glm::mat4>("model", light.model());
  m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  light.draw();
}
void ShadowVolumeRenderer::draw_dialog() { ImGui::Text("Shadow Volume"); }

} // namespace ren
