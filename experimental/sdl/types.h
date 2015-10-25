#pragma once

namespace gl {

struct vec2 : std::array<float32, 2> {
  constexpr vec2(float32 v0, float32 v1) : std::array<float32, 2>({v0, v1}) {}
};

STATIC_ASSERT(sizeof(vec2) == 2 * sizeof(float32));

constexpr vec2 operator+(float32 k, const vec2& v) {
  return vec2(k + v[0], k + v[1]);
}

constexpr vec2 operator+(const vec2& v, float32 k) {
  return vec2(k + v[0], k + v[1]);
}

constexpr vec2 operator+(const vec2& a, const vec2& b) {
  return vec2(a[0] + b[0], a[1] + b[1]);
}

constexpr vec2 operator-(float32 k, const vec2& v) {
  return vec2(k - v[0], k - v[1]);
}

constexpr vec2 operator-(const vec2& v, float32 k) {
  return vec2(k - v[0], k - v[1]);
}

constexpr vec2 operator-(const vec2& a, const vec2& b) {
  return vec2(a[0] - b[0], a[1] - b[1]);
}

constexpr vec2 operator*(float32 k, const vec2& v) {
  return vec2(k * v[0], k * v[1]);
}

constexpr vec2 operator*(const vec2& v, float32 k) {
  return vec2(k * v[0], k * v[1]);
}

constexpr vec2 operator*(const vec2& a, const vec2& b) {
  return vec2(a[0] * b[0], a[1] * b[1]);
}

constexpr vec2 operator/(float32 k, const vec2& v) {
  return vec2(k / v[0], k / v[1]);
}

constexpr vec2 operator/(const vec2& v, float32 k) {
  return vec2(k / v[0], k / v[1]);
}

constexpr vec2 operator/(const vec2& a, const vec2& b) {
  return vec2(a[0] / b[0], a[1] / b[1]);
}

struct vec3 : std::array<float32, 3> {
  constexpr vec3(float32 v0, float32 v1, float32 v2)
      : std::array<float32, 3>({v0, v1, v2}) {}
};

STATIC_ASSERT(sizeof(vec3) == 3 * sizeof(float32));

constexpr vec3 operator+(float32 k, const vec3& v) {
  return vec3(k + v[0], k + v[1], k + v[2]);
}

constexpr vec3 operator+(const vec3& v, float32 k) {
  return vec3(k + v[0], k + v[1], k + v[2]);
}

constexpr vec3 operator+(const vec3& a, const vec3& b) {
  return vec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

constexpr vec3 operator-(float32 k, const vec3& v) {
  return vec3(k - v[0], k - v[1], k - v[2]);
}

constexpr vec3 operator-(const vec3& v, float32 k) {
  return vec3(k - v[0], k - v[1], k - v[2]);
}

constexpr vec3 operator-(const vec3& a, const vec3& b) {
  return vec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

constexpr vec3 operator*(float32 k, const vec3& v) {
  return vec3(k * v[0], k * v[1], k * v[2]);
}

constexpr vec3 operator*(const vec3& v, float32 k) {
  return vec3(k * v[0], k * v[1], k * v[2]);
}

constexpr vec3 operator*(const vec3& a, const vec3& b) {
  return vec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

constexpr vec3 operator/(float32 k, const vec3& v) {
  return vec3(k / v[0], k / v[1], k / v[2]);
}

constexpr vec3 operator/(const vec3& v, float32 k) {
  return vec3(k / v[0], k / v[1], k / v[2]);
}

constexpr vec3 operator/(const vec3& a, const vec3& b) {
  return vec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

struct vec4 : std::array<float32, 4> {
  constexpr vec4(float32 v0, float32 v1, float32 v2, float32 v3)
      : std::array<float32, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(vec4) == 4 * sizeof(float32));

constexpr vec4 operator+(float32 k, const vec4& v) {
  return vec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr vec4 operator+(const vec4& v, float32 k) {
  return vec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr vec4 operator+(const vec4& a, const vec4& b) {
  return vec4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr vec4 operator-(float32 k, const vec4& v) {
  return vec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr vec4 operator-(const vec4& v, float32 k) {
  return vec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr vec4 operator-(const vec4& a, const vec4& b) {
  return vec4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr vec4 operator*(float32 k, const vec4& v) {
  return vec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr vec4 operator*(const vec4& v, float32 k) {
  return vec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr vec4 operator*(const vec4& a, const vec4& b) {
  return vec4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr vec4 operator/(float32 k, const vec4& v) {
  return vec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr vec4 operator/(const vec4& v, float32 k) {
  return vec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr vec4 operator/(const vec4& a, const vec4& b) {
  return vec4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

struct dvec2 : std::array<float64, 2> {
  constexpr dvec2(float64 v0, float64 v1) : std::array<float64, 2>({v0, v1}) {}
};

STATIC_ASSERT(sizeof(dvec2) == 2 * sizeof(float64));

constexpr dvec2 operator+(float64 k, const dvec2& v) {
  return dvec2(k + v[0], k + v[1]);
}

constexpr dvec2 operator+(const dvec2& v, float64 k) {
  return dvec2(k + v[0], k + v[1]);
}

constexpr dvec2 operator+(const dvec2& a, const dvec2& b) {
  return dvec2(a[0] + b[0], a[1] + b[1]);
}

constexpr dvec2 operator-(float64 k, const dvec2& v) {
  return dvec2(k - v[0], k - v[1]);
}

constexpr dvec2 operator-(const dvec2& v, float64 k) {
  return dvec2(k - v[0], k - v[1]);
}

constexpr dvec2 operator-(const dvec2& a, const dvec2& b) {
  return dvec2(a[0] - b[0], a[1] - b[1]);
}

constexpr dvec2 operator*(float64 k, const dvec2& v) {
  return dvec2(k * v[0], k * v[1]);
}

constexpr dvec2 operator*(const dvec2& v, float64 k) {
  return dvec2(k * v[0], k * v[1]);
}

constexpr dvec2 operator*(const dvec2& a, const dvec2& b) {
  return dvec2(a[0] * b[0], a[1] * b[1]);
}

constexpr dvec2 operator/(float64 k, const dvec2& v) {
  return dvec2(k / v[0], k / v[1]);
}

constexpr dvec2 operator/(const dvec2& v, float64 k) {
  return dvec2(k / v[0], k / v[1]);
}

constexpr dvec2 operator/(const dvec2& a, const dvec2& b) {
  return dvec2(a[0] / b[0], a[1] / b[1]);
}

struct dvec3 : std::array<float64, 3> {
  constexpr dvec3(float64 v0, float64 v1, float64 v2)
      : std::array<float64, 3>({v0, v1, v2}) {}
};

STATIC_ASSERT(sizeof(dvec3) == 3 * sizeof(float64));

constexpr dvec3 operator+(float64 k, const dvec3& v) {
  return dvec3(k + v[0], k + v[1], k + v[2]);
}

constexpr dvec3 operator+(const dvec3& v, float64 k) {
  return dvec3(k + v[0], k + v[1], k + v[2]);
}

constexpr dvec3 operator+(const dvec3& a, const dvec3& b) {
  return dvec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

constexpr dvec3 operator-(float64 k, const dvec3& v) {
  return dvec3(k - v[0], k - v[1], k - v[2]);
}

constexpr dvec3 operator-(const dvec3& v, float64 k) {
  return dvec3(k - v[0], k - v[1], k - v[2]);
}

constexpr dvec3 operator-(const dvec3& a, const dvec3& b) {
  return dvec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

constexpr dvec3 operator*(float64 k, const dvec3& v) {
  return dvec3(k * v[0], k * v[1], k * v[2]);
}

constexpr dvec3 operator*(const dvec3& v, float64 k) {
  return dvec3(k * v[0], k * v[1], k * v[2]);
}

constexpr dvec3 operator*(const dvec3& a, const dvec3& b) {
  return dvec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

constexpr dvec3 operator/(float64 k, const dvec3& v) {
  return dvec3(k / v[0], k / v[1], k / v[2]);
}

constexpr dvec3 operator/(const dvec3& v, float64 k) {
  return dvec3(k / v[0], k / v[1], k / v[2]);
}

constexpr dvec3 operator/(const dvec3& a, const dvec3& b) {
  return dvec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

struct dvec4 : std::array<float64, 4> {
  constexpr dvec4(float64 v0, float64 v1, float64 v2, float64 v3)
      : std::array<float64, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(dvec4) == 4 * sizeof(float64));

constexpr dvec4 operator+(float64 k, const dvec4& v) {
  return dvec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr dvec4 operator+(const dvec4& v, float64 k) {
  return dvec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr dvec4 operator+(const dvec4& a, const dvec4& b) {
  return dvec4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr dvec4 operator-(float64 k, const dvec4& v) {
  return dvec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr dvec4 operator-(const dvec4& v, float64 k) {
  return dvec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr dvec4 operator-(const dvec4& a, const dvec4& b) {
  return dvec4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr dvec4 operator*(float64 k, const dvec4& v) {
  return dvec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr dvec4 operator*(const dvec4& v, float64 k) {
  return dvec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr dvec4 operator*(const dvec4& a, const dvec4& b) {
  return dvec4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr dvec4 operator/(float64 k, const dvec4& v) {
  return dvec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr dvec4 operator/(const dvec4& v, float64 k) {
  return dvec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr dvec4 operator/(const dvec4& a, const dvec4& b) {
  return dvec4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

struct bvec2 : std::array<bool, 2> {
  constexpr bvec2(bool v0, bool v1) : std::array<bool, 2>({v0, v1}) {}
};

STATIC_ASSERT(sizeof(bvec2) == 2 * sizeof(bool));

constexpr bvec2 operator&&(bool k, const bvec2& v) {
  return bvec2(k && v[0], k && v[1]);
}

constexpr bvec2 operator&&(const bvec2& v, bool k) {
  return bvec2(k && v[0], k && v[1]);
}

constexpr bvec2 operator&&(const bvec2& a, const bvec2& b) {
  return bvec2(a[0] && b[0], a[1] && b[1]);
}

constexpr bvec2 operator||(bool k, const bvec2& v) {
  return bvec2(k || v[0], k || v[1]);
}

constexpr bvec2 operator||(const bvec2& v, bool k) {
  return bvec2(k || v[0], k || v[1]);
}

constexpr bvec2 operator||(const bvec2& a, const bvec2& b) {
  return bvec2(a[0] || b[0], a[1] || b[1]);
}

struct bvec3 : std::array<bool, 3> {
  constexpr bvec3(bool v0, bool v1, bool v2)
      : std::array<bool, 3>({v0, v1, v2}) {}
};

STATIC_ASSERT(sizeof(bvec3) == 3 * sizeof(bool));

constexpr bvec3 operator&&(bool k, const bvec3& v) {
  return bvec3(k && v[0], k && v[1], k && v[2]);
}

constexpr bvec3 operator&&(const bvec3& v, bool k) {
  return bvec3(k && v[0], k && v[1], k && v[2]);
}

constexpr bvec3 operator&&(const bvec3& a, const bvec3& b) {
  return bvec3(a[0] && b[0], a[1] && b[1], a[2] && b[2]);
}

constexpr bvec3 operator||(bool k, const bvec3& v) {
  return bvec3(k || v[0], k || v[1], k || v[2]);
}

constexpr bvec3 operator||(const bvec3& v, bool k) {
  return bvec3(k || v[0], k || v[1], k || v[2]);
}

constexpr bvec3 operator||(const bvec3& a, const bvec3& b) {
  return bvec3(a[0] || b[0], a[1] || b[1], a[2] || b[2]);
}

struct bvec4 : std::array<bool, 4> {
  constexpr bvec4(bool v0, bool v1, bool v2, bool v3)
      : std::array<bool, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(bvec4) == 4 * sizeof(bool));

constexpr bvec4 operator&&(bool k, const bvec4& v) {
  return bvec4(k && v[0], k && v[1], k && v[2], k && v[3]);
}

constexpr bvec4 operator&&(const bvec4& v, bool k) {
  return bvec4(k && v[0], k && v[1], k && v[2], k && v[3]);
}

constexpr bvec4 operator&&(const bvec4& a, const bvec4& b) {
  return bvec4(a[0] && b[0], a[1] && b[1], a[2] && b[2], a[3] && b[3]);
}

constexpr bvec4 operator||(bool k, const bvec4& v) {
  return bvec4(k || v[0], k || v[1], k || v[2], k || v[3]);
}

constexpr bvec4 operator||(const bvec4& v, bool k) {
  return bvec4(k || v[0], k || v[1], k || v[2], k || v[3]);
}

constexpr bvec4 operator||(const bvec4& a, const bvec4& b) {
  return bvec4(a[0] || b[0], a[1] || b[1], a[2] || b[2], a[3] || b[3]);
}

struct ivec2 : std::array<int32, 2> {
  constexpr ivec2(int32 v0, int32 v1) : std::array<int32, 2>({v0, v1}) {}
};

STATIC_ASSERT(sizeof(ivec2) == 2 * sizeof(int32));

constexpr ivec2 operator+(int32 k, const ivec2& v) {
  return ivec2(k + v[0], k + v[1]);
}

constexpr ivec2 operator+(const ivec2& v, int32 k) {
  return ivec2(k + v[0], k + v[1]);
}

constexpr ivec2 operator+(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] + b[0], a[1] + b[1]);
}

constexpr ivec2 operator-(int32 k, const ivec2& v) {
  return ivec2(k - v[0], k - v[1]);
}

constexpr ivec2 operator-(const ivec2& v, int32 k) {
  return ivec2(k - v[0], k - v[1]);
}

constexpr ivec2 operator-(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] - b[0], a[1] - b[1]);
}

constexpr ivec2 operator*(int32 k, const ivec2& v) {
  return ivec2(k * v[0], k * v[1]);
}

constexpr ivec2 operator*(const ivec2& v, int32 k) {
  return ivec2(k * v[0], k * v[1]);
}

constexpr ivec2 operator*(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] * b[0], a[1] * b[1]);
}

constexpr ivec2 operator/(int32 k, const ivec2& v) {
  return ivec2(k / v[0], k / v[1]);
}

constexpr ivec2 operator/(const ivec2& v, int32 k) {
  return ivec2(k / v[0], k / v[1]);
}

constexpr ivec2 operator/(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] / b[0], a[1] / b[1]);
}

constexpr ivec2 operator%(int32 k, const ivec2& v) {
  return ivec2(k % v[0], k % v[1]);
}

constexpr ivec2 operator%(const ivec2& v, int32 k) {
  return ivec2(k % v[0], k % v[1]);
}

constexpr ivec2 operator%(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] % b[0], a[1] % b[1]);
}

constexpr ivec2 operator&(int32 k, const ivec2& v) {
  return ivec2(k & v[0], k & v[1]);
}

constexpr ivec2 operator&(const ivec2& v, int32 k) {
  return ivec2(k & v[0], k & v[1]);
}

constexpr ivec2 operator&(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] & b[0], a[1] & b[1]);
}

constexpr ivec2 operator|(int32 k, const ivec2& v) {
  return ivec2(k | v[0], k | v[1]);
}

constexpr ivec2 operator|(const ivec2& v, int32 k) {
  return ivec2(k | v[0], k | v[1]);
}

constexpr ivec2 operator|(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] | b[0], a[1] | b[1]);
}

constexpr ivec2 operator^(int32 k, const ivec2& v) {
  return ivec2(k ^ v[0], k ^ v[1]);
}

constexpr ivec2
operator^(const ivec2& v, int32 k) { return ivec2(k ^ v[0], k ^ v[1]); }

constexpr ivec2
operator^(const ivec2& a, const ivec2& b) {
  return ivec2(a[0] ^ b[0], a[1] ^ b[1]);
}

struct ivec3 : std::array<int32, 3> {
  constexpr ivec3(int32 v0, int32 v1, int32 v2)
      : std::array<int32, 3>({v0, v1, v2}) {}
};

STATIC_ASSERT(sizeof(ivec3) == 3 * sizeof(int32));

constexpr ivec3 operator+(int32 k, const ivec3& v) {
  return ivec3(k + v[0], k + v[1], k + v[2]);
}

constexpr ivec3 operator+(const ivec3& v, int32 k) {
  return ivec3(k + v[0], k + v[1], k + v[2]);
}

constexpr ivec3 operator+(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

constexpr ivec3 operator-(int32 k, const ivec3& v) {
  return ivec3(k - v[0], k - v[1], k - v[2]);
}

constexpr ivec3 operator-(const ivec3& v, int32 k) {
  return ivec3(k - v[0], k - v[1], k - v[2]);
}

constexpr ivec3 operator-(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

constexpr ivec3 operator*(int32 k, const ivec3& v) {
  return ivec3(k * v[0], k * v[1], k * v[2]);
}

constexpr ivec3 operator*(const ivec3& v, int32 k) {
  return ivec3(k * v[0], k * v[1], k * v[2]);
}

constexpr ivec3 operator*(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

constexpr ivec3 operator/(int32 k, const ivec3& v) {
  return ivec3(k / v[0], k / v[1], k / v[2]);
}

constexpr ivec3 operator/(const ivec3& v, int32 k) {
  return ivec3(k / v[0], k / v[1], k / v[2]);
}

constexpr ivec3 operator/(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

constexpr ivec3 operator%(int32 k, const ivec3& v) {
  return ivec3(k % v[0], k % v[1], k % v[2]);
}

constexpr ivec3 operator%(const ivec3& v, int32 k) {
  return ivec3(k % v[0], k % v[1], k % v[2]);
}

constexpr ivec3 operator%(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] % b[0], a[1] % b[1], a[2] % b[2]);
}

constexpr ivec3 operator&(int32 k, const ivec3& v) {
  return ivec3(k & v[0], k & v[1], k & v[2]);
}

constexpr ivec3 operator&(const ivec3& v, int32 k) {
  return ivec3(k & v[0], k & v[1], k & v[2]);
}

constexpr ivec3 operator&(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] & b[0], a[1] & b[1], a[2] & b[2]);
}

constexpr ivec3 operator|(int32 k, const ivec3& v) {
  return ivec3(k | v[0], k | v[1], k | v[2]);
}

constexpr ivec3 operator|(const ivec3& v, int32 k) {
  return ivec3(k | v[0], k | v[1], k | v[2]);
}

constexpr ivec3 operator|(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] | b[0], a[1] | b[1], a[2] | b[2]);
}

constexpr ivec3 operator^(int32 k, const ivec3& v) {
  return ivec3(k ^ v[0], k ^ v[1], k ^ v[2]);
}

constexpr ivec3
operator^(const ivec3& v, int32 k) {
  return ivec3(k ^ v[0], k ^ v[1], k ^ v[2]);
}

constexpr ivec3
operator^(const ivec3& a, const ivec3& b) {
  return ivec3(a[0] ^ b[0], a[1] ^ b[1], a[2] ^ b[2]);
}

struct ivec4 : std::array<int32, 4> {
  constexpr ivec4(int32 v0, int32 v1, int32 v2, int32 v3)
      : std::array<int32, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(ivec4) == 4 * sizeof(int32));

constexpr ivec4 operator+(int32 k, const ivec4& v) {
  return ivec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr ivec4 operator+(const ivec4& v, int32 k) {
  return ivec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr ivec4 operator+(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr ivec4 operator-(int32 k, const ivec4& v) {
  return ivec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr ivec4 operator-(const ivec4& v, int32 k) {
  return ivec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr ivec4 operator-(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr ivec4 operator*(int32 k, const ivec4& v) {
  return ivec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr ivec4 operator*(const ivec4& v, int32 k) {
  return ivec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr ivec4 operator*(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr ivec4 operator/(int32 k, const ivec4& v) {
  return ivec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr ivec4 operator/(const ivec4& v, int32 k) {
  return ivec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr ivec4 operator/(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

constexpr ivec4 operator%(int32 k, const ivec4& v) {
  return ivec4(k % v[0], k % v[1], k % v[2], k % v[3]);
}

constexpr ivec4 operator%(const ivec4& v, int32 k) {
  return ivec4(k % v[0], k % v[1], k % v[2], k % v[3]);
}

constexpr ivec4 operator%(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] % b[0], a[1] % b[1], a[2] % b[2], a[3] % b[3]);
}

constexpr ivec4 operator&(int32 k, const ivec4& v) {
  return ivec4(k & v[0], k & v[1], k & v[2], k & v[3]);
}

constexpr ivec4 operator&(const ivec4& v, int32 k) {
  return ivec4(k & v[0], k & v[1], k & v[2], k & v[3]);
}

constexpr ivec4 operator&(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] & b[0], a[1] & b[1], a[2] & b[2], a[3] & b[3]);
}

constexpr ivec4 operator|(int32 k, const ivec4& v) {
  return ivec4(k | v[0], k | v[1], k | v[2], k | v[3]);
}

constexpr ivec4 operator|(const ivec4& v, int32 k) {
  return ivec4(k | v[0], k | v[1], k | v[2], k | v[3]);
}

constexpr ivec4 operator|(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] | b[0], a[1] | b[1], a[2] | b[2], a[3] | b[3]);
}

constexpr ivec4 operator^(int32 k, const ivec4& v) {
  return ivec4(k ^ v[0], k ^ v[1], k ^ v[2], k ^ v[3]);
}

constexpr ivec4
operator^(const ivec4& v, int32 k) {
  return ivec4(k ^ v[0], k ^ v[1], k ^ v[2], k ^ v[3]);
}

constexpr ivec4
operator^(const ivec4& a, const ivec4& b) {
  return ivec4(a[0] ^ b[0], a[1] ^ b[1], a[2] ^ b[2], a[3] ^ b[3]);
}

struct uvec2 : std::array<uint32, 2> {
  constexpr uvec2(uint32 v0, uint32 v1) : std::array<uint32, 2>({v0, v1}) {}
};

STATIC_ASSERT(sizeof(uvec2) == 2 * sizeof(uint32));

constexpr uvec2 operator+(uint32 k, const uvec2& v) {
  return uvec2(k + v[0], k + v[1]);
}

constexpr uvec2 operator+(const uvec2& v, uint32 k) {
  return uvec2(k + v[0], k + v[1]);
}

constexpr uvec2 operator+(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] + b[0], a[1] + b[1]);
}

constexpr uvec2 operator-(uint32 k, const uvec2& v) {
  return uvec2(k - v[0], k - v[1]);
}

constexpr uvec2 operator-(const uvec2& v, uint32 k) {
  return uvec2(k - v[0], k - v[1]);
}

constexpr uvec2 operator-(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] - b[0], a[1] - b[1]);
}

constexpr uvec2 operator*(uint32 k, const uvec2& v) {
  return uvec2(k * v[0], k * v[1]);
}

constexpr uvec2 operator*(const uvec2& v, uint32 k) {
  return uvec2(k * v[0], k * v[1]);
}

constexpr uvec2 operator*(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] * b[0], a[1] * b[1]);
}

constexpr uvec2 operator/(uint32 k, const uvec2& v) {
  return uvec2(k / v[0], k / v[1]);
}

constexpr uvec2 operator/(const uvec2& v, uint32 k) {
  return uvec2(k / v[0], k / v[1]);
}

constexpr uvec2 operator/(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] / b[0], a[1] / b[1]);
}

constexpr uvec2 operator%(uint32 k, const uvec2& v) {
  return uvec2(k % v[0], k % v[1]);
}

constexpr uvec2 operator%(const uvec2& v, uint32 k) {
  return uvec2(k % v[0], k % v[1]);
}

constexpr uvec2 operator%(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] % b[0], a[1] % b[1]);
}

constexpr uvec2 operator&(uint32 k, const uvec2& v) {
  return uvec2(k & v[0], k & v[1]);
}

constexpr uvec2 operator&(const uvec2& v, uint32 k) {
  return uvec2(k & v[0], k & v[1]);
}

constexpr uvec2 operator&(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] & b[0], a[1] & b[1]);
}

constexpr uvec2 operator|(uint32 k, const uvec2& v) {
  return uvec2(k | v[0], k | v[1]);
}

constexpr uvec2 operator|(const uvec2& v, uint32 k) {
  return uvec2(k | v[0], k | v[1]);
}

constexpr uvec2 operator|(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] | b[0], a[1] | b[1]);
}

constexpr uvec2 operator^(uint32 k, const uvec2& v) {
  return uvec2(k ^ v[0], k ^ v[1]);
}

constexpr uvec2
operator^(const uvec2& v, uint32 k) { return uvec2(k ^ v[0], k ^ v[1]); }

constexpr uvec2
operator^(const uvec2& a, const uvec2& b) {
  return uvec2(a[0] ^ b[0], a[1] ^ b[1]);
}

struct uvec3 : std::array<uint32, 3> {
  constexpr uvec3(uint32 v0, uint32 v1, uint32 v2)
      : std::array<uint32, 3>({v0, v1, v2}) {}
};

STATIC_ASSERT(sizeof(uvec3) == 3 * sizeof(uint32));

constexpr uvec3 operator+(uint32 k, const uvec3& v) {
  return uvec3(k + v[0], k + v[1], k + v[2]);
}

constexpr uvec3 operator+(const uvec3& v, uint32 k) {
  return uvec3(k + v[0], k + v[1], k + v[2]);
}

constexpr uvec3 operator+(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] + b[0], a[1] + b[1], a[2] + b[2]);
}

constexpr uvec3 operator-(uint32 k, const uvec3& v) {
  return uvec3(k - v[0], k - v[1], k - v[2]);
}

constexpr uvec3 operator-(const uvec3& v, uint32 k) {
  return uvec3(k - v[0], k - v[1], k - v[2]);
}

constexpr uvec3 operator-(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] - b[0], a[1] - b[1], a[2] - b[2]);
}

constexpr uvec3 operator*(uint32 k, const uvec3& v) {
  return uvec3(k * v[0], k * v[1], k * v[2]);
}

constexpr uvec3 operator*(const uvec3& v, uint32 k) {
  return uvec3(k * v[0], k * v[1], k * v[2]);
}

constexpr uvec3 operator*(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

constexpr uvec3 operator/(uint32 k, const uvec3& v) {
  return uvec3(k / v[0], k / v[1], k / v[2]);
}

constexpr uvec3 operator/(const uvec3& v, uint32 k) {
  return uvec3(k / v[0], k / v[1], k / v[2]);
}

constexpr uvec3 operator/(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

constexpr uvec3 operator%(uint32 k, const uvec3& v) {
  return uvec3(k % v[0], k % v[1], k % v[2]);
}

constexpr uvec3 operator%(const uvec3& v, uint32 k) {
  return uvec3(k % v[0], k % v[1], k % v[2]);
}

constexpr uvec3 operator%(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] % b[0], a[1] % b[1], a[2] % b[2]);
}

constexpr uvec3 operator&(uint32 k, const uvec3& v) {
  return uvec3(k & v[0], k & v[1], k & v[2]);
}

constexpr uvec3 operator&(const uvec3& v, uint32 k) {
  return uvec3(k & v[0], k & v[1], k & v[2]);
}

constexpr uvec3 operator&(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] & b[0], a[1] & b[1], a[2] & b[2]);
}

constexpr uvec3 operator|(uint32 k, const uvec3& v) {
  return uvec3(k | v[0], k | v[1], k | v[2]);
}

constexpr uvec3 operator|(const uvec3& v, uint32 k) {
  return uvec3(k | v[0], k | v[1], k | v[2]);
}

constexpr uvec3 operator|(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] | b[0], a[1] | b[1], a[2] | b[2]);
}

constexpr uvec3 operator^(uint32 k, const uvec3& v) {
  return uvec3(k ^ v[0], k ^ v[1], k ^ v[2]);
}

constexpr uvec3
operator^(const uvec3& v, uint32 k) {
  return uvec3(k ^ v[0], k ^ v[1], k ^ v[2]);
}

constexpr uvec3
operator^(const uvec3& a, const uvec3& b) {
  return uvec3(a[0] ^ b[0], a[1] ^ b[1], a[2] ^ b[2]);
}

struct uvec4 : std::array<uint32, 4> {
  constexpr uvec4(uint32 v0, uint32 v1, uint32 v2, uint32 v3)
      : std::array<uint32, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(uvec4) == 4 * sizeof(uint32));

constexpr uvec4 operator+(uint32 k, const uvec4& v) {
  return uvec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr uvec4 operator+(const uvec4& v, uint32 k) {
  return uvec4(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr uvec4 operator+(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr uvec4 operator-(uint32 k, const uvec4& v) {
  return uvec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr uvec4 operator-(const uvec4& v, uint32 k) {
  return uvec4(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr uvec4 operator-(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr uvec4 operator*(uint32 k, const uvec4& v) {
  return uvec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr uvec4 operator*(const uvec4& v, uint32 k) {
  return uvec4(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr uvec4 operator*(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr uvec4 operator/(uint32 k, const uvec4& v) {
  return uvec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr uvec4 operator/(const uvec4& v, uint32 k) {
  return uvec4(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr uvec4 operator/(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

constexpr uvec4 operator%(uint32 k, const uvec4& v) {
  return uvec4(k % v[0], k % v[1], k % v[2], k % v[3]);
}

constexpr uvec4 operator%(const uvec4& v, uint32 k) {
  return uvec4(k % v[0], k % v[1], k % v[2], k % v[3]);
}

constexpr uvec4 operator%(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] % b[0], a[1] % b[1], a[2] % b[2], a[3] % b[3]);
}

constexpr uvec4 operator&(uint32 k, const uvec4& v) {
  return uvec4(k & v[0], k & v[1], k & v[2], k & v[3]);
}

constexpr uvec4 operator&(const uvec4& v, uint32 k) {
  return uvec4(k & v[0], k & v[1], k & v[2], k & v[3]);
}

constexpr uvec4 operator&(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] & b[0], a[1] & b[1], a[2] & b[2], a[3] & b[3]);
}

constexpr uvec4 operator|(uint32 k, const uvec4& v) {
  return uvec4(k | v[0], k | v[1], k | v[2], k | v[3]);
}

constexpr uvec4 operator|(const uvec4& v, uint32 k) {
  return uvec4(k | v[0], k | v[1], k | v[2], k | v[3]);
}

constexpr uvec4 operator|(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] | b[0], a[1] | b[1], a[2] | b[2], a[3] | b[3]);
}

constexpr uvec4 operator^(uint32 k, const uvec4& v) {
  return uvec4(k ^ v[0], k ^ v[1], k ^ v[2], k ^ v[3]);
}

constexpr uvec4
operator^(const uvec4& v, uint32 k) {
  return uvec4(k ^ v[0], k ^ v[1], k ^ v[2], k ^ v[3]);
}

constexpr uvec4
operator^(const uvec4& a, const uvec4& b) {
  return uvec4(a[0] ^ b[0], a[1] ^ b[1], a[2] ^ b[2], a[3] ^ b[3]);
}

struct mat2x2 : std::array<float32, 4> {
  constexpr mat2x2(float32 v0, float32 v1, float32 v2, float32 v3)
      : std::array<float32, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(mat2x2) == 4 * sizeof(float32));

constexpr mat2x2 operator+(float32 k, const mat2x2& v) {
  return mat2x2(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr mat2x2 operator+(const mat2x2& v, float32 k) {
  return mat2x2(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr mat2x2 operator+(const mat2x2& a, const mat2x2& b) {
  return mat2x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr mat2x2 operator-(float32 k, const mat2x2& v) {
  return mat2x2(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr mat2x2 operator-(const mat2x2& v, float32 k) {
  return mat2x2(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr mat2x2 operator-(const mat2x2& a, const mat2x2& b) {
  return mat2x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr mat2x2 operator*(float32 k, const mat2x2& v) {
  return mat2x2(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr mat2x2 operator*(const mat2x2& v, float32 k) {
  return mat2x2(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr mat2x2 operator*(const mat2x2& a, const mat2x2& b) {
  return mat2x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr mat2x2 operator/(float32 k, const mat2x2& v) {
  return mat2x2(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr mat2x2 operator/(const mat2x2& v, float32 k) {
  return mat2x2(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr mat2x2 operator/(const mat2x2& a, const mat2x2& b) {
  return mat2x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

struct mat2x3 : std::array<float32, 6> {
  constexpr mat2x3(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5)
      : std::array<float32, 6>({v0, v1, v2, v3, v4, v5}) {}
};

STATIC_ASSERT(sizeof(mat2x3) == 6 * sizeof(float32));

constexpr mat2x3 operator+(float32 k, const mat2x3& v) {
  return mat2x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr mat2x3 operator+(const mat2x3& v, float32 k) {
  return mat2x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr mat2x3 operator+(const mat2x3& a, const mat2x3& b) {
  return mat2x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5]);
}

constexpr mat2x3 operator-(float32 k, const mat2x3& v) {
  return mat2x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr mat2x3 operator-(const mat2x3& v, float32 k) {
  return mat2x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr mat2x3 operator-(const mat2x3& a, const mat2x3& b) {
  return mat2x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5]);
}

constexpr mat2x3 operator*(float32 k, const mat2x3& v) {
  return mat2x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr mat2x3 operator*(const mat2x3& v, float32 k) {
  return mat2x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr mat2x3 operator*(const mat2x3& a, const mat2x3& b) {
  return mat2x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5]);
}

constexpr mat2x3 operator/(float32 k, const mat2x3& v) {
  return mat2x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr mat2x3 operator/(const mat2x3& v, float32 k) {
  return mat2x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr mat2x3 operator/(const mat2x3& a, const mat2x3& b) {
  return mat2x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5]);
}

struct mat2x4 : std::array<float32, 8> {
  constexpr mat2x4(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7)
      : std::array<float32, 8>({v0, v1, v2, v3, v4, v5, v6, v7}) {}
};

STATIC_ASSERT(sizeof(mat2x4) == 8 * sizeof(float32));

constexpr mat2x4 operator+(float32 k, const mat2x4& v) {
  return mat2x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7]);
}

constexpr mat2x4 operator+(const mat2x4& v, float32 k) {
  return mat2x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7]);
}

constexpr mat2x4 operator+(const mat2x4& a, const mat2x4& b) {
  return mat2x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7]);
}

constexpr mat2x4 operator-(float32 k, const mat2x4& v) {
  return mat2x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7]);
}

constexpr mat2x4 operator-(const mat2x4& v, float32 k) {
  return mat2x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7]);
}

constexpr mat2x4 operator-(const mat2x4& a, const mat2x4& b) {
  return mat2x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7]);
}

constexpr mat2x4 operator*(float32 k, const mat2x4& v) {
  return mat2x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7]);
}

constexpr mat2x4 operator*(const mat2x4& v, float32 k) {
  return mat2x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7]);
}

constexpr mat2x4 operator*(const mat2x4& a, const mat2x4& b) {
  return mat2x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7]);
}

constexpr mat2x4 operator/(float32 k, const mat2x4& v) {
  return mat2x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7]);
}

constexpr mat2x4 operator/(const mat2x4& v, float32 k) {
  return mat2x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7]);
}

constexpr mat2x4 operator/(const mat2x4& a, const mat2x4& b) {
  return mat2x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7]);
}

struct mat3x2 : std::array<float32, 6> {
  constexpr mat3x2(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5)
      : std::array<float32, 6>({v0, v1, v2, v3, v4, v5}) {}
};

STATIC_ASSERT(sizeof(mat3x2) == 6 * sizeof(float32));

constexpr mat3x2 operator+(float32 k, const mat3x2& v) {
  return mat3x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr mat3x2 operator+(const mat3x2& v, float32 k) {
  return mat3x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr mat3x2 operator+(const mat3x2& a, const mat3x2& b) {
  return mat3x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5]);
}

constexpr mat3x2 operator-(float32 k, const mat3x2& v) {
  return mat3x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr mat3x2 operator-(const mat3x2& v, float32 k) {
  return mat3x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr mat3x2 operator-(const mat3x2& a, const mat3x2& b) {
  return mat3x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5]);
}

constexpr mat3x2 operator*(float32 k, const mat3x2& v) {
  return mat3x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr mat3x2 operator*(const mat3x2& v, float32 k) {
  return mat3x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr mat3x2 operator*(const mat3x2& a, const mat3x2& b) {
  return mat3x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5]);
}

constexpr mat3x2 operator/(float32 k, const mat3x2& v) {
  return mat3x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr mat3x2 operator/(const mat3x2& v, float32 k) {
  return mat3x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr mat3x2 operator/(const mat3x2& a, const mat3x2& b) {
  return mat3x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5]);
}

struct mat3x3 : std::array<float32, 9> {
  constexpr mat3x3(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7, float32 v8)
      : std::array<float32, 9>({v0, v1, v2, v3, v4, v5, v6, v7, v8}) {}
};

STATIC_ASSERT(sizeof(mat3x3) == 9 * sizeof(float32));

constexpr mat3x3 operator+(float32 k, const mat3x3& v) {
  return mat3x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8]);
}

constexpr mat3x3 operator+(const mat3x3& v, float32 k) {
  return mat3x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8]);
}

constexpr mat3x3 operator+(const mat3x3& a, const mat3x3& b) {
  return mat3x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7], a[8] + b[8]);
}

constexpr mat3x3 operator-(float32 k, const mat3x3& v) {
  return mat3x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8]);
}

