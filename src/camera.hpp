#pragma once

#include <algorithm>

#include "glm/glm.hpp"

#include "log.hpp"
#include "ray.hpp"
#include "vec3.hpp"

namespace ren {

class Camera {
public:
  Camera(glm::vec3 look_from, glm::vec3 look_at, glm::vec3 vup, float vfov,
         float const aspect_ratio)
      : m_position(look_from), m_up(vup), m_world_up(vup) {
    auto theta = glm::radians(vfov);
    auto h = std::tan(theta / 2);
    float viewport_height = 2.0 * h;
    float viewport_width = aspect_ratio * viewport_height;

    auto w = glm::normalize(look_from - look_at);
    auto u = glm::normalize(cross(vup, w));
    auto v = cross(w, u);

    origin = look_from;
    horizontal = viewport_width * u;
    vertical = viewport_height * v;
    lower_left_corner = origin - horizontal / 2.0f - vertical / 2.0f - w;
    update_vectors();
  }

  auto view() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
  }

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
  ///	auto get_ray(float s, float t) const
  ///	{
  ///		return Ray(origin,
  ///				   lower_left_corner + s * horizontal + t *
  /// vertical
  ///- origin);
  ///	}

private:
  float m_yaw{-90.0f};
  float m_pitch{0.0f};

  glm::vec3 m_position{};
  glm::vec3 m_front{};
  glm::vec3 m_up{};
  glm::vec3 m_right{};
  glm::vec3 m_world_up{};

  double m_last_x{};
  double m_last_y{};

  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  // vec3 u, v, w;

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
