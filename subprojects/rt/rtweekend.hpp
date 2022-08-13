#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

double const infinity = std::numeric_limits<double>::infinity();
double const pi = 3.1415926535897932385;


inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}


inline double random_double()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
inline double random_double(double min, double max)
{
	return min + (max-min)*random_double();
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

#include "ray.hpp"
#include "vec3.hpp"
