#include "shadow_mapping.hpp"

#include "../camera.hpp"
#include "../scene.hpp"

namespace ren {
ShadowMappingRenderer::ShadowMappingRenderer(std::filesystem::path root_dir,
                                             int const a_shadow_width,
                                             int const a_shadow_height)
    : shadow_width(a_shadow_width), shadow_height(a_shadow_height) {
  m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
                               root_dir / "shaders/solid_color.frag");
  assert(m_solid_shader.success());
  m_depth_shader = ren::Shader(root_dir / "shaders/point_shadow_depth.vert",
                               root_dir / "shaders/point_shadow_depth.frag",
                               root_dir / "shaders/point_shadow_depth.geom");
  assert(m_depth_shader.success());
  m_shadow_shader = ren::Shader(root_dir / "shaders/shadows.vert",
                                root_dir / "shaders/shadows.frag");
  assert(m_shadow_shader.success());

  glEnable(GL_DEPTH_TEST);

  glGenFramebuffers(1, &m_depth_map_FBO);
  glGenTextures(1, &m_depth_cubemap);

  glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
  for (GLuint i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  float const shadow_aspect =
      static_cast<float>(shadow_width) / static_cast<float>(shadow_height);
  m_shadow_proj = glm::perspective(glm::radians(90.0f), shadow_aspect,
                                   near_plane, far_plane);
  auto light_pos = glm::vec3(1.f);
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                  glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                  glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, 1.0f)));
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(0.0f, -1.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, -1.0f)));
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                  glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms.push_back(
      m_shadow_proj * glm::lookAt(light_pos,
                                  light_pos + glm::vec3(0.0f, 0.0f, -1.0f),
                                  glm::vec3(0.0f, -1.0f, 0.0f)));
}

void ShadowMappingRenderer::render(const Scene &scene,
                                   Transformations const &trans, double ticks) {

  auto const &light = scene.lights()[0];
  auto const light_pos =
      glm::vec3(light.model()[3]); // get the translation vector
  m_shadow_transforms[0] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms[1] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms[2] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f)));
  m_shadow_transforms[3] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(0.0f, -1.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, -1.0f)));
  m_shadow_transforms[4] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                   glm::vec3(0.0f, -1.0f, 0.0f)));
  m_shadow_transforms[5] =
      (m_shadow_proj * glm::lookAt(light_pos,
                                   light_pos + glm::vec3(0.0f, 0.0f, -1.0f),
                                   glm::vec3(0.0f, -1.0f, 0.0f)));

  // 1. render scene to depth cubemap
  // --------------------------------
  glViewport(0, 0, shadow_width, shadow_height);
  glBindFramebuffer(GL_FRAMEBUFFER, m_depth_map_FBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  m_depth_shader.use();
  for (unsigned int i = 0; i < 6; ++i)
    m_depth_shader.set(("shadow_matrices[" + std::to_string(i) + "]").c_str(),
                       m_shadow_transforms[i]);
  m_depth_shader.set("far_plane", far_plane);
  m_depth_shader.set("light_pos", light_pos);
  scene.render(m_depth_shader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 2. render scene as normal
  // -------------------------
  glViewport(0, 0, trans.screen_width, trans.screen_height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_shadow_shader.use();
  m_shadow_shader.set("projection", trans.projection);
  m_shadow_shader.set("view", trans.view);
  // set lighting uniforms
  m_shadow_shader.set("light.position", light_pos);
  m_shadow_shader.set("view_pos", trans.camera_position);
  // shadow_shader.set<int>("shadows", true); // enable/disable shadows by
  // pressing 'SPACE'
  m_shadow_shader.set("far_plane", far_plane);

  m_shadow_shader.set<GLuint>("diffuse_texture", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 1); // no_tex.id);

  m_shadow_shader.set<GLuint>("depth_map", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
  scene.render(m_shadow_shader);

  m_solid_shader.use();
  m_solid_shader.set<glm::mat4>("projection", trans.projection);
  m_solid_shader.set<glm::mat4>("view", trans.view);
  m_solid_shader.set<glm::mat4>("model", light.model());
  m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  scene.lights().front().draw();
}
} // namespace ren
