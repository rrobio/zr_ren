#pragma once

#include <string>

namespace ren {

class Image final {
public:
  Image(std::string const &path);
  ~Image();

  auto *data() { return m_data; }
  auto width() const { return m_width; }
  auto heigth() const { return m_height; }
  auto n_channels() const { return m_n_channels; }

private:
  int m_width;
  int m_height;
  int m_n_channels;

  unsigned char *m_data;
};

class ResourceManager final {
  static auto load_image(std::string const &path);
};

using rm = ResourceManager;
} // namespace ren
