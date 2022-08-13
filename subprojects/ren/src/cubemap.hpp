#pragma once

#include <vector>
#include <string>
#include "glad/glad.h"

#include "resource_manager.hpp"
#include "texture.hpp"
#include "object.hpp"

namespace ren {
class CubeMap {
public:
	CubeMap(std::vector<std::string> faces): m_tex({})
	{
		m_box_obj = create_skybox();
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex.id);
		for (size_t i = 0; i < faces.size(); i++) {
			auto img = Image(faces[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
						 img.width(), img.heigth(), 0, GL_RGB, GL_UNSIGNED_BYTE,
						 img.data());
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void draw()
	{
		glActiveTexture(GL_TEXTURE0);
		// TODO:
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex.id);
		m_box_obj.draw();
	}

	void bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex.id);
	}

	Texture m_tex;
	Object m_box_obj;
};
}