constexpr mat3x3 operator-(const mat3x3& v, float32 k) {
  return mat3x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8]);
}

constexpr mat3x3 operator-(const mat3x3& a, const mat3x3& b) {
  return mat3x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7], a[8] - b[8]);
}

constexpr mat3x3 operator*(float32 k, const mat3x3& v) {
  return mat3x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8]);
}

constexpr mat3x3 operator*(const mat3x3& v, float32 k) {
  return mat3x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8]);
}

constexpr mat3x3 operator*(const mat3x3& a, const mat3x3& b) {
  return mat3x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7], a[8] * b[8]);
}

constexpr mat3x3 operator/(float32 k, const mat3x3& v) {
  return mat3x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8]);
}

constexpr mat3x3 operator/(const mat3x3& v, float32 k) {
  return mat3x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8]);
}

constexpr mat3x3 operator/(const mat3x3& a, const mat3x3& b) {
  return mat3x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7], a[8] / b[8]);
}

struct mat3x4 : std::array<float32, 12> {
  constexpr mat3x4(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7, float32 v8, float32 v9,
                   float32 v10, float32 v11)
      : std::array<float32, 12>(
            {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11}) {}
};

STATIC_ASSERT(sizeof(mat3x4) == 12 * sizeof(float32));

constexpr mat3x4 operator+(float32 k, const mat3x4& v) {
  return mat3x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr mat3x4 operator+(const mat3x4& v, float32 k) {
  return mat3x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr mat3x4 operator+(const mat3x4& a, const mat3x4& b) {
  return mat3x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7], a[8] + b[8], a[9] + b[9],
                a[10] + b[10], a[11] + b[11]);
}

