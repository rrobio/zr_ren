#pragma once

#include "../renderer.hpp"

#include <atomic>
#include <thread>
#include <array>

// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include "../shader.hpp"
#include "../texture.hpp"


namespace ren {

using Pixels = std::vector<uint8_t>;

class RayTracingRenderer final : public Renderer {
public:
  RayTracingRenderer(std::filesystem::path root_dir);
  ~RayTracingRenderer() {
    thread_cleanup();
  };
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override;

private:
  Shader m_fstexture_shader{};

  Shader m_material_shader;
  Shader m_solid_shader;

  int m_n_threads{1};
  std::vector<std::thread> m_threads{};
  std::array<std::atomic<bool>, 16> m_thread_finished{};
  bool threads_finished() {
    for (size_t i = 0; i < m_n_threads; ++i) {
      if (!m_thread_finished.at(i)) return false;
    }
    return true;
  }

  double const aspect_ratio = 16.0 / 9.0;
  std::size_t const m_image_width = 400;
  std::size_t const m_image_height =
      255; // static_cast<std::size_t>(m_image_width / aspect_ratio);
  std::size_t const m_channels = 3;
  std::size_t const m_len = m_image_width * m_image_height * m_channels;
  int m_samples_per_pixel = 100;
  int m_max_depth = 50;

  Pixels m_pixels{};
  Texture m_texture{};

  // auto R = cos(pi / 4);
  Scene m_scene;

  bool m_rendering{false};
  bool m_has_render{false};

  void render_frame(Scene*);
  bool is_render_done();
  void create_image_data();
  void save_to_file();
  void thread_cleanup(); 
};

} // namespace ren