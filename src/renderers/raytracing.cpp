#include "raytracing.hpp"

#include "../log.hpp"
#include "../scene.hpp"

#include <iostream>

#include "../util.hpp"

#include "../camera.hpp"
#include "../material.hpp"
#include "raytracing/color.hpp"
// #include "raytracing/sphere.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace ren {
RayTracingRenderer::RayTracingRenderer(std::filesystem::path root_dir) {
//   m_material_shader = ren::Shader(root_dir / "shaders/material.vert",
//                                   root_dir / "shaders/material.frag");
//   assert(m_material_shader.success());
//   m_solid_shader = ren::Shader(root_dir / "shaders/solid_color.vert",
//                                root_dir / "shaders/solid_color.frag");
//   assert(m_solid_shader.success());

//   auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
//   auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
//   auto material_left = std::make_shared<dielectric>(1.5);
//   auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

//   m_world.add(
//       make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
//   m_world.add(
//       make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
//   m_world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
//   m_world.add(
//       make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
//   m_world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
//   m_world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 100, material_left));
}

void RayTracingRenderer::render(const Scene &scene,
                                Transformations const &trans, double ticks) {

  // if (m_rendering) {
  //   if (is_render_done()) {
  //     create_image_data();
  //     m_rendering = false;
  //   }
  // }
  // // auto trans = scene.transformations();

  // auto const &light = scene.lights()[0];
  // auto const light_pos =
  //     glm::vec3(light.model()[3]); // get the translation vector

  // glViewport(0, 0, trans.screen_width, trans.screen_height);
  // glEnable(GL_DEPTH_TEST);
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // m_material_shader.use();
  // m_material_shader.set<glm::mat4>("projection", trans.projection);
  // m_material_shader.set<glm::mat4>("view", trans.view);

  // auto const &l_mat = light.material();
  // assert(l_mat != nullptr);
  // m_material_shader.set<glm::vec3>("light.ambient", l_mat->ambient);
  // m_material_shader.set<glm::vec3>("light.diffuse", l_mat->diffuse);
  // m_material_shader.set<glm::vec3>("light.specular", l_mat->specular);
  // m_material_shader.set<glm::vec3>("light.position", light_pos);

  // for (auto const &obj : scene.objects()) {
  //   auto const &mat = obj.material();
  //   assert(mat != nullptr);
  //   m_material_shader.set<glm::mat4>("model", obj.model());
  //   // std::cout << glm::to_string(mat->ambient) << std::endl;
  //   m_material_shader.set<glm::vec3>("material.ambient", mat->ambient);
  //   m_material_shader.set<glm::vec3>("material.diffuse", mat->diffuse);
  //   m_material_shader.set<glm::vec3>("material.specular", mat->specular);
  //   m_material_shader.set<float>("material.shininess", mat->shininess);
  //   obj.draw();
  // }

  // m_solid_shader.use();
  // m_solid_shader.set<glm::mat4>("projection", trans.projection);
  // m_solid_shader.set<glm::mat4>("view", trans.view);
  // m_solid_shader.set<glm::mat4>("model", light.model());
  // m_solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
  // scene.lights().front().draw();
}

void RayTracingRenderer::draw_dialog() {
  ImGui::Text("RayTracing");
  if (!m_rendering) {
    if (ImGui::Button("Render Frame")) {
      if (!m_rendering)
        render_frame();
    }
  }

  ImGui::InputInt("Number of threads", &m_n_threads);
  ImGui::InputInt("Max Depth", &max_depth);
  ImGui::InputInt("Samples Per Pixle", &samples_per_pixel);

  if (m_has_render) {
    if (ImGui::Button("Save to file")) {
      // save_to_file();
    }
    ImGui::Begin("Rendered Frame");
    m_texture.bind();
    ImGui::Image((ImTextureID)(intptr_t)m_texture.id,
                 ImVec2(m_texture.width, m_texture.height));
    ImGui::End();
  }
}

