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

// clang-format off
#include "window.hpp"
#include "input.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "resource_manager.hpp"
#include "texture.hpp"
#include "cubemap.hpp"
#include "log.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "util.hpp"

#include "renderers/solid.hpp"
#include "renderers/shadow_mapping.hpp"
#include "renderers/material.hpp"
// clang-format on

enum RenderIndex {
  simple_shadow_mapping = 0,
  material,
};

int const screen_width = 1024;
int const screen_height = 768;
// int const channel_count = 4;
float const screen_aspect =
    static_cast<float>(screen_width) / static_cast<float>(screen_height);
int const shadow_width = 1440;
int const shadow_height = 1440;
// float const shadow_aspect =
//     static_cast<float>(shadow_width) / static_cast<float>(shadow_height);

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
  // auto current_working_directory = std::filesystem::current_path();
  auto const ren_directory = get_root_directory();
  // auto const ren_directory = root_directory / "subprojects/ren";

  auto window = ren::Window("ren", screen_width, screen_height, false);
  window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  window.set_input_mode(GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  auto cam = ren::Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.f, 0.f, 0.f),
                         glm::vec3(0.0f, 1.0f, 0.0f), screen_aspect);

  ren::Scene scene{};
  scene.add_object(ren::create_plane());
  for (size_t i = 0; i < 10; i++) {
    scene.add_object(ren::create_sphere());
  }

  glEnable(GL_DEPTH_TEST);

  auto no_tex_img = ren::Image(ren_directory / "res/tex/no-tex.png");
  auto no_tex = ren::Texture(no_tex_img);

  ImGui::CreateContext();
  auto &io = ImGui::GetIO();

  ImGui_ImplGlfw_InitForOpenGL(window.get_ptr(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  ren::Log::init();

  auto plane_model = glm::scale(glm::mat4(1.0f), glm::vec3(15.f, 1.f, 15.f));
  plane_model = glm::translate(plane_model, glm::vec3(0.f, -5.f, 0.f));

  auto light_sphere = ren::create_sphere();

  // auto world_model = glm::scale(glm::mat4(1), glm::vec3(30.f, 30.f, 30.f));

  auto material = std::make_shared<ren::Material>();
  material->diffuse = glm::vec3(0.2f, 0.5f, 0.1f);

  scene.object_at(0)->set_model(plane_model);
  for (size_t i = 1; i < scene.size(); i++) {
    auto x = (random_double() * 2 - 1) * 5;
    auto y = (random_double() * 2 - 1) * 5;
    auto z = (random_double() * 2 - 1) * 5;
    auto cube_model = glm::translate(glm::mat4(1), glm::vec3(x, y, z));

    scene.object_at(i)->set_model(cube_model);
  }
  //  RT ---------------------
  // int const DATA_SIZE = screen_width * screen_height * channel_count;

  // GLuint pbo;
  // glGenBuffers(1, &pbo);
  // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
  // glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
  // void* mapped_buffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
  // glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  // GLubyte *image_data = new GLubyte[DATA_SIZE];

  // GLuint rt_tex;
  // glGenTextures(1, &rt_tex);
  // glBindTexture(GL_TEXTURE_2D, rt_tex);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screen_width, screen_height, 0,
  //              GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid *)image_data);
  // glBindTexture(GL_TEXTURE_2D, 0);

  // \RT ---------------------

  auto const speed = 0.05f;
  auto keymap = ren::Keymap{};
  keymap.set_bind(
      {GLFW_KEY_ESCAPE, [&window]() { window.set_should_close(true); }});
  keymap.set_bind({GLFW_KEY_W, [&cam, speed]() {
                     cam.move(glm::vec3(0.f, 0.f, 1.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_A, [&cam, speed]() {
                     cam.move(glm::vec3(-1.f, 0.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_S, [&cam, speed]() {
                     cam.move(glm::vec3(0.f, 0.f, -1.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_D, [&cam, speed]() {
                     cam.move(glm::vec3(1.f, 0.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_E, [&cam, speed]() {
                     cam.move(glm::vec3(0.f, 1.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_Q, [&cam, speed]() {
                     cam.move(glm::vec3(0.f, -1.f, 0.f), speed);
                   }});
  keymap.set_bind({GLFW_KEY_F1, GLFW_RELEASE,
                   [&window, &io]() { toggle_debug(window, io); }});
  // keymap->set_bind({GLFW_KEY_R, [&world, &image_data, &cam]() {
  //					  render_scene_rt(world,
  // cam, image_data);
  //                   }});
  window.set_keymap(keymap);


  auto solidrenderer = ren::SolidRenderer(ren_directory);
  auto smrenderer = std::make_shared<ren::ShadowMappingRenderer>(
      ren_directory, shadow_width, shadow_height);
  auto materialrenderer =
      std::make_shared<ren::MaterialRenderer>(ren_directory);

  ren::Renderer::Transformations transformations{
      1024,
      768,
      static_cast<float>(screen_width) / static_cast<float>(screen_height),
      glm::perspective(glm::radians(90.0f), screen_aspect, 0.1f, 100.f),
      cam.pos(),
      cam.view(),
  };

  std::shared_ptr<ren::Renderer> current_renderer = smrenderer;

  RenderIndex current_render_index = RenderIndex::simple_shadow_mapping;
  auto new_render_index = current_render_index;
  while (!window.should_close()) {
    glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto ticks = glfwGetTime();

    auto const light_pos =
        glm::vec3(glm::sin(ticks) * 3, 10.f, glm::cos(ticks) * 3);

    auto light_model = glm::translate(glm::mat4(1.f), light_pos);

    scene.light_at(0)->set_model(light_model);
    transformations.view = cam.view();

    if (current_render_index != new_render_index) {
      current_render_index = new_render_index;
      switch (current_render_index) {
      case RenderIndex::simple_shadow_mapping:
        current_renderer = smrenderer;
        break;
      case RenderIndex::material:
        current_renderer = materialrenderer;
        break;
      }
    }

    current_renderer->render(scene, transformations, ticks);

    window.poll_events();
    window.exec_keymap();
    if (!debug)
      cam.rotate_offset(window.get_cursor_pos());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto frametime = glfwGetTime() - ticks;
    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Text("frametime: %f", frametime);

    ren::Log::the().draw("Log");

    ImGui::Begin("Renderer");
    const char *items[] = {"Simple Shadow Mapping", "Material", "RayTracing"};
    static int combo_index = static_cast<int>(current_render_index);
    ImGui::Combo("Renderer", &combo_index, items, IM_ARRAYSIZE(items));
    new_render_index = static_cast<RenderIndex>(combo_index);
    // ren::Log::the().add_log("current %d\n", new_render_index);
    ImGui::Separator();
    current_renderer->draw_dialog();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    window.swap_buffers();
  }
  ren::Log::destroy();
  // delete[] image_data;
}
