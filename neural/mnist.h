#pragma once

#include <vector>

#include "eigen/Eigen"

namespace neural {

using Matrix = Eigen::Matrix<float32, Eigen::Dynamic, Eigen::Dynamic>;

struct MNIST {
  MNIST();

  Matrix training_images;
  std::vector<uint8> training_labels;
  Matrix test_images;
  std::vector<uint8> test_labels;
};

}  // namespace neural
