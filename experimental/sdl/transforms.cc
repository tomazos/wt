//#include "experimental/sdl/transforms.h"

// namespace gl {

// mat4 scale(float32 sx, float32 sy, float32 sz) {
//  mat4 result;
//  result << sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1;
//  return result;
//}

// mat4 translate(vec3 delta) {
//  mat4 result;
//  result << 1, 0, 0, delta(0), 0, 1, 0, delta(1), 0, 0, 1, delta(2), 0, 0, 0,
//  1;
//  return result;
//}

// mat4 roll(float32 alpha) {
//  mat4 result;
//  result << std::cos(alpha), -std::sin(alpha), 0, 0, std::sin(alpha),
//      std::cos(alpha), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
//  return result;
//}

// mat4 yaw(float32 beta) {
//  mat4 result;
//  result << std::cos(beta), 0, std::sin(beta), 0, 0, 1, 0, 0, -std::sin(beta),
//      0, std::cos(beta), 0, 0, 0, 0, 1;
//  return result;
//}

// mat4 pitch(float32 gamma) {
//  mat4 result;
//  result << 1, 0, 0, 0, 0, std::cos(gamma), -std::sin(gamma), 0, 0,
//      std::sin(gamma), std::cos(gamma), 0, 0, 0, 0, 1;
//  return result;
//}

// mat4 rotate(vec3 axis, float32 theta) {
//  const vec3 normalized_axis = normalize(axis);
//  const float32 x = normalized_axis(0);
//  const float32 y = normalized_axis(1);
//  const float32 z = normalized_axis(2);
//  const float32 c = std::cos(theta);
//  const float32 s = std::sin(theta);
//  mat4 result;
//  result << (c + x * x * (1 - c)), (x * y * (1 - c) - z * s),
//      (x * z * (1 - c) + y * s), 0,

//      (y * x * (1 - c) + z * s), (c + y * y * (1 - c)),
//      (y * z * (1 - c) - x * s), 0,

//      (z * x * (1 - c) - y * s), (z * y * (1 - c) + x * s),
//      (c + z * z * (1 - c)), 0,

//      0, 0, 0, 1;
//  return result;
//}

// mat4 perspective(float32 l, float32 r, float32 t, float32 b, float32 n,
//                 float32 f) {
//  mat4 result;
//  result << 2 * n / (r - l), 0, (r + l) / (r - l), 0, 0, 2 * n / (t - b),
//      (t + b) / (t - b), 0, 0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n),
//      0,
//      0, -1, 0;
//  return result;
//}

//}  // namespace gl
