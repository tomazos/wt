#include "gl/geometry.h"

#include "gtest/gtest.h"

namespace gl {

#define TEST_VECTOR(vec, type, N, ...)            \
  {                                               \
    type A[N] = {__VA_ARGS__};                    \
    vec B;                                        \
    B << __VA_ARGS__;                             \
    vec C(__VA_ARGS__);                           \
    EXPECT_EQ(sizeof(A), sizeof(B));              \
    EXPECT_EQ(std::memcmp(&A, &B, sizeof(A)), 0); \
    EXPECT_EQ(std::memcmp(&A, &C, sizeof(A)), 0); \
  }

TEST(GeometryTest, Vectors) {
  TEST_VECTOR(vec2, float32, 2, 2.1, 2.2);
  TEST_VECTOR(vec3, float32, 3, 2.1, 2.2, 2.3);
  TEST_VECTOR(vec4, float32, 4, 2.1, 2.2, 2.3, 2.4);
  TEST_VECTOR(dvec2, float64, 2, 2.1, 2.2);
  TEST_VECTOR(dvec3, float64, 3, 2.1, 2.2, 2.3);
  TEST_VECTOR(dvec4, float64, 4, 2.1, 2.2, 2.3, 2.4);
  TEST_VECTOR(bvec2, bool, 2, true, false);
  TEST_VECTOR(bvec3, bool, 3, true, false, false);
  TEST_VECTOR(bvec4, bool, 4, true, false, false, true);
  TEST_VECTOR(ivec2, int32, 2, 41, -42);
  TEST_VECTOR(ivec3, int32, 3, 41, -42, 43);
  TEST_VECTOR(ivec4, int32, 4, 41, -42, 43, -44);
  TEST_VECTOR(uvec2, uint32, 2, 41, 42);
  TEST_VECTOR(uvec3, uint32, 3, 41, 42, 43);
  TEST_VECTOR(uvec4, uint32, 4, 41, 42, 43, 44);
}

#define TEST_MATRIX(matb, matc, type, N, ...)     \
  {                                               \
    type A[N] = {__VA_ARGS__};                    \
    matb B;                                       \
    B << __VA_ARGS__;                             \
    matc C = B.transpose();                       \
    EXPECT_EQ(sizeof(A), sizeof(C));              \
    EXPECT_EQ(std::memcmp(&A, &C, sizeof(A)), 0); \
  }

TEST(GeometryTest, Matrixes) {
  TEST_MATRIX(mat2x2, mat2x2, float32, 4, 2.1, 2.2, 2.3, 2.4);
  TEST_MATRIX(mat3x2, mat2x3, float32, 6, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6);
  TEST_MATRIX(mat4x2, mat2x4, float32, 8, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8);
  TEST_MATRIX(mat2x3, mat3x2, float32, 6, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6);
  TEST_MATRIX(mat3x3, mat3x3, float32, 9, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8, 2.9);
  TEST_MATRIX(mat4x3, mat3x4, float32, 12, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8, 2.9, 3.0, 3.1, 3.2);
  TEST_MATRIX(mat2x4, mat4x2, float32, 8, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8);
  TEST_MATRIX(mat3x4, mat4x3, float32, 12, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8, 2.9, 3.0, 3.1, 3.2);
  TEST_MATRIX(mat4x4, mat4x4, float32, 16, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7,
              2.8, 2.9, 3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6);
}

TEST(GeometryTest, Dot) {
  EXPECT_EQ(dot(vec2(1, 2), vec2(2, 3)), 8);
  EXPECT_EQ(dot(vec3(1, 2, 3), vec3(2, 3, 4)), 20);
  EXPECT_EQ(dot(vec4(1, 2, 3, 4), vec4(2, 3, 4, 5)), 40);
}

TEST(GeometryTest, Cross) {
  EXPECT_TRUE(cross(vec3(1, 2, 3), vec3(4, 5, 6)) == vec3(-3, 6, -3));
}

TEST(GeometryTest, Length) { EXPECT_EQ(length(vec2(3, 4)), 5); }

TEST(GeometryTest, Normalize) {
  EXPECT_TRUE(normalize(vec2(3, 4)) == vec2(3.0 / 5, 4.0 / 5));
}

TEST(GeometryTest, Angle) {
  EXPECT_FLOAT_EQ(angle(vec2(1, 0), vec2(0, 1)), M_PI / 2);
  EXPECT_FLOAT_EQ(angle(vec2(5, 0), vec2(0, 5)), M_PI / 2);
  EXPECT_FLOAT_EQ(angle(vec2(5, 0), vec2(0, 5)), M_PI / 2);
  EXPECT_NEAR(angle(vec2(5, 3), vec2(-5, -3)), M_PI, 0.00001);
}

TEST(GeometryTest, Projection) {
  EXPECT_TRUE(projection(vec2(1, 0), vec2(0, 1)) == vec2(0, 0));
  EXPECT_TRUE(projection(vec2(5, 0), vec2(5, 0)) == vec2(5, 0));
  EXPECT_EQ(projection(vec2(5, 6), vec2(5, 0))(0), 5);
  EXPECT_EQ(projection(vec2(5, 6), vec2(5, 0))(1), 0);
  EXPECT_TRUE(projection(vec2(5, 5), vec2(5, 0)) == vec2(5, 0));
}

TEST(GeometryTest, Resize) {
  EXPECT_TRUE(resize<vec3>(vec4(1, 2, 3, 4)) == vec3(1, 2, 3));
  EXPECT_TRUE(resize<vec4>(vec3(1, 2, 3)) == vec4(1, 2, 3, 0));
}

TEST(GeometryTest, Scale) {
  vec4 v(1, 2, 3, 1);
  mat4 t = scale(2, 3, 4);
  vec4 v2 = t * v;
  EXPECT_EQ(v2(0), 2);
  EXPECT_EQ(v2(1), 6);
  EXPECT_EQ(v2(2), 12);
  EXPECT_EQ(v2(3), 1);
}

TEST(GeometryTest, Translate) {
  vec4 v(1, 2, 3, 1);
  mat4 t = translate(vec3(2, 3, 4));
  vec4 v2 = t * v;
  EXPECT_EQ(v2(0), 3);
  EXPECT_EQ(v2(1), 5);
  EXPECT_EQ(v2(2), 7);
  EXPECT_EQ(v2(3), 1);
}

TEST(GeometryTest, Yaw) {
  vec4 v(1, 2, 3, 1);
  EXPECT_NEAR((yaw(2 * M_PI) * v)(0), 1, 0.000001);
  EXPECT_NEAR((yaw(2 * M_PI) * v)(1), 2, 0.000001);
  EXPECT_NEAR((yaw(2 * M_PI) * v)(2), 3, 0.000001);
  EXPECT_NEAR((yaw(2 * M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((yaw(M_PI) * v)(0), -1, 0.000001);
  EXPECT_NEAR((yaw(M_PI) * v)(1), 2, 0.000001);
  EXPECT_NEAR((yaw(M_PI) * v)(2), -3, 0.000001);
  EXPECT_NEAR((yaw(M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((yaw(M_PI / 2) * v)(0), 3, 0.000001);
  EXPECT_NEAR((yaw(M_PI / 2) * v)(1), 2, 0.000001);
  EXPECT_NEAR((yaw(M_PI / 2) * v)(2), -1, 0.000001);
  EXPECT_NEAR((yaw(M_PI / 2) * v)(3), 1, 0.000001);
}

TEST(GeometryTest, Pitch) {
  vec4 v(1, 2, 3, 1);
  EXPECT_NEAR((pitch(2 * M_PI) * v)(0), 1, 0.000001);
  EXPECT_NEAR((pitch(2 * M_PI) * v)(1), 2, 0.000001);
  EXPECT_NEAR((pitch(2 * M_PI) * v)(2), 3, 0.000001);
  EXPECT_NEAR((pitch(2 * M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((pitch(M_PI) * v)(0), 1, 0.000001);
  EXPECT_NEAR((pitch(M_PI) * v)(1), -2, 0.000001);
  EXPECT_NEAR((pitch(M_PI) * v)(2), -3, 0.000001);
  EXPECT_NEAR((pitch(M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((pitch(M_PI / 2) * v)(0), 1, 0.000001);
  EXPECT_NEAR((pitch(M_PI / 2) * v)(1), -3, 0.000001);
  EXPECT_NEAR((pitch(M_PI / 2) * v)(2), 2, 0.000001);
  EXPECT_NEAR((pitch(M_PI / 2) * v)(3), 1, 0.000001);
}

TEST(GeometryTest, Roll) {
  vec4 v(1, 2, 3, 1);
  EXPECT_NEAR((roll(2 * M_PI) * v)(0), 1, 0.000001);
  EXPECT_NEAR((roll(2 * M_PI) * v)(1), 2, 0.000001);
  EXPECT_NEAR((roll(2 * M_PI) * v)(2), 3, 0.000001);
  EXPECT_NEAR((roll(2 * M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((roll(M_PI) * v)(0), -1, 0.000001);
  EXPECT_NEAR((roll(M_PI) * v)(1), -2, 0.000001);
  EXPECT_NEAR((roll(M_PI) * v)(2), 3, 0.000001);
  EXPECT_NEAR((roll(M_PI) * v)(3), 1, 0.000001);
  EXPECT_NEAR((roll(M_PI / 2) * v)(0), -2, 0.000001);
  EXPECT_NEAR((roll(M_PI / 2) * v)(1), 1, 0.000001);
  EXPECT_NEAR((roll(M_PI / 2) * v)(2), 3, 0.000001);
  EXPECT_NEAR((roll(M_PI / 2) * v)(3), 1, 0.000001);
}

template <typename M>
void ExpectMatEqual(M a, M b) {
  for (size_t row = 0; row < M::RowsAtCompileTime; ++row)
    for (size_t col = 0; col < M::ColsAtCompileTime; ++col)
      EXPECT_NEAR(a(row, col), b(row, col), 0.000001);
}

TEST(GeometryTest, Rotate) {
  ExpectMatEqual(rotate(vec3(1, 0, 0), 0.3), pitch(0.3));
  ExpectMatEqual(rotate(vec3(0, 1, 0), 0.3), yaw(0.3));
  ExpectMatEqual(rotate(vec3(0, 0, 1), 0.3), roll(0.3));
}

}  // namespace gl
