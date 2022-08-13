#pragma once

#include <algorithm>

#include "glm/glm.hpp"

#include "log.hpp"
//#include "ray.hpp"

namespace ren {

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up,
		   float const screen_aspect)
		: m_position(position), m_up(up), m_world_up(up)
	{
		auto theta = glm::radians(90.f);
		auto h = tan(theta / 2);
		float viewport_height = 2.0 * h;
		float viewport_width = screen_aspect * viewport_height;

		w = glm::normalize(position - lookat);
		u = glm::normalize(cross(up, w));
		v = glm::cross(w, u);

		horizontal = viewport_width * u;
		vertical = viewport_height * v;
		lower_left_corner =
			origin - (horizontal * (1.f/2.f)) - (vertical * (1.f/2.f));
		update_vectors();
	}

	auto view() const
	{
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	auto pos() const { return m_position; }

	void move(glm::vec3 const m, float const speed)
	{
		if (m.x != 0) m_position += m_right * m.x * speed;
		if (m.z != 0) m_position += m_front * m.z * speed;
		if (m.y != 0) m_position += m_up * m.y * speed;
	}

	void rotate(float y_off, float p_off, float speed)
	{
		m_yaw += y_off * speed;
		m_pitch -= p_off * speed;

		if (m_pitch > 89.0f) m_pitch = 89.0f;
		if (m_pitch < -89.0f) m_pitch = -89.0f;

		update_vectors();
	}

	void rotate_offset(std::tuple<double, double> xy)
	{
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
///				   lower_left_corner + s * horizontal + t * vertical - origin);
///	}

private:
	float m_yaw {-90.0f};
	float m_pitch {0.0f};

	glm::vec3 m_position {};
	glm::vec3 m_front {};
	glm::vec3 m_up {};
	glm::vec3 m_right {};
	glm::vec3 m_world_up {};

	double m_last_x {};
	double m_last_y {};

	glm::vec3 origin;
	glm::vec3 lower_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 u, v, w;

	void update_vectors()
	{
		glm::vec3 front;
		front.x =
			glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		front.y = glm::sin(glm::radians(m_pitch));
		front.z =
			glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);

		m_right = glm::normalize(glm::cross(m_front, m_world_up));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};
} // namespace ren
