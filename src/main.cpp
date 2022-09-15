// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <cassert>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <random>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "camera.hpp"
#include "cubemap.hpp"
#include "input.hpp"
#include "log.hpp"
#include "material.hpp"
#include "object.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "util.hpp"
#include "window.hpp"

#include "renderers/material.hpp"
#include "renderers/raytracing.hpp"
#include "renderers/shadow_mapping.hpp"
#include "renderers/shadow_volume.hpp"

enum RenderIndex {
  simple_shadow_mapping = 0,
  shadow_volume,
  material,
  raytracing,
};

int const screen_width = 1024;
int const screen_height = 768;
float const screen_aspect =
    static_cast<float>(screen_width) / static_cast<float>(screen_height);
int const shadow_width = screen_width*4;
int const shadow_height = shadow_width;;

bool should_close = false;
bool debug = false;

void toggle_debug(ren::Window &window, ImGuiIO &io) {
  auto const io_flags = ImGuiConfigFlags_NoMouse;
  if (!debug) {
    window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    io.ConfigFlags &= ~io_flags;
    debug = true;
  } else {
    window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    io.ConfigFlags |= io_flags;
    debug = false;
  }
}

auto get_root_directory() -> std::filesystem::path {
  auto path = std::filesystem::canonical("/proc/self/exe");

  do {
    path = path.parent_path();
  } while (path.filename().string() != "zr");

  return path;
}
int main() {
  auto const ren_directory = get_root_directory();

  // Initialization -------------------
  auto window = ren::Window("ren", screen_width, screen_height, false);
  window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  window.set_input_mode(GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  auto cam = std::make_shared<ren::Camera>(
      ren::Camera(glm::vec3(-2.0f, 2.0f, 1.0f), glm::vec3(0.f, 0.f, -1.f),
                  glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, screen_aspect));

  glEnable(GL_DEPTH_TEST);

  auto no_tex_img = ren::Image(ren_directory / "res/tex/no-tex.png");
  auto no_tex = ren::Texture(no_tex_img);

  ImGui::CreateContext();
  auto &io = ImGui::GetIO();

  ImGui_ImplGlfw_InitForOpenGL(window.get_ptr(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  ren::Log::init();

  // Setup the scene -------------------
  ren::Scene scene{};

  auto material_plane =
      ren::Material::create_material_from_scatter<ren::lambertian>(color(0.8, 0.8, 0.0));
  auto material_sphere =
      ren::Material::create_material_from_scatter<ren::lambertian>(color(0.1, 0.2, 0.5));
  auto material_light =
      ren::Material::create_material_from_scatter<ren::diffuse_light>(color(1.f, 1.f, 1.f));

  scene.add_light(ren::create_sphere(point3(0, 0, 0), 1.f, material_light));

  scene.add_object(ren::create_plane(vec3(0.f, -5.f, 0.f),
                                     vec3(20.f, 1.f, 20.f), material_plane));

  for (size_t i = 1; i < 10; i++) {
    auto x = (random_float() * 2 - 1) * 5;
    auto y = random_float() * 5;
    auto z = (random_float() * 2 - 1) * 5;
    auto translation = vec3(x, y, z);
    auto sphere_model = glm::translate(glm::mat4(1), translation);

    scene.add_object(ren::create_sphere(point3(x, y, z), 1.f, material_sphere));
  }

  auto const speed = 0.05f;
  auto keymap = ren::Keymap{};
  keymap.set_bind(
      {GLFW_KEY_ESCAPE, [&window]() { window.set_should_close(true); }});
  keymap.set_bind({GLFW_KEY_W, [&cam, speed]() {
                     cam->move(glm::vec3(0.f, 0.f, 1.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_A, [&cam, speed]() {
                     cam->move(glm::vec3(-1.f, 0.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_S, [&cam, speed]() {
                     cam->move(glm::vec3(0.f, 0.f, -1.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_D, [&cam, speed]() {
                     cam->move(glm::vec3(1.f, 0.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_E, [&cam, speed]() {
                     cam->move(glm::vec3(0.f, 1.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_Q, [&cam, speed]() {
                     cam->move(glm::vec3(0.f, -1.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_F1, GLFW_RELEASE,
                   [&window, &io]() { toggle_debug(window, io); }});
  window.set_keymap(keymap);

  ren::Renderer::Transformations transformations{
      screen_width,
      screen_height,
      static_cast<float>(screen_width) / static_cast<float>(screen_height),
      glm::perspective(glm::radians(cam->fov()), screen_aspect, 0.1f, 100.f),
      cam,
  };

  auto smrenderer = std::make_shared<ren::ShadowMappingRenderer>(
      ren_directory, shadow_width, shadow_height);
  auto svrenderer = std::make_shared<ren::ShadowVolumeRenderer>(ren_directory, transformations);
  auto materialrenderer =
      std::make_shared<ren::MaterialRenderer>(ren_directory);
  auto raytracing_renderer =
      std::make_shared<ren::RayTracingRenderer>(ren_directory);

  std::shared_ptr<ren::Renderer> current_renderer = smrenderer;

  RenderIndex current_render_index = RenderIndex::simple_shadow_mapping;
  auto new_render_index = current_render_index;

  bool pause_scene = false;
  while (!window.should_close()) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto ticks = glfwGetTime();

    if (!pause_scene) {
      auto const light_pos =
          glm::vec3(glm::sin(ticks) * 3, 10.f, glm::cos(ticks) * 3);

      scene.light_at(0)->set_translation(light_pos);
      scene.light_at(0)->update_model();
    }

    if (current_render_index != new_render_index) {
      current_render_index = new_render_index;
      switch (current_render_index) {
      case RenderIndex::simple_shadow_mapping:
        current_renderer = smrenderer;
        break;
      case RenderIndex::shadow_volume:
        current_renderer = svrenderer;
        break;
      case RenderIndex::material:
        current_renderer = materialrenderer;
        break;
      case RenderIndex::raytracing:
        current_renderer = raytracing_renderer;
        break;
      }
    }

    current_renderer->render(scene, transformations, ticks);

    window.poll_events();
    window.exec_keymap();
    if (!debug)
      cam->rotate_offset(window.get_cursor_pos());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto frametime = glfwGetTime() - ticks;
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Text("frametime: %f", frametime);

    ren::Log::the().draw("Log");

    ImGui::Begin("Renderer");
    ImGui::Checkbox("Puase scene", &pause_scene);
    const char *items[] = {"Simple Shadow Mapping", "Shadow Volume","Material", "RayTracing"};
    static int combo_index = static_cast<int>(current_render_index);
    ImGui::Combo("Renderer", &combo_index, items, IM_ARRAYSIZE(items));
    new_render_index = static_cast<RenderIndex>(combo_index);
    ImGui::Separator();
    current_renderer->draw_dialog();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    window.swap_buffers();
  }
  ren::Log::destroy();
}
