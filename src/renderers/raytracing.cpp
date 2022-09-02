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
RayTracingRenderer::RayTracingRenderer(std::filesystem::path root_dir) {
  auto ren_material_ground = std::make_shared<ren::Material>(
      std::make_shared<ren::lambertian>(color(0.8, 0.8, 0.0)));
  auto ren_material_center = std::make_shared<ren::Material>(
      std::make_shared<ren::lambertian>(color(0.1, 0.2, 0.5)));
  auto ren_material_left =
      std::make_shared<ren::Material>(std::make_shared<ren::dielectric>(1.5));
  auto ren_material_right = std::make_shared<ren::Material>(
      std::make_shared<ren::metal>(color(0.8, 0.6, 0.2), 0.0));

  m_scene.add_object(
      ren::create_sphere(point3(0.0, -100.5, -1.0), 100, ren_material_ground));
  m_scene.add_object(
      ren::create_sphere(point3(0.0, 0.0, -1.0), 0.5, ren_material_center));
  m_scene.add_object(
      ren::create_sphere(point3(-1.0, 0.0, -1.0), 0.5, ren_material_left));
  m_scene.add_object(
      ren::create_sphere(point3(-1.0, 0.0, -1.0), -0.45, ren_material_left));
  m_scene.add_object(
      ren::create_sphere(point3(1.0, 0.0, -1.0), 0.5, ren_material_right));
}

void RayTracingRenderer::render(const Scene &scene,
                                Transformations const &trans, double ticks) {

  assert(trans.cam);
  if (!a_camera) a_camera = trans.cam;
  if (m_rendering) {
    if (is_render_done()) {
      create_image_data();
      m_rendering = false;
      m_has_render = true;
      thread_cleanup();
    }
  }
}

void RayTracingRenderer::draw_dialog() {
  ImGui::Text("RayTracing");
  if (!m_rendering) {
    if (ImGui::Button("Render Frame")) {
      render_frame(&m_scene);
    }
  }

  ImGui::InputInt("Number of threads", &m_n_threads);
  ImGui::InputInt("Max Depth", &m_max_depth);
  ImGui::InputInt("Samples Per Pixle", &m_samples_per_pixel);

  if (m_has_render) {
    if (ImGui::Button("Save to file")) {
      save_to_file();
    }
    ImGui::Begin("Rendered Frame");
    m_texture.bind();
    ImGui::Image((ImTextureID)(intptr_t)m_texture.id,
                 ImVec2(m_texture.width, m_texture.height));
    ImGui::End();
  }
}

static bool hit_scene(ray const &r, Scene  const *world, int depth, hit_record &rec) {

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
  return hit_anything;
}

static color ren_ray_color(ray const &r, Scene const *world, int depth) {
  hit_record rec;

  if (depth <= 0)
    return color(0, 0, 0);

  if (hit_scene(r, world, depth, rec)) {
    // point3 target = rec.p + rec.normal + random_in_unit_sphere();
    // point3 target = rec.p + rec.normal +
    // random_in_hemisphere(rec.normal); point3 target = rec.p + rec.normal
    // + random_unit_vector(); return 0.5 * ray_color(ray(rec.p, target -
    // rec.p), world, depth-1);
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter->scatter(r, rec, attenuation, scattered))
      return attenuation * ren_ray_color(scattered, world, depth - 1);
    return color(0, 0, 0);
  }
  vec3 unit_direction = glm::normalize(r.direction());
  float t = 0.5 * (unit_direction.y + 1.0);
  return (1.0f - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

struct RenderTaskArgs {
  std::shared_ptr<Camera> cam;
  size_t image_height;
  size_t image_width;
  int samples_per_pixel;
  int max_depth;
  int start;
  int stop;
};

  int index = 0;
  int index_offset = ra.start * ra.image_width * 3;
void ren_task(RenderTaskArgs ra, Scene const *scene, Pixels *pixels) {
  // for (int j = ra.stop; j >= ra.start; --j) {
  for (int j = ra.start; j <= ra.stop; j++) {
    for (int i = 0; i < ra.image_width; ++i) {
      color pixel_color(0, 0, 0);
      for (int s = 0; s < ra.samples_per_pixel; ++s) {
        auto u = (i + random_float()) / (ra.image_width - 1);
        auto v = (j + random_float()) / (ra.image_height - 1);
        ray r = ra.cam.get_ray(u, v);

        pixel_color += ren_ray_color(r, scene, ra.max_depth);
      }
      auto [x, y, z] = get_pixel_tuple(pixel_color, ra.samples_per_pixel);
      (*pixels).at(index_offset + index++) = x;
      (*pixels).at(index_offset + index++) = y;
      (*pixels).at(index_offset + index++) = z;
    }
  }
};

void RayTracingRenderer::render_frame(Scene const *scene) {
  Log::the().add_log("Starting Raytracing\n");
  Log::the().add_log("Width=%zu, Height=%zu\n", m_image_width, m_image_height);
  Log::the().add_log("Threads=%d\n", m_n_threads);

  // Camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 90,
  //            aspect_ratio);
  assert(a_camera);
  a_camera->update_rt_vectors();

  m_pixels.clear();
  m_pixels.resize(m_len);

  m_threads.clear();
  for (auto &f : m_thread_finished)
    f = false;
  std::size_t step = m_image_height / m_n_threads;
  std::size_t cur_step = 0;
  for (std::size_t i = 0; i < m_n_threads; i++) {
    int start = cur_step;
    int stop = cur_step + step -1;
    if (stop >= m_image_height) {
      stop = m_image_height - 1;
    }
    // if it dosen't divide evenly get the rest
    if (i == m_n_threads - 1 && stop < m_image_height) {
      stop = m_image_height - 1;
    }
    RenderTaskArgs ra{
        a_camera,         m_image_height, m_image_width, m_samples_per_pixel,
        m_max_depth, start,          stop};
    auto call = [this, i](RenderTaskArgs ra, Scene const *s, Pixels *pixels) {
      ren_task(ra, s, pixels);
      m_thread_finished.at(i) = true;
    };
    m_threads.emplace_back(call, ra, scene, &m_pixels);
    cur_step += step;
  }

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
  m_texture.generate_from_data(m_pixels, m_image_width, m_image_height);
}
void RayTracingRenderer::save_to_file() {
  assert(m_has_render);
  stbi_write_png("./render.png", m_image_width, m_image_height, m_channels,
                 m_pixels.data(), m_image_width * m_channels);
}
} // namespace ren
