#pragma once

// clang-format: off
#include "GLFW/glfw3.h"
#include "glad/glad.h"
// clang-format: on

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>

#include "input.hpp"

namespace ren {

class Window final {
public:
  Window(std::string name, int width, int height, bool debug = false) {
    (void)debug; // currently unused
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!m_window) {
      std::cerr << "Failed to create GLFW window\n";
      m_success = false;
      return;
    }

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    m_success = true;
  }
  ~Window() { glfwTerminate(); }

  inline bool should_close() { return glfwWindowShouldClose(m_window); }

  inline void swap_buffers() { glfwSwapBuffers(m_window); }

  inline void poll_events() { glfwPollEvents(); }

  inline auto get_key_state(int key) { return glfwGetKey(m_window, key); }

  void set_should_close(bool b) { glfwSetWindowShouldClose(m_window, b); }

  void set_hint(int hint, int value) { glfwWindowHint(hint, value); }

  void set_input_mode(int mode, int value) {
    glfwSetInputMode(m_window, mode, value);
  }

  void set_key_callback(GLFWkeyfun callback) {
    glfwSetKeyCallback(m_window, callback);
  }

  std::tuple<double, double> get_cursor_pos() {
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    return std::make_tuple(xpos, ypos);
  }

  auto get_ptr() { return m_window; }

  void set_keymap(Keymap map) { m_keymap = map; }
  void exec_keymap() {
    for (auto &index : m_keymap.bound()) {
      auto &bind = m_keymap.map().at(index);
      auto const state = get_key_state(index);
      bind.trigger(state);
      bind.update(state);
    }
  }
  std::tuple<int, int> size() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return std::make_tuple(width, height);
  }

private:
  GLFWwindow *m_window{nullptr};

  bool m_success{false};
  Keymap m_keymap;
};
} // namespace ren