constexpr mat3x4 operator-(float32 k, const mat3x4& v) {
  return mat3x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr mat3x4 operator-(const mat3x4& v, float32 k) {
  return mat3x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr mat3x4 operator-(const mat3x4& a, const mat3x4& b) {
  return mat3x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7], a[8] - b[8], a[9] - b[9],
                a[10] - b[10], a[11] - b[11]);
}

constexpr mat3x4 operator*(float32 k, const mat3x4& v) {
  return mat3x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr mat3x4 operator*(const mat3x4& v, float32 k) {
  return mat3x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr mat3x4 operator*(const mat3x4& a, const mat3x4& b) {
  return mat3x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7], a[8] * b[8], a[9] * b[9],
                a[10] * b[10], a[11] * b[11]);
}

constexpr mat3x4 operator/(float32 k, const mat3x4& v) {
  return mat3x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr mat3x4 operator/(const mat3x4& v, float32 k) {
  return mat3x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr mat3x4 operator/(const mat3x4& a, const mat3x4& b) {
  return mat3x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7], a[8] / b[8], a[9] / b[9],
                a[10] / b[10], a[11] / b[11]);
}

struct mat4x2 : std::array<float32, 8> {
  constexpr mat4x2(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7)
      : std::array<float32, 8>({v0, v1, v2, v3, v4, v5, v6, v7}) {}
};

