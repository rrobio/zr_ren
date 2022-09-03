#pragma once

#include <algorithm>

#include "glm/glm.hpp"

#include "log.hpp"
#include "ray.hpp"
#include "vec3.hpp"

namespace ren {

class Camera {
public:
  Camera(vec3 look_from, point3 look_at, vec3 vup, float vfov,
         float const aspect_ratio)
      : m_position(look_from), m_up(vup), m_world_up(vup), m_fov(vfov) {
    auto theta = glm::radians(vfov);
    auto h = std::tan(theta / 2);
    m_viewport_height = 2.0 * h;
    m_viewport_width = aspect_ratio * m_viewport_height;

    auto w = glm::normalize(look_from - look_at);
    auto u = glm::normalize(cross(vup, w));
    auto v = cross(w, u);

    origin = look_from;
    horizontal = m_viewport_width * u;
    vertical = m_viewport_height * v;
    lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - w;
    update_vectors();
  }

  auto view() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
  }
  
  auto fov() const { return m_fov; }
  auto set_fov(float f) { m_fov = f; }

  ray get_ray(float s, float t) const {
    return ray(origin,
               lower_left_corner + s * horizontal + t * vertical - origin);
  }
  auto pos() const { return m_position; }

  void move(glm::vec3 const m, float const speed) {
    if (m.x != 0)
      m_position += m_right * m.x * speed;
    if (m.z != 0)
      m_position += m_front * m.z * speed;
    if (m.y != 0)
      m_position += m_up * m.y * speed;
  }

  void rotate(float y_off, float p_off, float speed) {
    m_yaw += y_off * speed;
    m_pitch -= p_off * speed;

    if (m_pitch > 89.0f)
      m_pitch = 89.0f;
    if (m_pitch < -89.0f)
      m_pitch = -89.0f;

    update_vectors();
  }

  void rotate_offset(std::tuple<double, double> xy) {
    auto const [x, y] = xy;
    auto const xoffset = x - m_last_x;
    auto const yoffset = y - m_last_y;
    m_last_x = x;
    m_last_y = y;

    this->rotate(xoffset, yoffset, 0.1f);
  }

  void update_rt_vectors() {
    auto w = glm::normalize(-m_front);
    auto u = glm::normalize(cross(m_up, w));
    auto v = cross(w, u);

    origin = m_position;
    horizontal = m_viewport_width * u;
    vertical = m_viewport_height * v;
    lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - w;
  }

private:
  float m_fov;
  
  float m_yaw{-90.0f};
  float m_pitch{0.0f};

  point3 m_position{};
  vec3 m_front{};
  vec3 m_up{};
  vec3 m_right{};
  vec3 m_world_up{};

  double m_last_x{};
  double m_last_y{};

  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  // vec3 u, v, w;
  
  float m_viewport_height;
  float m_viewport_width;

  void update_vectors() {
    glm::vec3 front;
    front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    front.y = glm::sin(glm::radians(m_pitch));
    front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up = glm::normalize(glm::cross(m_right, m_front));
  }
};
} // namespace ren
