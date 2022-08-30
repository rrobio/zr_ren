#pragma once

#include <iostream>
#include <tuple>

#include "util.hpp"
#include "vec3.hpp"

namespace ren {

void write_color(std::ostream &out, color pixel_color, int samples_per_pixel);
auto get_pixel_tuple(color pixel_color, int samples_per_pixel)
    -> std::tuple<uint8_t, uint8_t, uint8_t>;

} // namespace ren
