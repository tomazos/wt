#include "neural/feed_forward.h"

#include "core/must.h"
#include "main/noargs.h"

namespace neural {
namespace {

using Float = float32;
using NeuralNet = FeedForward<Float>;
using Matrix = NeuralNet::Matrix;

const Float epsilon = 0.1;

void FeedForwardSimple() {
  NeuralNet neural_net({{2, 2, SOFTMAX}});
  neural_net.Randomize(epsilon);

  Matrix input(2, 4);
  input << 0.7, 0.1, 0.9, 0.3, 0.2, 1.0, 0.3, 0.8;

  Matrix target(2, 4);
  target << 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0;

  NeuralNet::Activation activation(neural_net, 4);

  activation(input);

  NeuralNet::BackPropogation back_propogation(neural_net, activation, 4);

  back_propogation(target);

  neural_net.Describe(std::cout);
  std::cout << "WEIGHTS: " << std::endl
            << neural_net.layers.at(0).weights << std::endl;
  std::cout << std::endl;
  activation.Describe(std::cout);
  std::cout << "INPUT: " << std::endl
            << activation.activities.at(0) << std::endl;
  std::cout << "PREOUTPUT: " << std::endl
            << activation.preoutputs.at(0) << std::endl;
  std::cout << "OUTPUT: " << std::endl
            << activation.activities.at(1) << std::endl;
  std::cout << "TARGET: " << std::endl
            << target << std::endl;
  std::cout << std::endl;
  back_propogation.Describe(std::cout);
  std::cout << "DWEIGHTS: " << std::endl
            << back_propogation.weight_derivatives.at(0) << std::endl;
  std::cout << "DINPUT: " << std::endl
            << back_propogation.activity_derivatives.at(0) << std::endl;
  std::cout << "DPREOUTPUT: " << std::endl
            << back_propogation.preoutput_derivatives.at(0) << std::endl;
  std::cout << "DOUTPUT: " << std::endl
            << back_propogation.activity_derivatives.at(1) << std::endl;
}

}  // namespace
}  // namespace neural

void Main() { neural::FeedForwardSimple(); }
