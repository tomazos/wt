#include "neural/mnist.h"

#include <boost/filesystem.hpp>

#include "core/env.h"
#include "core/file_functions.h"
#include "core/must.h"

namespace neural {

namespace {

template <typename T>
string ToData(T t) {
  const char* b = (const char*)&t;
  const char* e = b + sizeof(T);
  string s(b, e);
  std::reverse(s.begin(), s.end());
  return s;
}

void ParseImages(const filesystem::path& file, Matrix& images) {
  constexpr size_t width = 28;
  constexpr size_t ncells = width * width;
  const size_t nimages = images.cols();

  const string data = GetFileContents(file);
  MUST_EQ(data.substr(0, 4), ToData<uint32>(2051));
  MUST_EQ(data.substr(4, 4), ToData<uint32>(nimages));
  MUST_EQ(data.substr(8, 4), ToData<uint32>(width));
  MUST_EQ(data.substr(12, 4), ToData<uint32>(width));
  MUST_EQ(data.size(), 16u + ncells * nimages);
  for (size_t col = 0; col < nimages; ++col)
    for (size_t row = 0; row < ncells; ++row)
      images(row, col) = float(uint8(data[16u + col * ncells + row])) / 255.0f;
}

void ParseLabels(const filesystem::path& file, std::vector<uint8>& labels) {
  const size_t nlabels = labels.size();

  const string data = GetFileContents(file);
  MUST_EQ(data.substr(0, 4), ToData<uint32>(2049));
  MUST_EQ(data.substr(4, 4), ToData<uint32>(nlabels));
  MUST_EQ(data.size(), 8u + nlabels);
  for (size_t i = 0; i < nlabels; ++i) labels[i] = data[8u + i];
}

}  // namespace

MNIST::MNIST()
    : training_images(Matrix::Zero(28 * 28, 60000)),
      training_labels(60000),
      test_images(Matrix::Zero(28 * 28, 10000)),
      test_labels(10000) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path testdata_dir = source_root / "neural" / "testdata";
  filesystem::path training_images_file =
      testdata_dir / "train-images-idx3-ubyte";
  filesystem::path training_labels_file =
      testdata_dir / "train-labels-idx1-ubyte";
  filesystem::path test_images_file = testdata_dir / "t10k-images-idx3-ubyte";
  filesystem::path test_labels_file = testdata_dir / "t10k-labels-idx1-ubyte";
  ParseImages(training_images_file, training_images);
  ParseImages(test_images_file, test_images);
  ParseLabels(training_labels_file, training_labels);
  ParseLabels(test_labels_file, test_labels);

  std::vector<size_t> I(60000);
  for (size_t i = 0; i < 60000; ++i) I[i] = i;
  std::random_shuffle(I.begin(), I.end());
  Matrix old_training_images = training_images;
  std::vector<uint8> old_training_labels = training_labels;
  for (size_t i = 0; i < 60000; ++i) {
    training_images.col(i) = old_training_images.col(I[i]);
    training_labels.at(i) = old_training_labels.at(I[i]);
  }
}

}  // namespace neural
