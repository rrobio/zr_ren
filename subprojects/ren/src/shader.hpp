#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/type_trait.hpp"

#include <array>
#include <string>
#include <vector>

#include <cassert>
#include <filesystem>
#include <iostream>

#include "material.hpp"

namespace ren {

class Shader final {
public:
  GLuint ID;

  enum Type { VERTEX, FRAGMENT, GEOMETRY, PROGRAM };

  Shader() = default;
  Shader(std::filesystem::path const &vertex_path,
         std::filesystem::path const &fragment_path,
         std::filesystem::path const &geometry_path = {});
  Shader &operator=(Shader &&other) {
    std::cout << "ass operator\n";
    ID = other.ID;
    m_success = other.m_success;
    return *this;
  }
  ~Shader() = default;

  void use() { glUseProgram(ID); }
  bool success() { return m_success; }

  template <typename T> void set(GLchar const *name, T const &value) {
    auto const location = glGetUniformLocation(ID, name);
    if (location == -1) {
      std::cout << "Shader (set): " << name << " -1" << std::endl;
    }
    assert(location != -1);

    // TODO: clean up
    if constexpr (std::is_same<T, bool>::value) { // bool
      glUniform1i(location, static_cast<int>(value));
    } else if constexpr (std::is_same<T, GLuint>::value) { // GLuint
      glUniform1i(location, value);
    } else if constexpr (std::is_same<T, int>::value) { // int
      glUniform1i(location, value);
    } else if constexpr (std::is_same<T, float>::value) { // float
      glUniform1f(location, value);
    } else if constexpr (std::is_same<T, glm::vec2>::value) { // vec2
      glUniform2f(location, value.x, value.y);
    } else if constexpr (std::is_same<T, glm::vec3>::value) { // vec3
      glUniform3f(location, value.x, value.y, value.z);
    } else if constexpr (std::is_same<T, glm::vec4>::value) { // vec4
      glUniform4f(location, value.x, value.y, value.z, value.w);
    } else if constexpr (std::is_same<T, glm::mat2>::value) { // mat2
      glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::is_same<T, glm::mat3>::value) { // mat3
      glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    } else if constexpr (std::is_same<T, glm::mat4>::value) { // mat4
      glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
  }

  void set_material(Material *mat) {
    this->set("material.diffuse", mat->diffuse);
  }

private:
  bool m_success{false};
};
} // namespace ren