STATIC_ASSERT(sizeof(mat4x2) == 8 * sizeof(float32));

constexpr mat4x2 operator+(float32 k, const mat4x2& v) {
  return mat4x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7]);
}

constexpr mat4x2 operator+(const mat4x2& v, float32 k) {
  return mat4x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7]);
}

constexpr mat4x2 operator+(const mat4x2& a, const mat4x2& b) {
  return mat4x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7]);
}

constexpr mat4x2 operator-(float32 k, const mat4x2& v) {
  return mat4x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7]);
}

constexpr mat4x2 operator-(const mat4x2& v, float32 k) {
  return mat4x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7]);
}

constexpr mat4x2 operator-(const mat4x2& a, const mat4x2& b) {
  return mat4x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7]);
}

constexpr mat4x2 operator*(float32 k, const mat4x2& v) {
  return mat4x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7]);
}

constexpr mat4x2 operator*(const mat4x2& v, float32 k) {
  return mat4x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7]);
}

constexpr mat4x2 operator*(const mat4x2& a, const mat4x2& b) {
  return mat4x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7]);
}

constexpr mat4x2 operator/(float32 k, const mat4x2& v) {
  return mat4x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7]);
}

constexpr mat4x2 operator/(const mat4x2& v, float32 k) {
  return mat4x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7]);
}

