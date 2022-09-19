#include "raytracing.hpp"

#include "../log.hpp"
#include "../scene.hpp"

#include <iostream>

#include "../util.hpp"

#include "../camera.hpp"
#include "../color.hpp"
#include "../material.hpp"
// #include "raytracing/sphere.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ren {
// raytracing
static bool hit_scene(ray const &r, Scene const *world, int depth,
                      hit_record &rec) {

  hit_record temp_rec;
  float t_min = 0.001;
  float t_max = infinity;
  bool hit_anything = false;
  auto closest_so_far = t_max;
  for (auto const &object : world->objects()) {
    if (!object.hit()) {
      continue;
    }
    if (object.hit()(object, r, t_min, closest_so_far, temp_rec)) { // malo
                                                                    // ruzno
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  for (auto const &object : world->lights()) {
    if (!object.hit()) {
      continue;
    }
    if (object.hit()(object, r, t_min, closest_so_far, temp_rec)) { // malo
                                                                    // ruzno
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}

static color ren_ray_color(ray const &r, Scene const *world, int depth) {
  hit_record rec;

  if (depth <= 0)
    return color(0, 0, 0);

  if (!hit_scene(r, world, depth, rec)) {
    return color(0.2f, 0.2f, 0.2f);
  }

  ray scattered;
  color albedo;
  color emitted = rec.mat_ptr->scatter->emitted();
  float pdf;

  if (!rec.mat_ptr->scatter->scatter(r, rec, albedo, scattered, pdf))
    return emitted;

  auto &light = world->lights().at(0);
  auto on_light = light.translation();
  auto to_light = on_light - rec.p;
  auto distance_squared = glm::length2(to_light);
  to_light = glm::normalize(to_light);

  if (glm::dot(to_light, rec.normal) < 0)
    return emitted;

  float light_area = 2048.f * light.scale().x;
  auto light_cosine = fabs(to_light.y);
  if (light_cosine < 0.000001)
    return emitted;

  pdf = distance_squared / (light_cosine * light_area);
  scattered = ray(rec.p, to_light);
  return emitted + albedo *
                       rec.mat_ptr->scatter->scattering_pdf(r, rec, scattered) *
                       ren_ray_color(scattered, world, depth - 1) / pdf;
}

void ren_task(RayTracingRenderer::ThreadTask task,
              RayTracingRenderer::RenderTaskArgs ra, Scene const *scene) {
  auto image_height = ra.image_height;
  auto image_width = ra.image_width;
  int const len = image_height * image_width * 3;
  int samples_per_pixel = ra.samples_per_pixel;
  int max_depth = ra.max_depth;
  int starting_index = ra.start * image_width * 3;
  int index = starting_index;

  assert(task.pixels);
  Pixels *pixels = task.pixels;

  auto type = task.type;
  do {
    if (type == RayTracingRenderer::ThreadTaskType::realtime) {
      ra.cam->update_rt_vectors();
      if (*(task.should_finish)) {
        return;
      }
      if (*task.thread_finished || *task.should_pause) {
        std::this_thread::yield();
        continue;
      }
    }
    for (int j = ra.start; j <= ra.stop; ++j) {
      for (int i = 0; i < image_width; ++i) {
        if (type == RayTracingRenderer::ThreadTaskType::realtime) {
          if (*task.should_finish) {
            return;
          }
          if (*task.should_pause) {
            do {
              std::this_thread::yield();
            } while (*task.should_pause);
          }
        }
        color pixel_color(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
          auto u = (i + random_float()) / (image_width - 1);
          auto v = (j + random_float()) / (image_height - 1);
          ray r = ra.cam->get_ray(u, v);
          pixel_color += ren_ray_color(r, scene, max_depth);
        }
        auto [x, y, z] = get_pixel_tuple(pixel_color, samples_per_pixel);
        (*pixels).at(index++) = x;
        (*pixels).at(index++) = y;
        (*pixels).at(index++) = z;
      }
    }
    index = starting_index;
    *task.thread_finished = true;
  } while (task.type == RayTracingRenderer::ThreadTaskType::realtime);
};

RayTracingRenderer::RayTracingRenderer(std::filesystem::path root_dir) {
  m_fstexture_shader = ren::Shader(root_dir / "shaders/fstexture.vert",
                                   root_dir / "shaders/fstexture.frag");
  assert(m_fstexture_shader.success());

  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  float vertices[] = {
      // positions        // texture coords
      1.f,  1.f,  0.0f, 1.0f, 1.0f, // top right
      1.f,  -1.f, 0.0f, 1.0f, 0.0f, // bottom right
      -1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
      -1.f, 1.f,  0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texture coord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  m_realtime_pixels.resize(m_len);
  m_realtime_texture.generate_from_data(m_pixels, m_image_width,
                                        m_image_height);
  assert(m_realtime_texture.m_is_valid);
}

void RayTracingRenderer::render(const Scene &scene,
                                Transformations const &trans, double ticks) {

  assert(trans.cam);
  a_camera = trans.cam;
  a_scene = &scene;
  if (m_rendering) {
    if (is_render_done()) {
      create_image_data();
      m_rendering = false;
      m_has_render = true;
      thread_cleanup();
    }
  }
  if (m_render_realtime) {
    if (realtime_threads_finished()) {
      rt_pause();
      rt_create_image_data();
      rt_unpause();
    }
  }

  // realtime rendering
  if (m_has_render || m_render_realtime) {
    // reset settings when switching from other renderers
    glDrawBuffer(GL_BACK);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_fstexture_shader.use();
    m_fstexture_shader.set("render_texture", 0);

    if (m_render_realtime) {
      m_realtime_texture.bind(GL_TEXTURE0);
    } else {
      m_texture.bind(GL_TEXTURE0);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}

void RayTracingRenderer::setup_realtime() {
  if (m_realtime_setup)
    return;

  m_realtime_threads.clear();
  for (auto &b : m_realtime_thread_finished) {
    b = false;
  }
  for (auto &t : m_realtime_thread_should_stop) {
    t = false;
  }
  m_should_pause = false;

  m_realtime_pixels.resize(m_len);

  setup_threads(
      {ThreadTaskType::realtime, &m_realtime_pixels, 0, nullptr, nullptr},
      a_scene);

  Log::the().add_log("Realtime setup\n");
  m_realtime_setup = true;
}
void RayTracingRenderer::destroy_realtime() {
  if (!m_realtime_setup)
    return;

  for (auto &t : m_realtime_thread_finished) {
    t = false;
  }
  for (auto &t : m_realtime_thread_should_stop) {
    t = true;
  }
  m_should_pause = false;
  for (auto &t : m_realtime_threads) {
    t.join();
  }

  m_realtime_pixels.clear();
  m_realtime_threads.clear();

  Log::the().add_log("Realtime destroyed\n");
  m_realtime_setup = false;
}

void RayTracingRenderer::draw_dialog() {
  ImGui::Text("RayTracing");
  // if (m_render_realtime) {
  //   ImGui::BeginDisabled(true);
  // }
  ImGui::Checkbox("Real time rendering", &m_render_realtime);
  ImGui::InputInt("(RT) Number of threads", &m_realtime_n_threads);
  ImGui::InputInt("(RT) Max Depth", &m_realtime_max_depth);
  ImGui::InputInt("(RT) Samples Per Pixle", &m_realtime_samples_per_pixel);
  // if (m_render_realtime) {
  //   ImGui::EndDisabled();
  // }
  if (m_render_realtime) {
    setup_realtime();
  } else {
    destroy_realtime();
  }

  if (!m_rendering && !m_render_realtime) {
    if (ImGui::Button("Render Frame")) {
      render_frame(a_scene);
    }
  }

  if (!m_render_realtime) {
    ImGui::InputInt("Number of threads", &m_n_threads);
    ImGui::InputInt("Max Depth", &m_max_depth);
    ImGui::InputInt("Samples Per Pixle", &m_samples_per_pixel);

    if (m_has_render) {
      if (ImGui::Button("Save to file")) {
        save_to_file();
      }
      // ImGui::Begin("Rendered Frame");
      // // m_texture.bind();
      // ImGui::Image((ImTextureID)(intptr_t)m_texture.id,
      //              ImVec2(m_texture.width, m_texture.height));
      // ImGui::End();
    }
  }
}
void RayTracingRenderer::setup_threads(ThreadTask task, Scene const *scene) {
  auto stepper = [](int height, int n_threads) -> std::vector<Bounds> {
    std::vector<Bounds> ret;
    int step = height / n_threads;
    int cur_step = 0;
    for (int i = 0; i < n_threads; i++) {
      int start = cur_step;
      int stop = cur_step + step - 1;
      if (stop >= height) {
        stop = height - 1;
      }
      // if it dosen't divide evenly get the rest
      if (i == n_threads - 1 && stop < height) {
        stop = height - 1;
      }

      ret.push_back({start, stop});
      cur_step += step;
    }

    return ret;
  };

  int i = 0;
  RenderTaskArgs ra{a_camera, m_image_height, m_image_width, 0, 0, 0, 0};

  // thread function;
  auto call = [this](ThreadTask tt, RenderTaskArgs ra, Scene const *s,
                     int index) {
    tt.index = index;
    ren_task(tt, ra, s);
    // m_thread_finished.at(index) = true;
  };

  // pre thread creation setup
  if (task.type == ThreadTaskType::normal) {
    m_threads.clear();
  } else {
    m_realtime_threads.clear();
  }

  // create the threads
  int n_threads = 0;
  if (task.type == ThreadTaskType::normal) {
    n_threads = m_n_threads;
    ra.samples_per_pixel = m_samples_per_pixel;
    ra.max_depth = m_max_depth;
  } else {
    n_threads = m_realtime_n_threads;
    ra.samples_per_pixel = m_realtime_samples_per_pixel;
    ra.max_depth = m_realtime_max_depth;
  }

  for (auto const s : stepper(m_image_height, n_threads)) {
    auto [start, stop] = s;
    ra.start = start;
    ra.stop = stop;
    if (task.type == ThreadTaskType::normal) {
      task.thread_finished = &m_thread_finished.at(i);
      task.should_finish = nullptr;
      task.should_pause = nullptr;

      m_threads.emplace_back(call, task, ra, scene, i);
    } else {
      task.thread_finished = &m_realtime_thread_finished.at(i);
      task.should_finish = &m_realtime_thread_should_stop.at(i);
      task.should_pause = &m_should_pause;

      m_realtime_threads.emplace_back(call, task, ra, scene, i);
    }
    i++;
  }
}

void RayTracingRenderer::render_frame(Scene const *scene) {
  Log::the().add_log("Starting Render\n");
  Log::the().add_log("Width=%zu, Height=%zu\n", m_image_width, m_image_height);
  Log::the().add_log("Threads=%d\n", m_n_threads);

  assert(a_camera);
  a_camera->update_rt_vectors();

  m_pixels.clear();
  m_pixels.resize(m_len);

  setup_threads({ThreadTaskType::normal, &m_pixels, 0, nullptr, nullptr},
                scene);

  m_rendering = true;
}

bool RayTracingRenderer::is_render_done() { return threads_finished(); }
void RayTracingRenderer::thread_cleanup() {
  for (auto &thread : m_threads) {
    thread.join();
  }
  m_threads.clear();
  for (auto &f : m_thread_finished)
    f = false;
  m_has_render = true;
}

void RayTracingRenderer::create_image_data() {
  if (m_texture.m_is_valid) {
    m_texture.update_data(m_pixels, m_image_width, m_image_height);
  } else {
    m_texture.generate_from_data(m_pixels, m_image_width, m_image_height);
  }
}

void RayTracingRenderer::rt_create_image_data() {
  if (m_realtime_texture.m_is_valid) {
    m_realtime_texture.update_data(m_realtime_pixels, m_image_width,
                                   m_image_height);
  }
}
void RayTracingRenderer::save_to_file() {
  assert(m_has_render);
  stbi_flip_vertically_on_write(true);
  stbi_write_png("./render.png", m_image_width, m_image_height, m_channels,
                 m_realtime_pixels.data(), m_image_width * m_channels);
}
} // namespace ren
