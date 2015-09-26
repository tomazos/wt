#pragma once

#include <algorithm>

#include "core/must.h"
#include "eigen/Dense"

namespace neural {

inline float32 rectline(float32 x) { return std::max(0.0f, x); }
inline float64 rectline(float64 x) { return std::max(0.0, x); }
inline float32 sigmoid(float32 x) { return 1.0 / (1.0 + std::exp(-x)); }
inline float64 sigmoid(float64 x) { return 1.0 / (1.0 + std::exp(-x)); }

enum ActivationFunction { SIGMOID, RECTLINEAR, LINEAR, SOFTMAX };

struct LayerLayout {
  size_t input, output;
  ActivationFunction activation_function;
};

template <typename TFloat>
class FeedForward {
 public:
  using Float = TFloat;
  using Vector = Eigen::Matrix<Float, Eigen::Dynamic, 1>;
  using Matrix = Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic>;

  struct Layer {
    LayerLayout layout;
    Matrix weights;
  };

  std::vector<Layer> layers;

  class Activation;
  class BackPropogation;

  FeedForward(const std::vector<LayerLayout>& layouts) {
    MUST_GT(layouts.size(), 0u);
    for (LayerLayout layout : layouts) {
      if (layers.size() > 0) MUST_EQ(layers.back().layout.output, layout.input);

      layers.push_back(Layer{layout, Matrix(layout.input + 1, layout.output)});
    }
  }
};

template <typename Float>
class FeedForward<Float>::Activation {
 public:
  std::vector<Matrix> activities;

  Activation(const FeedForward& feed_forward, size_t batch_size)
      : batch_size_(batch_size), feed_forward_(feed_forward) {
    const std::vector<Layer>& layers = feed_forward.layers;
    for (const Layer& layer : layers) {
      const size_t activity_size = layer.layout.input + 1;
      activities.emplace_back(activity_size, batch_size);
      activities.back().bottomRows(1) = Matrix::Ones(1, batch_size);
    }
    const size_t activity_size = layers.back().layout.output;
    activities.emplace_back(activity_size, batch_size);
  }

  void operator()(const Matrix& input) {
    MUST_EQ(input.rows() + 1, activities.front().rows());
    MUST_EQ(input.cols(), int64(batch_size_));
    activities.front().topRows(input.rows()) = input;
    for (size_t i = 0; i < feed_forward_.layers.size(); ++i) {
      Activate(activities[i], feed_forward_.layers[i], activities[i + 1]);
    }
  }

 private:
  void Activate(const Matrix& previous, const Layer& layer, Matrix& next) {
    const size_t output = layer.layout.output;
    next.topRows(output) = layer.weights.transpose() * previous;
    switch (layer.layout.activation_function) {
      case LINEAR:
        break;
      case RECTLINEAR:
        next.topRows(output) = next.topRows(output).unaryExpr(
            [](Float x) -> Float { return rectline(x); });
        break;
      case SIGMOID:
        next.topRows(output) = next.topRows(output).unaryExpr(
            [](Float x) -> Float { return sigmoid(x); });
        break;
      case SOFTMAX: {
        Matrix exps = next.topRows(output)
                          .unaryExpr([](Float x) -> Float { return exp(x); });
        Matrix expsums = exps.colwise().sum();
        next.topRows(output) =
            exps.cwiseQuotient(expsums.replicate(exps.rows(), 1));
        break;
      }
      default:
        FAIL();
    }
  }

  const size_t batch_size_;
  const FeedForward& feed_forward_;
};

template <typename Float>
class FeedForward<Float>::BackPropogation {
 public:
  std::vector<Matrix> weight_derivatives;
  std::vector<Matrix> activity_derivatives;

  BackPropogation(const FeedForward& feed_forward, const Activation& activation,
                  size_t batch_size)
      : batch_size_(batch_size),
        feed_forward_(feed_forward),
        activation_(activation) {
    for (const Layer& layer : feed_forward_.layers) {
      weight_derivatives.emplace_back(layer.weights.rows(),
                                      layer.weights.cols());
    }
    for (const Matrix& activity : activation_.activities) {
      activity_derivatives.emplace_back(activity.rows(), activity.cols());
    }
  }

  void operator()(const Matrix& target) {
    MUST_EQ(target.rows(), feed_forward_.layers.back().layout.output);
    MUST_EQ(target.cols(), batch_size_);
  }

 private:
  const size_t batch_size_;
  const FeedForward& feed_forward_;
  const Activation& activation_;
};

}  // namespace neural
