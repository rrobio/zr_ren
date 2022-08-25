#pragma once

#include <iostream>
#include <tuple>

#include "../../util.hpp"
#include "vec3.hpp"

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Divide the color by the number of samples.
  auto scale = 1.0 / samples_per_pixel;
  r = sqrt(scale * r);
  g = sqrt(scale * g);
  b = sqrt(scale * b);

  // Write the translated [0,255] value of each color component.
  out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

auto get_pixel_tuple(color pixel_color, int samples_per_pixel)
    -> std::tuple<uint8_t, uint8_t, uint8_t> {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  // Divide the color by the number of samples.
  auto scale = 1.0 / samples_per_pixel;
  r = sqrt(scale * r);
  g = sqrt(scale * g);
  b = sqrt(scale * b);

  // Write the translated [0,255] value of each color component.
  return {static_cast<uint8_t>(256 * clamp(r, 0.0, 0.999)),
          static_cast<uint8_t>(256 * clamp(g, 0.0, 0.999)),
          static_cast<uint8_t>(256 * clamp(b, 0.0, 0.999))};
}