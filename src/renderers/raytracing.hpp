#pragma once

#include "../renderer.hpp"

#include <array>
#include <atomic>
#include <thread>

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
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    destroy_realtime();
  };
  void render(const Scene &, Transformations const &, double ticks) override;
  void draw_dialog() override;

  struct RenderTaskArgs {
    std::shared_ptr<Camera> cam;
    size_t image_height;
    size_t image_width;
    int samples_per_pixel;
    int max_depth;
    int start;
    int stop;
  };

  enum ThreadTaskType {
    normal,
    realtime,
  };
  struct ThreadTask {
    ThreadTaskType type;
    Pixels *pixels;
    int index;
    std::atomic<bool> *thread_finished;
    std::atomic<bool> *should_finish;
    std::atomic<bool> *should_pause;
  };

private:
  Shader m_fstexture_shader{};

  Shader m_material_shader;
  Shader m_solid_shader;

  using Bounds = std::tuple<int, int>;
  void setup_threads(ThreadTask, Scene const *);

  int m_n_threads{1};
  std::vector<std::thread> m_threads{};
  std::array<std::atomic<bool>, 16> m_thread_finished{};
  bool threads_finished() {
      for (size_t i = 0; i < m_n_threads; ++i) {
        if (!m_thread_finished.at(i))
          return false;
      }
      return true;
  }
  bool realtime_threads_finished() {
      for (size_t i = 0; i < m_realtime_n_threads; ++i) {
        if (!m_realtime_thread_finished.at(i))
          return false;
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
  Scene const *a_scene;
  std::shared_ptr<Camera> a_camera;

  bool m_rendering{false};
  bool m_has_render{false};

  bool m_realtime_setup{false};
  void setup_realtime();
  void destroy_realtime();
  GLuint VBO, VAO, EBO;
  GLuint m_framebuffer;
  bool m_render_realtime{false};
  int m_realtime_samples_per_pixel = 5;
  int m_realtime_max_depth = 25;
  // double buffer
  Pixels m_realtime_pixels{};
  Texture m_realtime_texture{};

  int m_realtime_n_threads{1};
  std::vector<std::thread> m_realtime_threads{};
  // double buffer
  std::array<std::atomic<bool>, 16> m_realtime_thread_finished{false};
  std::array<std::atomic<bool>, 16> m_realtime_thread_should_stop{false};
  std::atomic<bool> m_should_pause{false};
  
  void rt_pause(){m_should_pause = true;}
  void rt_unpause(){
    for (size_t i = 0; i < m_realtime_n_threads; i++) {
      m_realtime_thread_finished.at(i) = false;
    }
    m_should_pause = false;
  }

  void render_frame(Scene const *);
  bool is_render_done();
  void create_image_data();
  void rt_create_image_data();
  void save_to_file();
  void thread_cleanup();
};

} // namespace ren