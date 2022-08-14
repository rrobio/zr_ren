#pragma once

#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace ren {

struct Bind {
  enum class Action {
    press,
    release,
    hold,
  };

  Bind() = default;
  Bind(int k, std::function<void()> c) : key(k), callback(c) {}
  Bind(int k, Action a, std::function<void()> c)
      : key(k), action(a), callback(c) {}

  int key{};
  bool prev_s{false}, cur_s{false};
  Action action{Action::press};
  std::function<void()> callback{};
};

class Keymap {
public:
  void set_bind(Bind b) {
    m_bound[b.key] = true;
    m_map[b.key] = b;
  }
  void exec_bind(int key) {
    if (m_bound[key]) {
      m_map[key].callback();
    }
  }
  auto const &map() { return m_map; }

private:
  std::map<int, Bind> m_map;
  std::array<bool, GLFW_KEY_LAST + 1> m_bound{false};
};
} // namespace ren