color ray_color(ray const &r, hittable const &world, int depth) {
//   hit_record rec;

//   if (depth <= 0)
//     return color(0, 0, 0);

//   if (world.hit(r, 0.001, infinity, rec)) {
//     // point3 target = rec.p + rec.normal + random_in_unit_sphere();
//     // point3 target = rec.p + rec.normal +
//     // random_in_hemisphere(rec.normal); point3 target = rec.p + rec.normal
//     // + random_unit_vector(); return 0.5 * ray_color(ray(rec.p, target -
//     // rec.p), world, depth-1);
//     ray scattered;
//     color attenuation;
//     if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
//       return attenuation * ray_color(scattered, world, depth - 1);
//     return color(0, 0, 0);
//   }
//   vec3 unit_direction = unit_vector(r.direction());
//   auto t = 0.5 * (unit_direction.y() + 1.0);
//   return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void RayTracingRenderer::render_frame() {
//   Log::the().add_log("Starting Raytracing\n");
//   Log::the().add_log("Width=%zu, Height=%zu\n", m_image_width, m_image_height);
//   Log::the().add_log("Threads=%zu\n", m_n_threads);

//   camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20,
//              aspect_ratio);

//   struct RenderInfo {
//     std::size_t image_height;
//     std::size_t image_width;
//     int samples_per_pixel;
//     int max_depth;
//   };
//   RenderInfo ri{m_image_height, m_image_width, samples_per_pixel, max_depth};
//   auto render_task = [](hittable_list const world, camera const cam,
//                         RenderInfo const ri, std::size_t start,
//                         std::size_t stop) {
//     std::vector<uint8_t> ret;
//     std::size_t const len = (stop - start) * ri.image_width * 3;
//     ret.resize(len);
//     for (std::size_t j = start; j < stop; ++j) {
//       for (std::size_t i = 0; i < ri.image_width; ++i) {
//         color pixel_color(0, 0, 0);
//         for (std::size_t s = 0; s < ri.samples_per_pixel; ++s) {
//           auto u = (i + random_double()) / (ri.image_width - 1);
//           auto v = (j + random_double()) / (ri.image_height - 1);
//           ray r = cam.get_ray(u, v);
//           pixel_color += ray_color(r, world, ri.max_depth);
//         }
//         std::size_t const index = ri.image_width * j + (i * 3);
//         ret.at(index + 0) = pixel_color.x();
//         ret.at(index + 1) = pixel_color.y();
//         ret.at(index + 2) = pixel_color.z();
//       }
//     }
//     return ret;
//   };

//   m_futures.clear();
//   std::size_t step = m_image_height / m_n_threads;
//   std::size_t cur_step = 0;
//   for (std::size_t i = 0; i < m_n_threads; i++) {
//     auto start = cur_step;
//     auto stop = cur_step + step;
//     if (stop > m_image_height) {
//       stop = m_image_height;
//     }
//     // if it dosen't divide evenly get the rest
//     if (i == m_n_threads - 1 && stop <= m_image_height) {
//       stop = m_image_height;
//     }

//     m_futures.emplace_back(std::async(std::launch::async, render_task, m_world,
//                                       cam, ri, start, stop));
//     cur_step += step;
//   }

//   m_rendering = true;
}

bool RayTracingRenderer::is_render_done() {
//   using namespace std::chrono_literals;
//   for (auto &f : m_futures) {
//     auto status = f.wait_for(0ms);
//     if (status != std::future_status::ready) {
//       return false;
//     }
//   }

//   m_has_render = true;
//   return true;
}

void RayTracingRenderer::create_image_data() {
//   pixels.clear();
//   // pixels.reserve(m_len);
//   for (auto &f : m_futures) {
//     auto const app = f.get();
//     pixels.insert(pixels.end(), app.begin(), app.end());
//   }
//   m_texture.generate_from_data(pixels, m_image_width, m_image_height);
//   m_futures.clear();
// }
// void RayTracingRenderer::save_to_file() {
//   assert(m_has_render);
//   stbi_write_png("./render.png", m_image_width, m_image_height, m_channels,
//                  pixels.data(), m_image_width * m_channels);
}
} // namespace ren
