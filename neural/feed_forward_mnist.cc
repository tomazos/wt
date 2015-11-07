#include "core/must.h"
#include "core/rwlock.h"
#include "neural/feed_forward.h"
#include "neural/mnist.h"
#include "main/noargs.h"

namespace neural {
namespace {

using Float = float32;

const Float epsilon = 0.1;
const Float learning_rate = 0.005;
const Float regularize = 0;
const size_t ntraining = 60000;
const size_t ntest = 10000;
const size_t nbatches = 600;
const size_t nthreads = 20;
const size_t nbatch = ntraining / nbatches;
const size_t inputsize = 28 * 28;
const size_t outputsize = 10;
// std::vector<LayerLayout> layers = {{inputsize, 1, SIGMOID},
//                                   {1, outputsize, LINEAR}};

std::vector<LayerLayout> layers = {{inputsize, 800, SIGMOID},
                                   {800, outputsize, SOFTMAX}};

// std::vector<LayerLayout> layers = {{inputsize, outputsize, SOFTMAX}};

using NeuralNet = FeedForward<Float>;
using Matrix = NeuralNet::Matrix;

Matrix LabelsToMatrix(const std::vector<uint8>& labels) {
  const size_t n = labels.size();
  Matrix result = Matrix::Zero(outputsize, n);
  for (size_t i = 0; i < n; ++i) result(labels.at(i), i) = 1;
  return result;
}

void FeedForwardMNIST() {
  MNIST mnist;
  mnist.training_images.array() -= 0.5;
  mnist.test_images.array() -= 0.5;

  ReadWriteMutex mutex;
  NeuralNet neural_net(layers);
  neural_net.Randomize(epsilon);

  std::vector<std::pair<Matrix, Matrix>> training_batches;

  for (size_t i = 0; i < nbatches; i++) {
    const auto labels_begin = mnist.training_labels.cbegin();
    const size_t batch_begin = i * nbatch;
    const size_t batch_end = batch_begin + nbatch;

    Matrix images =
        mnist.training_images.middleCols(batch_begin, nbatch).cast<Float>();
    Matrix labels =
        LabelsToMatrix({labels_begin + batch_begin, labels_begin + batch_end});

    training_batches.emplace_back(std::move(images), std::move(labels));
  }

  std::pair<Matrix, Matrix> test;
  test.first = mnist.test_images.cast<Float>();
  test.second = LabelsToMatrix(mnist.test_labels);

  for (size_t epoch = 1; true; epoch++) {
    std::vector<std::thread> threads;
    for (size_t thread_index = 0; thread_index < nthreads; ++thread_index) {
      threads.emplace_back([&](size_t thread_index) {
        const size_t batch_start = (nbatches / nthreads) * (thread_index + 0);
        const size_t batch_end = (nbatches / nthreads) * (thread_index + 1);
        for (size_t batch_index = batch_start; batch_index < batch_end;
             ++batch_index) {
          NeuralNet::Activation activation(neural_net, nbatch);
          NeuralNet::BackPropogation back_propogation(neural_net, activation,
                                                      nbatch);
          {
            ReaderLockGuard rlock(mutex);

            const auto& batch = training_batches.at(batch_index);
            activation(batch.first);
            back_propogation(batch.second);
          }
          {
            WriterLockGuard wlock(mutex);
            back_propogation.Learn(learning_rate);
            neural_net.Regularize(regularize);
          }
        }
      }, thread_index);
    }
    for (auto& t : threads) t.join();

    {
      LOGEXPR(epoch);
      NeuralNet::Activation test_activation(neural_net, ntest);
      NeuralNet::BackPropogation test_back_propogation(neural_net,
                                                       test_activation, ntest);
      test_activation(test.first);
      test_back_propogation(test.second);
      size_t ncorrect = 0;
      const Matrix& output = test_activation.activities.back();
      for (size_t i = 0; i < ntest; ++i) {
        uint8 guess = 0;
        for (size_t j = 1; j < outputsize; ++j)
          if (output(j, i) > output(guess, i)) guess = j;
        if (guess == mnist.test_labels.at(i)) ncorrect++;
      }
      LOGEXPR(ncorrect);
      neural_net.Describe(std::cout);
      test_activation.Describe(std::cout);
      test_back_propogation.Describe(std::cout);
    }
  }
}

}  // namespace
}  // namespace neural

void Main() { neural::FeedForwardMNIST(); }
