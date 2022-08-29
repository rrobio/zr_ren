#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

float const infinity = std::numeric_limits<float>::infinity();
float const pi = 3.1415926535897932385;

inline float degrees_to_radians(float degrees) { return degrees * pi / 180.0; }

inline float random_float() {
  static std::uniform_real_distribution<float> distribution(0.0, 1.0);
  static std::mt19937 generator;
  return distribution(generator);
}
inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}

inline float clamp(float x, float min, float max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}
