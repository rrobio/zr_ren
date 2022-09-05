#include "material.hpp"

#include "../camera.hpp"
#include "../scene.hpp"

#include "glm/gtx/string_cast.hpp"

namespace ren {
MaterialRenderer::MaterialRenderer(std::filesystem::path root_dir) {
  m_material_shader = ren::Shader(root_dir / "shaders/material.vert",
                                  root_dir / "shaders/material.frag");
  assert(m_material_shader.success());
  m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
                               root_dir / "shaders/solid_color.frag");
  assert(m_solid_shader.success());
}

void MaterialRenderer::render(const Scene &scene, Transformations const &trans,
                              double ticks) {
  assert(trans.cam);
  // auto trans = scene.transformations();

  auto const &light = scene.lights()[0];

  glViewport(0, 0, trans.screen_width, trans.screen_height);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_material_shader.use();
  m_material_shader.set<glm::mat4>("projection", trans.projection);
  m_material_shader.set<glm::mat4>("view", trans.cam->view());

  auto const &l_mat = light.material();
  assert(l_mat != nullptr);
  m_material_shader.set<glm::vec3>("light.ambient", l_mat->ambient);
  m_material_shader.set<glm::vec3>("light.diffuse", l_mat->diffuse);
  m_material_shader.set<glm::vec3>("light.specular", l_mat->specular);

  m_material_shader.set<glm::vec3>("light.position", light.translation());

  for (auto const &obj : scene.objects()) {
    auto const &mat = obj.material();
    assert(mat != nullptr);
    m_material_shader.set<glm::mat4>("model", obj.model());
    // std::cout << glm::to_string(mat->ambient) << std::endl;
    m_material_shader.set<glm::vec3>("material.ambient", mat->ambient);
    m_material_shader.set<glm::vec3>("material.diffuse", mat->diffuse);
    m_material_shader.set<glm::vec3>("material.specular", mat->specular);
    m_material_shader.set<float>("material.shininess", mat->shininess);
    obj.draw();
  }

  m_solid_shader.use();
  m_solid_shader.set<glm::mat4>("projection", trans.projection);
  m_solid_shader.set<glm::mat4>("view", trans.cam->view());
  m_solid_shader.set<glm::mat4>("model", light.model());
  m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  light.draw();
}
} // namespace ren
