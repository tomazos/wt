#include "neural/feed_forward.h"

#include "gtest/gtest.h"

namespace neural {

TEST(FeedForwardTest, Sigmoid) {
  EXPECT_FLOAT_EQ(sigmoid(0.0), 0.5);
  EXPECT_FLOAT_EQ(sigmoid(1.0f), 0.7310586);
  EXPECT_FLOAT_EQ(sigmoid(-1.0), 0.26894143);
}

TEST(FeedForwardTest, Construct) {
  FeedForward<float32> ff({{100, 50, LINEAR}, {50, 200, RECTLINEAR}});
  try {
    FeedForward<float32> badff({{100, 50, LINEAR}, {51, 200, RECTLINEAR}});
    GFAIL();
  } catch (const std::exception& e) {
  }
}

TEST(FeedForwardTest, ActivateLinear) {
  using Matrix = FeedForward<float64>::Matrix;
  using Activation = FeedForward<float64>::Activation;
  using BackPropogation = FeedForward<float64>::BackPropogation;

  FeedForward<float64> ff({{3, 2, LINEAR}});
  EXPECT_EQ(ff.layers[0].weights.rows(), 4);
  EXPECT_EQ(ff.layers[0].weights.cols(), 2);
  Activation a(ff, 2);
  BackPropogation bp(ff, a, 2);

  Matrix m(3, 2);
  m.setZero();
  a(m);
  EXPECT_EQ(a.activities[0].topRows(3), m);

  ff.layers[0].weights << 1, 2, 3, 4, 5, 6, 7, 8;

  m << 9, 10, 11, 12, 13, 14;
  //     1   1
  a(m);

  Matrix b = a.activities[1].topRows(2);
  Matrix o(2, 2);

  o << (9 * 1 + 11 * 3 + 13 * 5 + 1 * 7), (10 * 1 + 12 * 3 + 14 * 5 + 1 * 7),
      (9 * 2 + 11 * 4 + 13 * 6 + 1 * 8), (10 * 2 + 12 * 4 + 14 * 6 + 1 * 8);

  EXPECT_EQ(b.rows(), o.rows());
  EXPECT_EQ(b.cols(), o.cols());

  EXPECT_EQ(b(0, 0), o(0, 0));
  EXPECT_EQ(b(0, 1), o(0, 1));
  EXPECT_EQ(b(1, 0), o(1, 0));
  EXPECT_EQ(b(1, 1), o(1, 1));

  Matrix t(2, 2);
  t << -10, 20, 30, -40;
  bp(t);

  EXPECT_EQ(bp.weight_derivatives.size(), 1u);
  EXPECT_EQ(bp.activity_derivatives.size(), 2u);
  EXPECT_EQ(bp.preoutput_derivatives.size(), 1u);
}

TEST(FeedForwardTest, ActivateRectlinear) {
  using Matrix = FeedForward<float64>::Matrix;
  using Activation = FeedForward<float64>::Activation;
  //  using BackPropogation = FeedForward<float64>::BackPropogation;

  FeedForward<float64> ff({{3, 2, RECTLINEAR}});
  EXPECT_EQ(ff.layers[0].weights.rows(), 4);
  EXPECT_EQ(ff.layers[0].weights.cols(), 2);
  Activation a(ff, 2);
  Matrix m(3, 2);
  m.setZero();
  a(m);
  EXPECT_EQ(a.activities[0].topRows(3), m);

  ff.layers[0].weights << 1, -2, -3, 4, 5, -6, -7, -8;

  m << 9, 10, 11, 12, 13, 14;
  //     1   1
  a(m);

  Matrix b = a.activities[1].topRows(2);
  Matrix o(2, 2);

  o << (9 * 1 + -11 * 3 + 13 * 5 - 1 * 7), (10 * 1 - 12 * 3 + 14 * 5 - 1 * 7),
      0, 0;

  EXPECT_EQ(b.rows(), o.rows());
  EXPECT_EQ(b.cols(), o.cols());

  EXPECT_EQ(b(0, 0), o(0, 0));
  EXPECT_EQ(b(0, 1), o(0, 1));
  EXPECT_EQ(b(1, 0), o(1, 0));
  EXPECT_EQ(b(1, 1), o(1, 1));
}

TEST(FeedForwardTest, ActivateSigmoid) {
  using Matrix = FeedForward<float64>::Matrix;
  using Activation = FeedForward<float64>::Activation;
  //  using BackPropogation = FeedForward<float64>::BackPropogation;

  FeedForward<float64> ff({{3, 2, SIGMOID}});
  EXPECT_EQ(ff.layers[0].weights.rows(), 4);
  EXPECT_EQ(ff.layers[0].weights.cols(), 2);
  Activation a(ff, 2);
  Matrix m(3, 2);
  m.setZero();
  a(m);
  EXPECT_EQ(a.activities[0].topRows(3), m);

  ff.layers[0].weights << 1, -2, -3, 4, 5, -6, -7, -8;

  m << 9, 10, 11, 12, 13, 14;
  //     1   1
  a(m);

  Matrix b = a.activities[1].topRows(2);

  EXPECT_EQ(b.rows(), 2);
  EXPECT_EQ(b.cols(), 2);

  for (size_t i = 0; i < 2; ++i)
    for (size_t j = 0; j < 2; ++j) {
      EXPECT_LE(b(i, j), 1);
      EXPECT_GE(b(i, j), -1);
    }
}

TEST(FeedForwardTest, Softmax) {
  using Matrix = FeedForward<float64>::Matrix;
  using Activation = FeedForward<float64>::Activation;
  //  using BackPropogation = FeedForward<float64>::BackPropogation;

  FeedForward<float64> ff({{3, 2, SOFTMAX}});
  EXPECT_EQ(ff.layers[0].weights.rows(), 4);
  EXPECT_EQ(ff.layers[0].weights.cols(), 2);
  Activation a(ff, 2);
  Matrix m(3, 2);
  m.setZero();
  a(m);
  EXPECT_EQ(a.activities[0].topRows(3), m);

  ff.layers[0].weights << 1, -2, -3, 4, 5, -6, -7, -8;

  m << 9, 10, 11, 12, 13, 14;
  //     1   1
  a(m);

  Matrix b = a.activities[1].topRows(2);

  EXPECT_EQ(b.rows(), 2);
  EXPECT_EQ(b.cols(), 2);

  EXPECT_FLOAT_EQ(b(0, 0) + b(1, 0), 1);
  EXPECT_FLOAT_EQ(b(0, 1) + b(1, 1), 1);
}

TEST(FeedForwardTest, BackPropogateLinear) {
  using Matrix = FeedForward<float64>::Matrix;
  using Activation = FeedForward<float64>::Activation;
  //  using BackPropogation = FeedForward<float64>::BackPropogation;

  FeedForward<float64> ff({{3, 2, LINEAR}});
  EXPECT_EQ(ff.layers[0].weights.rows(), 4);
  EXPECT_EQ(ff.layers[0].weights.cols(), 2);
  Activation a(ff, 2);
  Matrix m(3, 2);
  m.setZero();
  a(m);
  EXPECT_EQ(a.activities[0].topRows(3), m);

  ff.layers[0].weights << 1, 2, 3, 4, 5, 6, 7, 8;

  m << 9, 10, 11, 12, 13, 14;
  //     1   1
  a(m);

  Matrix b = a.activities[1].topRows(2);
  Matrix o(2, 2);

  o << (9 * 1 + 11 * 3 + 13 * 5 + 1 * 7), (10 * 1 + 12 * 3 + 14 * 5 + 1 * 7),
      (9 * 2 + 11 * 4 + 13 * 6 + 1 * 8), (10 * 2 + 12 * 4 + 14 * 6 + 1 * 8);

  EXPECT_EQ(b.rows(), o.rows());
  EXPECT_EQ(b.cols(), o.cols());

  EXPECT_EQ(b(0, 0), o(0, 0));
  EXPECT_EQ(b(0, 1), o(0, 1));
  EXPECT_EQ(b(1, 0), o(1, 0));
  EXPECT_EQ(b(1, 1), o(1, 1));
}

}  // namespace neural
