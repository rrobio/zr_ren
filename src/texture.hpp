#pragma once

#include "glad/glad.h"
#include <string>
#include <utility>
#include <vector>

#include "gl_util.hpp"
#include "resource_manager.hpp"

namespace ren {
struct Texture {
  GLuint id{0};
  GLuint width{0};
  GLuint height{0};
  bool m_is_valid{false};

  Texture() {
    glGenTextures(1, &id);
    m_is_valid = true;
  }
  Texture(Image &img) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.heigth(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, img.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    height = img.heigth();
    width = img.width();
    m_is_valid = true;
  }
  ~Texture() {
    if (m_is_valid) {
      glDeleteTextures(1, &id);
    }
  }
};

class TextureManager {
public:
  TextureManager() = default;
  ~TextureManager() = default;

  std::size_t create_texture_from_image(Image &img) {
    m_textures.emplace_back(img);
    return m_textures.size() - 1;
  }

  std::size_t create_empty_texture() {
    m_textures.emplace_back();
    return m_textures.size() - 1;
  }

  auto &texture(std::size_t id) const { return m_textures.at(id); }
  auto name(std::size_t id) const { return m_textures.at(id).id; }

  void bind(std::size_t id) const {
    glBindTexture(GL_TEXTURE_2D, m_textures[id].id);
    check_gl_error("bind");
  }

  void bind(std::size_t index, GLenum target) const {
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, m_textures[index].id);
    check_gl_error("bind");
  }

private:
  std::vector<Texture> m_textures;
};

} // namespace ren
