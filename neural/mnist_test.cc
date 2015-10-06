#include "neural/mnist.h"

#include "gtest/gtest.h"

namespace neural {

TEST(MNISTTest, Parse) {
  MNIST mnist;

  EXPECT_EQ(mnist.training_labels.size(), 60000u);
  for (auto training_label : mnist.training_labels) {
    EXPECT_GE(training_label, 0);
    EXPECT_LE(training_label, 9);
  }

  EXPECT_EQ(mnist.test_labels.size(), 10000u);
  for (auto test_label : mnist.test_labels) {
    EXPECT_GE(test_label, 0);
    EXPECT_LE(test_label, 9);
  }

  EXPECT_EQ(mnist.training_images.rows(), 28 * 28);
  EXPECT_EQ(mnist.training_images.cols(), 60000);
  for (int64 row = 0; row < mnist.training_images.rows(); ++row)
    for (int64 col = 0; col < mnist.training_images.cols(); ++col) {
      EXPECT_GE(mnist.training_images(row, col), 0.0f);
      EXPECT_LE(mnist.training_images(row, col), 1.0f);
    }

  EXPECT_EQ(mnist.test_images.rows(), 28 * 28);
  EXPECT_EQ(mnist.test_images.cols(), 10000);
  for (int64 row = 0; row < mnist.test_images.rows(); ++row)
    for (int64 col = 0; col < mnist.test_images.cols(); ++col) {
      EXPECT_GE(mnist.test_images(row, col), 0.0f);
      EXPECT_LE(mnist.test_images(row, col), 1.0f);
    }
}

}  // namespace neural
