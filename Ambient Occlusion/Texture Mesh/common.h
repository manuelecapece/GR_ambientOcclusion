#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

// Common Headers
#include "ray.h"
#include "vec3.h"


// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const float  infinity = std::numeric_limits<float>::infinity();
const float        pi = 3.1415926535897932384f;
const float    TWO_PI = 6.2831853071795864769f;
const double 	invPI = 0.3183098861837906715;
const double kEpsilon = 0.0001;
const float Deg2Rad  = 3.1415926f / 180.0f;

// Utility Functions

inline float degrees_to_radians(float degrees) {
  return degrees * pi / 180.0f;
}

inline float random_float() {
  // Returns a random real in [0,1).
  return float(rand() / (RAND_MAX + 1.0f));
}

////Versione alternativa
//inline float random_float() {
//    // Crea un generatore di numeri casuali
//    static std::mt19937 generator(std::random_device{}());
//
//    // Crea una distribuzione uniforme tra 0 e 1
//    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
//
//    // Genera e restituisce un numero casuale in virgola mobile nell'intervallo [0,1)
//    return distribution(generator);
//}

inline float random_float(float min, float max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_float();
}

inline void set_rand_seed(const int seed) { srand(seed); }

inline int rand_int() { return rand(); }

inline float rand_float() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

inline float rand_float(int l, float h) { return rand_float() * (h - l) + l; }

inline int rand_int(int l, int h) { return (int)(rand_float(0, (float)h - (float)l + 1.0f) + (float)l); }

// lerp function

inline float lerp(float a, float b, float t) {
  return (1.0f - t) * a + t * b;
}

inline vec3 lerp(vec3& a, vec3& b, float t) {
  return (1.0f - t) * a + t * b;
}

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

#endif