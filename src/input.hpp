#pragma once

#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace ren {

struct Bind {
  Bind() = default;
  Bind(int k, std::function<void()> c) : key(k), callback(c) {}
  Bind(int k, int a, std::function<void()> c)
      : key(k), activation(a), callback(c) {}

  void update(int state) { previous_state = state; }
  void trigger(int state) const {
    if (activation == GLFW_PRESS && state == GLFW_PRESS)
      callback();
    if (activation == GLFW_RELEASE && previous_state == GLFW_PRESS &&
        state == GLFW_RELEASE)
      callback();
  }
  int key{};
  int previous_state{GLFW_RELEASE};
  int activation{GLFW_PRESS};
  std::function<void()> callback{};
};

class Keymap {
public:
  void set_bind(Bind b) {
    m_bound.push_back(b.key);
    m_map[b.key] = b;
  }
  auto &map() { return m_map; }
  auto const &bound() { return m_bound; }

private:
  std::array<Bind, GLFW_KEY_LAST + 1> m_map;
  std::vector<int> m_bound{};
};
} // namespace ren
