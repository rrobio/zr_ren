#pragma once

#include "../renderer.hpp"

#include <future>
#include <thread>

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"

#include "raytracing/hittable_list.hpp"

namespace ren {

using Point = glm::vec3;

class RayTracingRenderer final : public Renderer {
public:
  RayTracingRenderer(std::filesystem::path root_dir);
  ~RayTracingRenderer() = default;
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override;

private:
  ren::Shader m_fstexture_shader{};

  Shader m_material_shader;
  Shader m_solid_shader;

  hittable_list m_world{};

  size_t m_n_threads{1};
  std::vector<std::thread> m_threads;
  std::vector<std::future<std::vector<uint8_t>>> m_futures;

  double const aspect_ratio = 16.0 / 9.0;
  size_t const image_width = 400;
  size_t const image_height = static_cast<int>(image_width / aspect_ratio);
  size_t const samples_per_pixel = 100;
  int const max_depth = 50;

  // auto R = cos(pi / 4);

  bool m_rendering{false};
  bool m_has_render{false};

  void render_frame();
};

} // namespace ren