constexpr mat4x2 operator/(const mat4x2& a, const mat4x2& b) {
  return mat4x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7]);
}

struct mat4x3 : std::array<float32, 12> {
  constexpr mat4x3(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7, float32 v8, float32 v9,
                   float32 v10, float32 v11)
      : std::array<float32, 12>(
            {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11}) {}
};

STATIC_ASSERT(sizeof(mat4x3) == 12 * sizeof(float32));

constexpr mat4x3 operator+(float32 k, const mat4x3& v) {
  return mat4x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr mat4x3 operator+(const mat4x3& v, float32 k) {
  return mat4x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr mat4x3 operator+(const mat4x3& a, const mat4x3& b) {
  return mat4x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7], a[8] + b[8], a[9] + b[9],
                a[10] + b[10], a[11] + b[11]);
}

constexpr mat4x3 operator-(float32 k, const mat4x3& v) {
  return mat4x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr mat4x3 operator-(const mat4x3& v, float32 k) {
  return mat4x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr mat4x3 operator-(const mat4x3& a, const mat4x3& b) {
  return mat4x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7], a[8] - b[8], a[9] - b[9],
                a[10] - b[10], a[11] - b[11]);
}

constexpr mat4x3 operator*(float32 k, const mat4x3& v) {
  return mat4x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr mat4x3 operator*(const mat4x3& v, float32 k) {
  return mat4x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr mat4x3 operator*(const mat4x3& a, const mat4x3& b) {
  return mat4x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7], a[8] * b[8], a[9] * b[9],
                a[10] * b[10], a[11] * b[11]);
}

