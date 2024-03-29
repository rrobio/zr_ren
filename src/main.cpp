// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <cassert>
#include <chrono>
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
  raytracing,
};

int const screen_width = 1024;
int const screen_height = 768;
float const screen_aspect = 4.f / 3.f;
// static_cast<float>(screen_width) / static_cast<float>(screen_height);
int const shadow_width = screen_width * 4;
int const shadow_height = shadow_width;
;

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
  } while (path.filename().string() != "zr" && path.filename().string() != "zr_ren" && path.string() != "/");
  assert(path.string() != "/");
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
                  glm::vec3(0.0f, 1.0f, 0.0f), 100.0f, screen_aspect));

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
      ren::Material::create_material_from_scatter<ren::lambertian>(
          color(0.8, 0.8, 0.0));
  auto material_sphere =
      ren::Material::create_material_from_scatter<ren::lambertian>(
          color(0.1, 0.2, 0.5));
  auto material_light =
      ren::Material::create_material_from_scatter<ren::diffuse_light>(
          color(1.f, 1.f, 1.f));

  scene.add_light(ren::create_sphere(point3(0, 0, 0), 0.1f, material_light));

  scene.add_object(ren::create_plane(vec3(0.f, -5.f, 0.f),
                                     vec3(20.f, 1.f, 20.f), material_plane));

  // auto plane_model = glm::rotate(glm::mat4(1.f),glm::radians(180.f),
  // vec3(0.f,0.f,1.f)); plane_model = glm::translate(plane_model,vec3(0.f, 5.f,
  // 0.f)); plane_model = glm::scale(plane_model, vec3(20.f, 1.f, 20.f));
  // scene.object_at(0)->set_model(plane_model);
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
      glm::perspective(glm::radians(cam->fov()), screen_aspect, 0.1f, 50.f),
      cam,
  };

  auto smrenderer = std::make_shared<ren::ShadowMappingRenderer>(
      ren_directory, shadow_width, shadow_height);
  auto svrenderer = std::make_shared<ren::ShadowVolumeRenderer>(
      ren_directory, transformations);
  auto materialrenderer =
      std::make_shared<ren::MaterialRenderer>(ren_directory);
  auto raytracing_renderer =
      std::make_shared<ren::RayTracingRenderer>(ren_directory);

  std::shared_ptr<ren::Renderer> current_renderer = smrenderer;

  RenderIndex current_render_index = RenderIndex::simple_shadow_mapping;
  auto new_render_index = current_render_index;

  bool pause_scene = false;
  bool benchmark = false;
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
      case RenderIndex::raytracing:
        current_renderer = raytracing_renderer;
        break;
      }
    }

    auto start = std::chrono::system_clock::now();
    current_renderer->render(scene, transformations, ticks);
    auto end = std::chrono::system_clock::now();
    
    auto const elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    window.poll_events();
    window.exec_keymap();
    if (!debug)
      cam->rotate_offset(window.get_cursor_pos());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    if (benchmark) {
      if (current_render_index == RenderIndex::raytracing) {
       ren::Log::the().add_log("Render of a frame took: %zu us\n", raytracing_renderer->elapsed()); 
      } else 
       ren::Log::the().add_log("Render of a frame took: %zu us\n", elapsed.count()); 
      benchmark = false;
    }

    auto frametime = glfwGetTime() - ticks;
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Text("frametime: %f", frametime);
    ImGui::BulletText("W/A/S/D - Movement");
    ImGui::BulletText("E/Q - Up/Down");
    ImGui::BulletText("F1 - toggle debug");

    ren::Log::the().draw("Log");

    ImGui::Begin("Renderer");
    ImGui::Checkbox("Puase scene", &pause_scene);
    ImGui::Checkbox("Benchmark", &benchmark);
    const char *items[] = {"Simple Shadow Mapping", "Shadow Volume",
                           "RayTracing"};
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
  raytracing_renderer->destroy();
  ren::Log::destroy();
}
