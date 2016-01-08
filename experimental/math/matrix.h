#pragma once

namespace math {

template <typename T>
class matrix {
 public:
  using value_type = T;

  size_t size() const { return data_.size(); }
  size_t rows() const { return rows_; }
  size_t cols() const { return cols_; }

 private:
  matrix(size_t rows, size_t cols)
      : rows_(rows), cols_(cols), data_(rows * cols) {}

  size_t rows_;
  size_t cols_;
  std::valarray<T> data_;
};

}  // namespace math