constexpr mat4x3 operator/(float32 k, const mat4x3& v) {
  return mat4x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr mat4x3 operator/(const mat4x3& v, float32 k) {
  return mat4x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr mat4x3 operator/(const mat4x3& a, const mat4x3& b) {
  return mat4x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7], a[8] / b[8], a[9] / b[9],
                a[10] / b[10], a[11] / b[11]);
}

struct mat4x4 : std::array<float32, 16> {
  constexpr mat4x4(float32 v0, float32 v1, float32 v2, float32 v3, float32 v4,
                   float32 v5, float32 v6, float32 v7, float32 v8, float32 v9,
                   float32 v10, float32 v11, float32 v12, float32 v13,
                   float32 v14, float32 v15)
      : std::array<float32, 16>({v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
                                 v11, v12, v13, v14, v15}) {}
};

STATIC_ASSERT(sizeof(mat4x4) == 16 * sizeof(float32));

constexpr mat4x4 operator+(float32 k, const mat4x4& v) {
  return mat4x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11],
                k + v[12], k + v[13], k + v[14], k + v[15]);
}

constexpr mat4x4 operator+(const mat4x4& v, float32 k) {
  return mat4x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11],
                k + v[12], k + v[13], k + v[14], k + v[15]);
}

constexpr mat4x4 operator+(const mat4x4& a, const mat4x4& b) {
  return mat4x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3], a[4] + b[4],
                a[5] + b[5], a[6] + b[6], a[7] + b[7], a[8] + b[8], a[9] + b[9],
                a[10] + b[10], a[11] + b[11], a[12] + b[12], a[13] + b[13],
                a[14] + b[14], a[15] + b[15]);
}

constexpr mat4x4 operator-(float32 k, const mat4x4& v) {
  return mat4x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11],
                k - v[12], k - v[13], k - v[14], k - v[15]);
}

constexpr mat4x4 operator-(const mat4x4& v, float32 k) {
  return mat4x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11],
                k - v[12], k - v[13], k - v[14], k - v[15]);
}

constexpr mat4x4 operator-(const mat4x4& a, const mat4x4& b) {
  return mat4x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3], a[4] - b[4],
                a[5] - b[5], a[6] - b[6], a[7] - b[7], a[8] - b[8], a[9] - b[9],
                a[10] - b[10], a[11] - b[11], a[12] - b[12], a[13] - b[13],
                a[14] - b[14], a[15] - b[15]);
}

constexpr mat4x4 operator*(float32 k, const mat4x4& v) {
  return mat4x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11],
                k * v[12], k * v[13], k * v[14], k * v[15]);
}

constexpr mat4x4 operator*(const mat4x4& v, float32 k) {
  return mat4x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11],
                k * v[12], k * v[13], k * v[14], k * v[15]);
}

constexpr mat4x4 operator*(const mat4x4& a, const mat4x4& b) {
  return mat4x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3], a[4] * b[4],
                a[5] * b[5], a[6] * b[6], a[7] * b[7], a[8] * b[8], a[9] * b[9],
                a[10] * b[10], a[11] * b[11], a[12] * b[12], a[13] * b[13],
                a[14] * b[14], a[15] * b[15]);
}

constexpr mat4x4 operator/(float32 k, const mat4x4& v) {
  return mat4x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11],
                k / v[12], k / v[13], k / v[14], k / v[15]);
}

constexpr mat4x4 operator/(const mat4x4& v, float32 k) {
  return mat4x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11],
                k / v[12], k / v[13], k / v[14], k / v[15]);
}

constexpr mat4x4 operator/(const mat4x4& a, const mat4x4& b) {
  return mat4x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3], a[4] / b[4],
                a[5] / b[5], a[6] / b[6], a[7] / b[7], a[8] / b[8], a[9] / b[9],
                a[10] / b[10], a[11] / b[11], a[12] / b[12], a[13] / b[13],
                a[14] / b[14], a[15] / b[15]);
}

using mat2 = mat2x2;
using mat3 = mat3x3;
using mat4 = mat4x4;

struct dmat2x2 : std::array<float64, 4> {
  constexpr dmat2x2(float64 v0, float64 v1, float64 v2, float64 v3)
      : std::array<float64, 4>({v0, v1, v2, v3}) {}
};

STATIC_ASSERT(sizeof(dmat2x2) == 4 * sizeof(float64));

constexpr dmat2x2 operator+(float64 k, const dmat2x2& v) {
  return dmat2x2(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr dmat2x2 operator+(const dmat2x2& v, float64 k) {
  return dmat2x2(k + v[0], k + v[1], k + v[2], k + v[3]);
}

constexpr dmat2x2 operator+(const dmat2x2& a, const dmat2x2& b) {
  return dmat2x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
}

constexpr dmat2x2 operator-(float64 k, const dmat2x2& v) {
  return dmat2x2(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr dmat2x2 operator-(const dmat2x2& v, float64 k) {
  return dmat2x2(k - v[0], k - v[1], k - v[2], k - v[3]);
}

constexpr dmat2x2 operator-(const dmat2x2& a, const dmat2x2& b) {
  return dmat2x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
}

constexpr dmat2x2 operator*(float64 k, const dmat2x2& v) {
  return dmat2x2(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr dmat2x2 operator*(const dmat2x2& v, float64 k) {
  return dmat2x2(k * v[0], k * v[1], k * v[2], k * v[3]);
}

constexpr dmat2x2 operator*(const dmat2x2& a, const dmat2x2& b) {
  return dmat2x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
}

constexpr dmat2x2 operator/(float64 k, const dmat2x2& v) {
  return dmat2x2(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr dmat2x2 operator/(const dmat2x2& v, float64 k) {
  return dmat2x2(k / v[0], k / v[1], k / v[2], k / v[3]);
}

constexpr dmat2x2 operator/(const dmat2x2& a, const dmat2x2& b) {
  return dmat2x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]);
}

struct dmat2x3 : std::array<float64, 6> {
  constexpr dmat2x3(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5)
      : std::array<float64, 6>({v0, v1, v2, v3, v4, v5}) {}
};

STATIC_ASSERT(sizeof(dmat2x3) == 6 * sizeof(float64));

constexpr dmat2x3 operator+(float64 k, const dmat2x3& v) {
  return dmat2x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr dmat2x3 operator+(const dmat2x3& v, float64 k) {
  return dmat2x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr dmat2x3 operator+(const dmat2x3& a, const dmat2x3& b) {
  return dmat2x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5]);
}

constexpr dmat2x3 operator-(float64 k, const dmat2x3& v) {
  return dmat2x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr dmat2x3 operator-(const dmat2x3& v, float64 k) {
  return dmat2x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr dmat2x3 operator-(const dmat2x3& a, const dmat2x3& b) {
  return dmat2x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5]);
}

constexpr dmat2x3 operator*(float64 k, const dmat2x3& v) {
  return dmat2x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr dmat2x3 operator*(const dmat2x3& v, float64 k) {
  return dmat2x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr dmat2x3 operator*(const dmat2x3& a, const dmat2x3& b) {
  return dmat2x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5]);
}

constexpr dmat2x3 operator/(float64 k, const dmat2x3& v) {
  return dmat2x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr dmat2x3 operator/(const dmat2x3& v, float64 k) {
  return dmat2x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr dmat2x3 operator/(const dmat2x3& a, const dmat2x3& b) {
  return dmat2x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5]);
}

struct dmat2x4 : std::array<float64, 8> {
  constexpr dmat2x4(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7)
      : std::array<float64, 8>({v0, v1, v2, v3, v4, v5, v6, v7}) {}
};

STATIC_ASSERT(sizeof(dmat2x4) == 8 * sizeof(float64));

constexpr dmat2x4 operator+(float64 k, const dmat2x4& v) {
  return dmat2x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7]);
}

constexpr dmat2x4 operator+(const dmat2x4& v, float64 k) {
  return dmat2x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7]);
}

constexpr dmat2x4 operator+(const dmat2x4& a, const dmat2x4& b) {
  return dmat2x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7]);
}

