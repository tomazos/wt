#include "experimental/sdl/transforms.h"

#include "gtest/gtest.h"

namespace gl {

TEST(TransformsTest, Scale) {
  vec4 v(1, 2, 3, 1);
  mat4 t = scale(2, 3, 4);
  vec4 v2 = t * v;
  EXPECT_EQ(v2(0), 2);
  EXPECT_EQ(v2(1), 6);
  EXPECT_EQ(v2(2), 12);
  EXPECT_EQ(v2(3), 1);
}

TEST(TransformsTest, Translate) {
  vec4 v(1, 2, 3, 1);
  mat4 t = translate(vec3(2, 3, 4));
  vec4 v2 = t * v;
  EXPECT_EQ(v2(0), 3);
  EXPECT_EQ(v2(1), 5);
  EXPECT_EQ(v2(2), 7);
  EXPECT_EQ(v2(3), 1);
}

TEST(TransformsTest, Yaw) {
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

TEST(TransformsTest, Pitch) {
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

TEST(TransformsTest, Roll) {
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

TEST(TransformsTest, Rotate) {
  ExpectMatEqual(rotate(vec3(1, 0, 0), 0.3), pitch(0.3));
  ExpectMatEqual(rotate(vec3(0, 1, 0), 0.3), yaw(0.3));
  ExpectMatEqual(rotate(vec3(0, 0, 1), 0.3), roll(0.3));
}

}  // namespace gl
