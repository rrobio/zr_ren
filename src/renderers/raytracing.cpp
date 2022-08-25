#include "raytracing.hpp"

#include "../log.hpp"
#include "../scene.hpp"

#include <iostream>

#include "../util.hpp"

#include "raytracing/camera.hpp"
#include "raytracing/color.hpp"
#include "raytracing/material.hpp"
#include "raytracing/sphere.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ren {
RayTracingRenderer::RayTracingRenderer(std::filesystem::path root_dir) {
  m_material_shader = ren::Shader(root_dir / "shaders/material.vert",
                                  root_dir / "shaders/material.frag");
  assert(m_material_shader.success());
  m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
                               root_dir / "shaders/solid_color.frag");
  assert(m_solid_shader.success());

  auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
  auto material_left = std::make_shared<dielectric>(1.5);
  auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

  m_world.add(
      make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
  m_world.add(
      make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
  m_world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  m_world.add(
      make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
  m_world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
  m_world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 100, material_left));
}

void RayTracingRenderer::render(const Scene &scene,
                                Transformations const &trans, double ticks) {

  // auto trans = scene.transformations();

  auto const &light = scene.lights()[0];
  auto const light_pos =
      glm::vec3(light.model()[3]); // get the translation vector

  glViewport(0, 0, trans.screen_width, trans.screen_height);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_material_shader.use();
  m_material_shader.set<glm::mat4>("projection", trans.projection);
  m_material_shader.set<glm::mat4>("view", trans.view);

  auto const &l_mat = light.material();
  assert(l_mat != nullptr);
  m_material_shader.set<glm::vec3>("light.ambient", l_mat->ambient);
  m_material_shader.set<glm::vec3>("light.diffuse", l_mat->diffuse);
  m_material_shader.set<glm::vec3>("light.specular", l_mat->specular);
  m_material_shader.set<glm::vec3>("light.position", light_pos);

  for (auto const &obj : scene.objects()) {
    auto const &mat = obj.material();
    assert(mat != nullptr);
    m_material_shader.set<glm::mat4>("model", obj.model());
    // std::cout << glm::to_string(mat->ambient) << std::endl;
    m_material_shader.set<glm::vec3>("material.ambient", mat->ambient);
    m_material_shader.set<glm::vec3>("material.diffuse", mat->diffuse);
    m_material_shader.set<glm::vec3>("material.specular", mat->specular);
    m_material_shader.set<float>("material.shininess", mat->shininess);
    obj.draw();
  }

  m_solid_shader.use();
  m_solid_shader.set<glm::mat4>("projection", trans.projection);
  m_solid_shader.set<glm::mat4>("view", trans.view);
  m_solid_shader.set<glm::mat4>("model", light.model());
  m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  scene.lights().front().draw();
}

void RayTracingRenderer::draw_dialog() {
  ImGui::Text("RayTracing");
  if (ImGui::Button("Render Frame")) {
    render_frame();
  }
}

color ray_color(ray const &r, hittable const &world, int depth) {
  hit_record rec;

  if (depth <= 0)
    return color(0, 0, 0);

  if (world.hit(r, 0.001, infinity, rec)) {
    // point3 target = rec.p + rec.normal + random_in_unit_sphere();
    // point3 target = rec.p + rec.normal +
    // random_in_hemisphere(rec.normal); point3 target = rec.p + rec.normal
    // + random_unit_vector(); return 0.5 * ray_color(ray(rec.p, target -
    // rec.p), world, depth-1);
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return color(0, 0, 0);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void RayTracingRenderer::render_frame() {
  Log::the().add_log("Starting Raytracing\n");
  Log::the().add_log("Width=%zu, Height=%zu\n", image_width, image_height);
  Log::the().add_log("Threads=%zu\n", m_n_threads);

  camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20,
             aspect_ratio);

  std::vector<std::vector<color>> colors(image_width,
                                         std::vector<color>(image_height));
  size_t const channels = 3;
  size_t const len = image_width * image_height * channels;
  auto *pixels = new uint8_t[len];

  // std::packaged_task<std::vector<uint8_t>()> task([]{return
  // std::vector{1};});

  auto render_thread = [this, &cam, &colors](size_t start, size_t stop) {
    for (size_t j = start; j < stop; ++j) {
      for (size_t i = 0; i < image_width; ++i) {
        color pixel_color(0, 0, 0);
        for (size_t s = 0; s < samples_per_pixel; ++s) {
          auto u = (i + random_double()) / (image_width - 1);
          auto v = (j + random_double()) / (image_height - 1);
          ray r = cam.get_ray(u, v);
          pixel_color += ray_color(r, m_world, max_depth);
        }
        colors.at(i).at(j) = pixel_color;
      }
    }
  };

  struct RenderInfo {
    size_t image_height;
    size_t image_width;
    size_t samples_per_pixel;
    int max_depth;
  };
  // RenderInfo ri { image_width, image_height, samples_per_pixel, max_depth };
  // auto render_task = [](hittable_list const &world, camera const &cam,
  //                       RenderInfo const &ri, size_t start, size_t stop) {
  //   std::vector<uint8_t> ret;
  //   ret.resize((stop-start)*ri.image_width*3);
  //   for (size_t j = start; j < stop; ++j) {
  //     for (size_t i = 0; i < ri.image_width; ++i) {
  //       color pixel_color(0, 0, 0);
  //       for (size_t s = 0; s < ri.samples_per_pixel; ++s) {
  //         auto u = (i + random_double()) / (ri.image_width - 1);
  //         auto v = (j + random_double()) / (ri.image_height - 1);
  //         ray r = cam.get_ray(u, v);
  //         pixel_color += ray_color(r, world, ri.max_depth);
  //       }
  //       // ret[j*i] = pixel_color;
  //     }
  //   }
  //   return ret;
  // };

  // m_threads.clear();
  // size_t step = image_height / m_n_threads;
  // size_t cur_step = 0;
  // for (size_t i = 0; i < m_n_threads; i++) {
  //   auto start = cur_step;
  //   auto stop = cur_step + step;
  //   if (stop > image_height)
  //     stop = image_height;
  //   m_threads.push_back(std::thread(render_thread, start, stop));
  //   cur_step += step;
  // }

  m_threads.clear();
  size_t step = image_height / m_n_threads;
  size_t cur_step = 0;
  for (size_t i = 0; i < m_n_threads; i++) {
    auto start = cur_step;
    auto stop = cur_step + step;
    if (stop > image_height)
      stop = image_height;
    m_threads.push_back(std::thread(render_thread, start, stop));
    cur_step += step;
  }

  m_rendering = true;

  for (auto &t : m_threads)
    t.join();

  size_t index = 0;
  for (size_t i = 0; i < image_height; i++) {
    for (size_t j = 0; j < image_width; j++) {
      auto [x, y, z] = get_pixel_tuple(colors.at(j).at(i), samples_per_pixel);
      pixels[index++] = x;
      pixels[index++] = y;
      pixels[index++] = z;
    }
  }

  stbi_write_png("./render.png", image_width, image_height, channels, pixels,
                 image_width * channels);

  delete[] pixels;
}
} // namespace ren