constexpr dmat2x4 operator-(float64 k, const dmat2x4& v) {
  return dmat2x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7]);
}

constexpr dmat2x4 operator-(const dmat2x4& v, float64 k) {
  return dmat2x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7]);
}

constexpr dmat2x4 operator-(const dmat2x4& a, const dmat2x4& b) {
  return dmat2x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7]);
}

constexpr dmat2x4 operator*(float64 k, const dmat2x4& v) {
  return dmat2x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7]);
}

constexpr dmat2x4 operator*(const dmat2x4& v, float64 k) {
  return dmat2x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7]);
}

constexpr dmat2x4 operator*(const dmat2x4& a, const dmat2x4& b) {
  return dmat2x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7]);
}

constexpr dmat2x4 operator/(float64 k, const dmat2x4& v) {
  return dmat2x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7]);
}

constexpr dmat2x4 operator/(const dmat2x4& v, float64 k) {
  return dmat2x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7]);
}

constexpr dmat2x4 operator/(const dmat2x4& a, const dmat2x4& b) {
  return dmat2x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7]);
}

struct dmat3x2 : std::array<float64, 6> {
  constexpr dmat3x2(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5)
      : std::array<float64, 6>({v0, v1, v2, v3, v4, v5}) {}
};

STATIC_ASSERT(sizeof(dmat3x2) == 6 * sizeof(float64));

constexpr dmat3x2 operator+(float64 k, const dmat3x2& v) {
  return dmat3x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr dmat3x2 operator+(const dmat3x2& v, float64 k) {
  return dmat3x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5]);
}

constexpr dmat3x2 operator+(const dmat3x2& a, const dmat3x2& b) {
  return dmat3x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5]);
}

constexpr dmat3x2 operator-(float64 k, const dmat3x2& v) {
  return dmat3x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr dmat3x2 operator-(const dmat3x2& v, float64 k) {
  return dmat3x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5]);
}

constexpr dmat3x2 operator-(const dmat3x2& a, const dmat3x2& b) {
  return dmat3x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5]);
}

constexpr dmat3x2 operator*(float64 k, const dmat3x2& v) {
  return dmat3x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr dmat3x2 operator*(const dmat3x2& v, float64 k) {
  return dmat3x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5]);
}

constexpr dmat3x2 operator*(const dmat3x2& a, const dmat3x2& b) {
  return dmat3x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5]);
}

constexpr dmat3x2 operator/(float64 k, const dmat3x2& v) {
  return dmat3x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr dmat3x2 operator/(const dmat3x2& v, float64 k) {
  return dmat3x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5]);
}

constexpr dmat3x2 operator/(const dmat3x2& a, const dmat3x2& b) {
  return dmat3x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5]);
}

struct dmat3x3 : std::array<float64, 9> {
  constexpr dmat3x3(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7, float64 v8)
      : std::array<float64, 9>({v0, v1, v2, v3, v4, v5, v6, v7, v8}) {}
};

STATIC_ASSERT(sizeof(dmat3x3) == 9 * sizeof(float64));

constexpr dmat3x3 operator+(float64 k, const dmat3x3& v) {
  return dmat3x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8]);
}

constexpr dmat3x3 operator+(const dmat3x3& v, float64 k) {
  return dmat3x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8]);
}

constexpr dmat3x3 operator+(const dmat3x3& a, const dmat3x3& b) {
  return dmat3x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7],
                 a[8] + b[8]);
}

constexpr dmat3x3 operator-(float64 k, const dmat3x3& v) {
  return dmat3x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8]);
}

constexpr dmat3x3 operator-(const dmat3x3& v, float64 k) {
  return dmat3x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8]);
}

constexpr dmat3x3 operator-(const dmat3x3& a, const dmat3x3& b) {
  return dmat3x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7],
                 a[8] - b[8]);
}

constexpr dmat3x3 operator*(float64 k, const dmat3x3& v) {
  return dmat3x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8]);
}

constexpr dmat3x3 operator*(const dmat3x3& v, float64 k) {
  return dmat3x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8]);
}

constexpr dmat3x3 operator*(const dmat3x3& a, const dmat3x3& b) {
  return dmat3x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7],
                 a[8] * b[8]);
}

constexpr dmat3x3 operator/(float64 k, const dmat3x3& v) {
  return dmat3x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8]);
}

constexpr dmat3x3 operator/(const dmat3x3& v, float64 k) {
  return dmat3x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8]);
}

constexpr dmat3x3 operator/(const dmat3x3& a, const dmat3x3& b) {
  return dmat3x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7],
                 a[8] / b[8]);
}

struct dmat3x4 : std::array<float64, 12> {
  constexpr dmat3x4(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7, float64 v8, float64 v9,
                    float64 v10, float64 v11)
      : std::array<float64, 12>(
            {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11}) {}
};

STATIC_ASSERT(sizeof(dmat3x4) == 12 * sizeof(float64));

constexpr dmat3x4 operator+(float64 k, const dmat3x4& v) {
  return dmat3x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr dmat3x4 operator+(const dmat3x4& v, float64 k) {
  return dmat3x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr dmat3x4 operator+(const dmat3x4& a, const dmat3x4& b) {
  return dmat3x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7],
                 a[8] + b[8], a[9] + b[9], a[10] + b[10], a[11] + b[11]);
}

constexpr dmat3x4 operator-(float64 k, const dmat3x4& v) {
  return dmat3x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr dmat3x4 operator-(const dmat3x4& v, float64 k) {
  return dmat3x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr dmat3x4 operator-(const dmat3x4& a, const dmat3x4& b) {
  return dmat3x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7],
                 a[8] - b[8], a[9] - b[9], a[10] - b[10], a[11] - b[11]);
}

constexpr dmat3x4 operator*(float64 k, const dmat3x4& v) {
  return dmat3x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr dmat3x4 operator*(const dmat3x4& v, float64 k) {
  return dmat3x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr dmat3x4 operator*(const dmat3x4& a, const dmat3x4& b) {
  return dmat3x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7],
                 a[8] * b[8], a[9] * b[9], a[10] * b[10], a[11] * b[11]);
}

constexpr dmat3x4 operator/(float64 k, const dmat3x4& v) {
  return dmat3x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr dmat3x4 operator/(const dmat3x4& v, float64 k) {
  return dmat3x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr dmat3x4 operator/(const dmat3x4& a, const dmat3x4& b) {
  return dmat3x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7],
                 a[8] / b[8], a[9] / b[9], a[10] / b[10], a[11] / b[11]);
}

