#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "audio/audio_functions.h"
#include "audio/speech_sample.h"
#include "core/must.h"
#include "neural/feed_forward.h"
#include "neural/mnist.h"
#include "main/noargs.h"

typedef boost::shared_mutex Lock;
typedef boost::unique_lock<Lock> WriteLock;
typedef boost::shared_lock<Lock> ReadLock;

namespace neural {
namespace {

using Float = float32;

const Float threshold = 0.05;
const Float epsilon = 0.0001;
const Float learning_rate = 0.1;
const Float regularize = 0;  // .000025;
const size_t ntraining = 1000000;
const size_t nbatches = 1000;
const size_t nthreads = 20;
const size_t nbatch = ntraining / nbatches;
const size_t inputsize = 600;
const size_t outputsize = 60;
// std::vector<LayerLayout> layers = {{inputsize, 1, SIGMOID},
//                                   {1, outputsize, LINEAR}};

std::vector<LayerLayout> layers = {{inputsize, 800, SIGMOID},
                                   {800, outputsize, SIGMOID},
                                   {outputsize, inputsize, LINEAR}};

// std::vector<LayerLayout> layers = {{inputsize, outputsize, SOFTMAX}};

using NeuralNet = FeedForward<Float>;
using Matrix = NeuralNet::Matrix;

void FeedForwardAudio() {
  Matrix audio_samples =
      audio::GetSpeechSamples<Float>(inputsize, ntraining, threshold);

  Lock lock;
  NeuralNet neural_net(layers);
  neural_net.Randomize(epsilon);

  std::vector<Matrix> training_batches;

  for (size_t i = 0; i < nbatches; i++) {
    const size_t batch_begin = i * nbatch;

    training_batches.emplace_back(
        audio_samples.middleCols(batch_begin, nbatch).cast<Float>());
  }

  for (size_t epoch = 1; true; epoch++) {
    std::vector<std::thread> threads;
    Float total_cost = 0;
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
            ReadLock rlock(lock);
            const auto& batch = training_batches.at(batch_index);
            activation(batch);
            back_propogation(batch);
          }
          {
            WriteLock wlock(lock);
            back_propogation.Learn(learning_rate);
            total_cost += back_propogation.cost;
            neural_net.Regularize(regularize);
          }
        }
      }, thread_index);
    }
    for (auto& t : threads) t.join();

    {
      LOGEXPR(epoch);
      const Float avg_cost = total_cost / nbatches;
      LOGEXPR(avg_cost);
      NeuralNet::Activation test_activation(neural_net, nbatch);
      NeuralNet::BackPropogation test_back_propogation(neural_net,
                                                       test_activation, nbatch);
      test_activation(training_batches.at(epoch % nbatches));
      test_back_propogation(training_batches.at(epoch % nbatches));

      neural_net.Describe(std::cout);
      test_activation.Describe(std::cout);
      test_back_propogation.Describe(std::cout);
    }
  }
}

}  // namespace
}  // namespace neural

void Main() { neural::FeedForwardAudio(); }
