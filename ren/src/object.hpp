#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "mesh.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <memory>
#include <array>
#include <utility>

#include "material.hpp"

namespace ren {
class Object {
public:
	Object() = default;
	Object(std::vector<Vertex> vertices, std::vector<GLuint> indices)
	{
		m_mesh = Mesh::construct(vertices, indices);
	}
	Object(std::vector<Vertex> vertices)
	{
		m_mesh = Mesh::construct(vertices);
	}

	void draw() const
	{
		assert(m_mesh != nullptr);
		m_mesh->draw();
	}
	bool is_valid() const { return m_mesh != nullptr; }

	auto model() const { return m_model; }
	void set_model(glm::mat4 m) { m_model = m; }

	auto material() const -> std::shared_ptr<Material> { return m_material; }
	void set_material(std::shared_ptr<Material> m) { m_material = m; }

private:
	std::unique_ptr<Mesh> m_mesh {};
	std::shared_ptr<Material> m_material {};
	glm::mat4 m_model {};
};

Object create_icospehere(const int subdivisions);
Object create_sphere();
Object create_cube();
Object create_textured_cube();
Object create_plane();
Object create_skybox();

} // namespace ren

