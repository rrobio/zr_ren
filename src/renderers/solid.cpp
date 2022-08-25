#include "solid.hpp"

#include "../camera.hpp"
#include "../scene.hpp"

namespace ren {
SolidRenderer::SolidRenderer(std::filesystem::path root_dir) {
  m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
                               root_dir / "shaders/solid_color.frag");
  assert(m_solid_shader.success());

  glEnable(GL_DEPTH_TEST);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SolidRenderer::render(const Scene &scene, Transformations const &trans,
                           double ticks) {
  // auto trans = scene.transformations();

  auto const &light = scene.lights()[0];

  glViewport(0, 0, trans.screen_width, trans.screen_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_solid_shader.use();
  m_solid_shader.set<glm::mat4>("projection", trans.projection);
  m_solid_shader.set<glm::mat4>("view", trans.view);

  for (auto const &obj : scene.objects()) {
    m_solid_shader.set<glm::mat4>("model", obj.model());
    m_solid_shader.set<glm::vec3>("color", {0.5f, 1.f, 0.2f});
    obj.draw();
  }

  m_solid_shader.set<glm::mat4>("model", light.model());
  m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  scene.lights().front().draw();
}
} // namespace ren
