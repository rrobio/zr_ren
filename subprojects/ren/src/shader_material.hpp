#pragma once

#include <strgin>

#include "glm/glm.hpp"

#include "shader.hpp"

namespace ren {
class Material {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float shin)
		:ambient(amb), diffuse(diff), specular(spec), shininess(shin)
	{}
};
}
