#pragma once

#include "../renderer.hpp"

#include <future>
#include <thread>

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"
#include "../texture.hpp"


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

  int m_n_threads{2};
  std::vector<std::thread> m_threads;
  std::vector<std::future<std::vector<uint8_t>>> m_futures;

  double const aspect_ratio = 16.0 / 9.0;
  std::size_t const m_image_width = 400;
  std::size_t const m_image_height =
      255; // static_cast<std::size_t>(m_image_width / aspect_ratio);
  std::size_t const m_channels = 3;
  std::size_t const m_len = m_image_width * m_image_height * m_channels;
  int samples_per_pixel = 100;
  int max_depth = 50;

  std::vector<uint8_t> pixels{};
  ren::Texture m_texture{};

  // auto R = cos(pi / 4);

  bool m_rendering{false};
  bool m_has_render{false};

  void render_frame();
  bool is_render_done();
  void create_image_data();
  void save_to_file();
};

} // namespace ren