struct dmat4x2 : std::array<float64, 8> {
  constexpr dmat4x2(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7)
      : std::array<float64, 8>({v0, v1, v2, v3, v4, v5, v6, v7}) {}
};

STATIC_ASSERT(sizeof(dmat4x2) == 8 * sizeof(float64));

constexpr dmat4x2 operator+(float64 k, const dmat4x2& v) {
  return dmat4x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7]);
}

constexpr dmat4x2 operator+(const dmat4x2& v, float64 k) {
  return dmat4x2(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7]);
}

constexpr dmat4x2 operator+(const dmat4x2& a, const dmat4x2& b) {
  return dmat4x2(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7]);
}

constexpr dmat4x2 operator-(float64 k, const dmat4x2& v) {
  return dmat4x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7]);
}

constexpr dmat4x2 operator-(const dmat4x2& v, float64 k) {
  return dmat4x2(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7]);
}

constexpr dmat4x2 operator-(const dmat4x2& a, const dmat4x2& b) {
  return dmat4x2(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7]);
}

constexpr dmat4x2 operator*(float64 k, const dmat4x2& v) {
  return dmat4x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7]);
}

constexpr dmat4x2 operator*(const dmat4x2& v, float64 k) {
  return dmat4x2(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7]);
}

constexpr dmat4x2 operator*(const dmat4x2& a, const dmat4x2& b) {
  return dmat4x2(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7]);
}

constexpr dmat4x2 operator/(float64 k, const dmat4x2& v) {
  return dmat4x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7]);
}

constexpr dmat4x2 operator/(const dmat4x2& v, float64 k) {
  return dmat4x2(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7]);
}

constexpr dmat4x2 operator/(const dmat4x2& a, const dmat4x2& b) {
  return dmat4x2(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7]);
}

struct dmat4x3 : std::array<float64, 12> {
  constexpr dmat4x3(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7, float64 v8, float64 v9,
                    float64 v10, float64 v11)
      : std::array<float64, 12>(
            {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11}) {}
};

STATIC_ASSERT(sizeof(dmat4x3) == 12 * sizeof(float64));

constexpr dmat4x3 operator+(float64 k, const dmat4x3& v) {
  return dmat4x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr dmat4x3 operator+(const dmat4x3& v, float64 k) {
  return dmat4x3(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11]);
}

constexpr dmat4x3 operator+(const dmat4x3& a, const dmat4x3& b) {
  return dmat4x3(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7],
                 a[8] + b[8], a[9] + b[9], a[10] + b[10], a[11] + b[11]);
}

constexpr dmat4x3 operator-(float64 k, const dmat4x3& v) {
  return dmat4x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr dmat4x3 operator-(const dmat4x3& v, float64 k) {
  return dmat4x3(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11]);
}

constexpr dmat4x3 operator-(const dmat4x3& a, const dmat4x3& b) {
  return dmat4x3(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7],
                 a[8] - b[8], a[9] - b[9], a[10] - b[10], a[11] - b[11]);
}

constexpr dmat4x3 operator*(float64 k, const dmat4x3& v) {
  return dmat4x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr dmat4x3 operator*(const dmat4x3& v, float64 k) {
  return dmat4x3(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11]);
}

constexpr dmat4x3 operator*(const dmat4x3& a, const dmat4x3& b) {
  return dmat4x3(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7],
                 a[8] * b[8], a[9] * b[9], a[10] * b[10], a[11] * b[11]);
}

constexpr dmat4x3 operator/(float64 k, const dmat4x3& v) {
  return dmat4x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr dmat4x3 operator/(const dmat4x3& v, float64 k) {
  return dmat4x3(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11]);
}

constexpr dmat4x3 operator/(const dmat4x3& a, const dmat4x3& b) {
  return dmat4x3(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7],
                 a[8] / b[8], a[9] / b[9], a[10] / b[10], a[11] / b[11]);
}

struct dmat4x4 : std::array<float64, 16> {
  constexpr dmat4x4(float64 v0, float64 v1, float64 v2, float64 v3, float64 v4,
                    float64 v5, float64 v6, float64 v7, float64 v8, float64 v9,
                    float64 v10, float64 v11, float64 v12, float64 v13,
                    float64 v14, float64 v15)
      : std::array<float64, 16>({v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
                                 v11, v12, v13, v14, v15}) {}
};

STATIC_ASSERT(sizeof(dmat4x4) == 16 * sizeof(float64));

constexpr dmat4x4 operator+(float64 k, const dmat4x4& v) {
  return dmat4x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11],
                 k + v[12], k + v[13], k + v[14], k + v[15]);
}

constexpr dmat4x4 operator+(const dmat4x4& v, float64 k) {
  return dmat4x4(k + v[0], k + v[1], k + v[2], k + v[3], k + v[4], k + v[5],
                 k + v[6], k + v[7], k + v[8], k + v[9], k + v[10], k + v[11],
                 k + v[12], k + v[13], k + v[14], k + v[15]);
}

constexpr dmat4x4 operator+(const dmat4x4& a, const dmat4x4& b) {
  return dmat4x4(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3],
                 a[4] + b[4], a[5] + b[5], a[6] + b[6], a[7] + b[7],
                 a[8] + b[8], a[9] + b[9], a[10] + b[10], a[11] + b[11],
                 a[12] + b[12], a[13] + b[13], a[14] + b[14], a[15] + b[15]);
}

constexpr dmat4x4 operator-(float64 k, const dmat4x4& v) {
  return dmat4x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11],
                 k - v[12], k - v[13], k - v[14], k - v[15]);
}

constexpr dmat4x4 operator-(const dmat4x4& v, float64 k) {
  return dmat4x4(k - v[0], k - v[1], k - v[2], k - v[3], k - v[4], k - v[5],
                 k - v[6], k - v[7], k - v[8], k - v[9], k - v[10], k - v[11],
                 k - v[12], k - v[13], k - v[14], k - v[15]);
}

constexpr dmat4x4 operator-(const dmat4x4& a, const dmat4x4& b) {
  return dmat4x4(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3],
                 a[4] - b[4], a[5] - b[5], a[6] - b[6], a[7] - b[7],
                 a[8] - b[8], a[9] - b[9], a[10] - b[10], a[11] - b[11],
                 a[12] - b[12], a[13] - b[13], a[14] - b[14], a[15] - b[15]);
}

constexpr dmat4x4 operator*(float64 k, const dmat4x4& v) {
  return dmat4x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11],
                 k * v[12], k * v[13], k * v[14], k * v[15]);
}

constexpr dmat4x4 operator*(const dmat4x4& v, float64 k) {
  return dmat4x4(k * v[0], k * v[1], k * v[2], k * v[3], k * v[4], k * v[5],
                 k * v[6], k * v[7], k * v[8], k * v[9], k * v[10], k * v[11],
                 k * v[12], k * v[13], k * v[14], k * v[15]);
}

constexpr dmat4x4 operator*(const dmat4x4& a, const dmat4x4& b) {
  return dmat4x4(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3],
                 a[4] * b[4], a[5] * b[5], a[6] * b[6], a[7] * b[7],
                 a[8] * b[8], a[9] * b[9], a[10] * b[10], a[11] * b[11],
                 a[12] * b[12], a[13] * b[13], a[14] * b[14], a[15] * b[15]);
}

constexpr dmat4x4 operator/(float64 k, const dmat4x4& v) {
  return dmat4x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11],
                 k / v[12], k / v[13], k / v[14], k / v[15]);
}

constexpr dmat4x4 operator/(const dmat4x4& v, float64 k) {
  return dmat4x4(k / v[0], k / v[1], k / v[2], k / v[3], k / v[4], k / v[5],
                 k / v[6], k / v[7], k / v[8], k / v[9], k / v[10], k / v[11],
                 k / v[12], k / v[13], k / v[14], k / v[15]);
}

constexpr dmat4x4 operator/(const dmat4x4& a, const dmat4x4& b) {
  return dmat4x4(a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3],
                 a[4] / b[4], a[5] / b[5], a[6] / b[6], a[7] / b[7],
                 a[8] / b[8], a[9] / b[9], a[10] / b[10], a[11] / b[11],
                 a[12] / b[12], a[13] / b[13], a[14] / b[14], a[15] / b[15]);
}

using dmat2 = dmat2x2;
using dmat3 = dmat3x3;
using dmat4 = dmat4x4;

}  // namespace gl
