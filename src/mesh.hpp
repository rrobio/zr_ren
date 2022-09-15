#pragma once

#include <memory>
#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "gl_util.hpp"

namespace ren {

struct Vertex {
  glm::vec3 pos;
  glm::vec3 norm;
  glm::vec2 tex;
};

class Mesh {
public:
  static std::unique_ptr<Mesh> construct(std::vector<Vertex> vertices,
                                         std::vector<GLuint> indices,
                                         bool has_adjacencies = false) {
    auto m = std::make_unique<Mesh>(has_adjacencies);
    m->m_verts = vertices;
    m->m_indices = indices;
    m->setup();

    return m;
  }
  static std::unique_ptr<Mesh> construct(std::vector<Vertex> vertices,
                                         bool find_adjacencies = false) {
    auto m = std::make_unique<Mesh>(find_adjacencies);
    m->m_verts = vertices;
    m->setup();

    return m;
  }

  Mesh(bool find_adjacencies = false) : m_has_adjacencies(find_adjacencies){};
  ~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }
  void draw() const {
    bind_vao();
    if (m_has_adjacencies) {
      glDrawElements(GL_TRIANGLES_ADJACENCY, m_indices.size(), GL_UNSIGNED_INT,
                     0);
    } else if (m_indices.size() != 0) {
      glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, m_verts.size());
    }
    glBindVertexArray(0);
  }

  void bind_vao() const { glBindVertexArray(VAO); }

  std::size_t n_indices() const { return m_indices.size(); }

private:
  void generate_adjacencies();
  void setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, m_verts.size() * sizeof(Vertex),
                 m_verts.data(), GL_STATIC_DRAW);

    if (m_indices.size() != 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
                   m_indices.data(), GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, norm));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, tex));

    glBindVertexArray(0);
  }

  bool m_has_adjacencies;
  GLuint VAO, VBO, EBO;
  std::vector<Vertex> m_verts;
  std::vector<GLuint> m_indices;
};

} // namespace ren
