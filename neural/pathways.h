#pragma once

namespace neural {
namespace pathways {

class Pathway {
 public:
  virtual void Activate(const Matrix& input, Matrix& output) = 0;
  virtual void BackProp(const Matrix& output, const Matrix& output_derivative,
                        const Matrix& input, Matrix& output_derivative) = 0;

  virtual ~Pathway() = default;
};

class LinearCombine : public Pathway {
  void Activate(const Matrix& input, Matrix& output) {}

  void BackProp(const Matrix& output, const Matrix& output_derivative,
                const Matrix& input, Matrix& output_derivative) {}

 private:
  Matrix weights_;
};

}  // namespace pathways
}  // namespace neural
