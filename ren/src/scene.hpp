#pragma once

#include <string>
#include <vector>
#include <initializer_list>

#include "shader.hpp"
#include "object.hpp"

namespace ren {

class Scene {
public:
	Scene(std::initializer_list<Object> l) : m_objects(l) {}

	void render_scene(Shader const&);
private:
	std::vector<Object> m_objects;
};

}
