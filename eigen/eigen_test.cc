#include <iostream>

#include "eigen/Eigen"

using namespace Eigen;

template <typename Scalar>
using MatrixX = Matrix<Scalar, Dynamic, Dynamic>;

int main() {
  const MatrixX<int> a = MatrixX<int>::Random(4096, 4096);
  const MatrixX<int> b = MatrixX<int>::Random(4096, 4096);
  const MatrixX<int> c = a * b;
  return int(c.sum());
}
