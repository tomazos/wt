#include "gl/geometry.h"

namespace gl {

mat4 scale(float32 sx, float32 sy, float32 sz) {
  mat4 result;
  result << sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1;
  return result;
}

mat4 translate(vec3 delta) {
  mat4 result;
  result << 1, 0, 0, delta(0), 0, 1, 0, delta(1), 0, 0, 1, delta(2), 0, 0, 0, 1;
  return result;
}

mat4 roll(float32 alpha) {
  mat4 result;
  result << std::cos(alpha), -std::sin(alpha), 0, 0, std::sin(alpha),
      std::cos(alpha), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
  return result;
}

mat4 yaw(float32 beta) {
  mat4 result;
  result << std::cos(beta), 0, std::sin(beta), 0, 0, 1, 0, 0, -std::sin(beta),
      0, std::cos(beta), 0, 0, 0, 0, 1;
  return result;
}

mat4 pitch(float32 gamma) {
  mat4 result;
  result << 1, 0, 0, 0, 0, std::cos(gamma), -std::sin(gamma), 0, 0,
      std::sin(gamma), std::cos(gamma), 0, 0, 0, 0, 1;
  return result;
}

mat4 rotate(vec3 axis, float32 theta) {
  const vec3 normalized_axis = normalize(axis);
  const float32 x = normalized_axis(0);
  const float32 y = normalized_axis(1);
  const float32 z = normalized_axis(2);
  const float32 c = std::cos(theta);
  const float32 s = std::sin(theta);
  mat4 result;
  result << (c + x * x * (1 - c)), (x * y * (1 - c) - z * s),
      (x * z * (1 - c) + y * s), 0,

      (y * x * (1 - c) + z * s), (c + y * y * (1 - c)),
      (y * z * (1 - c) - x * s), 0,

      (z * x * (1 - c) - y * s), (z * y * (1 - c) + x * s),
      (c + z * z * (1 - c)), 0,

      0, 0, 0, 1;
  return result;
}

mat4 perspective(float32 l, float32 r, float32 t, float32 b, float32 n,
                 float32 f) {
  mat4 result;
  result << 2 * n / (r - l), 0, (r + l) / (r - l), 0, 0, 2 * n / (t - b),
      (t + b) / (t - b), 0, 0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n), 0,
      0, -1, 0;
  return result;
}

std::vector<dvec3> sphere(int tesselation_depth) {
  static const float32 phi = (1 + std::sqrt(5.0)) / 2;

  std::vector<dvec3> iv{{0, 1, phi},
                        {0, -1, phi},
                        {0, 1, -phi},
                        {0, -1, -phi},
                        {phi, 0, 1},
                        {phi, 0, -1},
                        {-phi, 0, 1},
                        {-phi, 0, -1},
                        {1, phi, 0},
                        {-1, phi, 0},
                        {1, -phi, 0},
                        {-1, -phi, 0}};

  std::vector<dvec3> vo;

  auto d = [&](size_t a, size_t b) {
    return std::abs(length(dvec3(iv[a] - iv[b])) - 2.0) < 0.0001;
  };

  for (size_t i = 0; i < 12; ++i) {
    for (size_t j = i + 1; j < 12; ++j) {
      for (size_t k = j + 1; k < 12; ++k) {
        if (d(i, j) && d(j, k) && d(k, i)) {
          vo.push_back(normalize(iv[i]));
          vo.push_back(normalize(iv[j]));
          vo.push_back(normalize(iv[k]));
        }
      }
    }
  }

  while (tesselation_depth-- > 0) {
    std::vector<dvec3> vi;
    std::swap(vi, vo);

    for (size_t i = 0; i < vi.size() / 3; ++i) {
      dvec3 v0 = vi[3 * i + 0];
      dvec3 v2 = vi[3 * i + 1];
      dvec3 v4 = vi[3 * i + 2];
      dvec3 v1 = normalize(dvec3((v0 + v2) / 2));
      dvec3 v3 = normalize(dvec3((v2 + v4) / 2));
      dvec3 v5 = normalize(dvec3((v4 + v0) / 2));

      std::vector<dvec3> vm{v0, v1, v5, v1, v2, v3, v1, v3, v5, v5, v3, v4};

      for (auto v : vm) vo.push_back(v);
    }
  }
  return vo;
}

}  // namespace gl
