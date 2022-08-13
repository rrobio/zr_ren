#pragma once

#include <glm/glm.hpp>

class Primitive {
public:
	enum class Type {
		Sphere,
		Cube,
	};
	Primitive(Type t) : m_type(t) {}
	Primitive(Type t, glm::mat4 m) : m_type(t), m_model(m) {}

	void set_model(glm::mat4 m) { m_model = m; };
	auto model(glm::mat4 m) const { return m_model; };


private:
	Type m_type {};
	glm::mat4 m_model {};
};
