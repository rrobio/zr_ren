#include "resource_manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ren {

Image::Image(std::filesystem::path const &path) {
  stbi_set_flip_vertically_on_load(true);
  m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_n_channels, 0);
  assert(m_data);
}

Image::~Image() { stbi_image_free(m_data); }

} // namespace ren
