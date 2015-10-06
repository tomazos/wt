#pragma once

#include <algorithm>

#include "core/must.h"
#include "core/random.h"
#include "eigen/Dense"

namespace neural {

inline float32 rectline(float32 x) { return std::max(0.0f, x); }
inline float64 rectline(float64 x) { return std::max(0.0, x); }
inline float32 sigmoid(float32 x) { return 1.0 / (1.0 + std::exp(-x)); }
inline float64 sigmoid(float64 x) { return 1.0 / (1.0 + std::exp(-x)); }

template <typename M>
void DescribeMatrix(std::ostream& os, const M& m) {
  os << "[" << m.minCoeff() << ", " << (m.sum() / (m.rows() * m.cols())) << ", "
     << m.maxCoeff() << "]";
}

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
  using Array = Eigen::Array<Float, Eigen::Dynamic, Eigen::Dynamic>;

  struct Layer {
    LayerLayout layout;
    Matrix weights;
  };

  std::vector<Layer> layers;
  Mutex layers_mu_;

  class Activation;
  class BackPropogation;
  class WeightDerivatives;

  FeedForward(const std::vector<LayerLayout>& layouts) {
    MUST_GT(layouts.size(), 0u);
    for (LayerLayout layout : layouts) {
      if (layers.size() > 0) MUST_EQ(layers.back().layout.output, layout.input);

      layers.push_back(
          Layer{layout, Matrix::Zero(layout.input + 1, layout.output)});
    }
    MUST(IsFinite());
  }

  void Randomize(Float upper) {
    auto R = [upper](Float) -> Float {
      const Float x = RandFloat() * 2 - 1;
      return x * upper;
    };

    for (Layer& layer : layers) {
      layer.weights = layer.weights.unaryExpr(R).eval();
    }
  }

  void Regularize(Float regularization) {
    for (Layer& layer : layers) {
      layer.weights.array() *= 1 - regularization;
    }
  }

  bool IsFinite() {
    for (const Layer& layer : layers)
      if (!layer.weights.allFinite()) return false;
    return true;
  }

  void Describe(std::ostream& os) const {
    for (size_t i = 0; i < layers.size(); ++i) {
      os << "WEIGHTS " << i << " ";
      DescribeMatrix(os, layers[i].weights);
      os << std::endl;
    }
  }
};

template <typename V>
void DescribeVectorMatrix(std::ostream& os, string_view name, const V& v) {
  for (size_t i = 0; i < v.size(); ++i) {
    os << name << " " << i << " ";
    DescribeMatrix(os, v[i]);
    os << std::endl;
  }
}

template <typename Float>
class FeedForward<Float>::Activation {
 public:
  std::vector<Matrix> activities;
  std::vector<Matrix> preoutputs;

  bool IsFinite() {
    for (const Matrix& m : activities)
      if (!m.allFinite()) return false;
    for (const Matrix& m : preoutputs)
      if (!m.allFinite()) return false;
    return true;
  }

  void Describe(std::ostream& os) {
    DescribeVectorMatrix(os, "ACTIVITY", activities);
    DescribeVectorMatrix(os, "PREOUTPUT", preoutputs);
  }

  Activation(const FeedForward& feed_forward, size_t batch_size)
      : batch_size_(batch_size), feed_forward_(feed_forward) {
    const std::vector<Layer>& layers = feed_forward.layers;
    for (const Layer& layer : layers) {
      const size_t activity_size = layer.layout.input + 1 /*bias*/;
      activities.push_back(Matrix::Zero(activity_size, batch_size));
      activities.back().bottomRows(1) = Matrix::Ones(1, batch_size);

      const size_t preoutput_size = layer.layout.output;
      preoutputs.push_back(Matrix::Zero(preoutput_size, batch_size));
    }
    const size_t activity_size = layers.back().layout.output;
    activities.push_back(Matrix::Zero(activity_size, batch_size));

    MUST(IsFinite());
  }

  void operator()(const Matrix& input) {
    MUST_EQ(input.rows() + 1, activities.front().rows());
    MUST_EQ(input.cols(), int64(batch_size_));
    activities.front().topRows(input.rows()) = input;
    for (size_t i = 0; i < feed_forward_.layers.size(); ++i) {
      Activate(activities.at(i), feed_forward_.layers.at(i), preoutputs.at(i),
               activities.at(i + 1));
    }
  }

 private:
  void Activate(const Matrix& previous, const Layer& layer, Matrix& preoutput,
                Matrix& next) {
    const size_t output = layer.layout.output;
    preoutput = layer.weights.transpose() * previous;
    switch (layer.layout.activation_function) {
      case LINEAR:
        next.topRows(output) = preoutput;
        break;
      case RECTLINEAR:
        next.topRows(output) =
            preoutput.unaryExpr([](Float x) -> Float { return rectline(x); });
        break;
      case SIGMOID:
        next.topRows(output) =
            preoutput.unaryExpr([](Float x) -> Float { return sigmoid(x); });
        break;
      case SOFTMAX: {
        Float avg = preoutput.sum() / (preoutput.rows() * preoutput.cols());

        Matrix exps =
            preoutput.unaryExpr([=](Float x) -> Float { return exp(x - avg); });
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
  Float cost = 0;
  std::vector<Matrix> weight_derivatives;
  std::vector<Matrix> activity_derivatives;
  std::vector<Matrix> preoutput_derivatives;

  bool IsFinite() {
    for (const Matrix& m : weight_derivatives)
      if (!m.allFinite()) return false;
    for (const Matrix& m : activity_derivatives)
      if (!m.allFinite()) return false;
    for (const Matrix& m : preoutput_derivatives)
      if (!m.allFinite()) return false;
    return true;
  }

  void Describe(std::ostream& os) {
    os << "COST " << cost << std::endl;
    DescribeVectorMatrix(os, "DWEIGHTS", weight_derivatives);
    DescribeVectorMatrix(os, "DACTIVITY", activity_derivatives);
    DescribeVectorMatrix(os, "DPREOUTPUT", preoutput_derivatives);
  }

  BackPropogation(FeedForward& feed_forward, const Activation& activation,
                  size_t batch_size)
      : batch_size_(batch_size),
        feed_forward_(feed_forward),
        activation_(activation) {
    for (const Layer& layer : feed_forward_.layers)
      weight_derivatives.push_back(
          Matrix::Zero(layer.weights.rows(), layer.weights.cols()));
    for (const Matrix& activity : activation_.activities)
      activity_derivatives.push_back(
          Matrix::Zero(activity.rows(), activity.cols()));
    for (const Matrix& preoutput : activation_.preoutputs)
      preoutput_derivatives.push_back(
          Matrix::Zero(preoutput.rows(), preoutput.cols()));
    MUST(IsFinite());
  }

  void operator()(const Matrix& target) {
    // cross-entropy:
    //    cost = -(target.cwiseProduct(activation_.activities.back().unaryExpr(
    //                 [](Float x) -> Float { return log(x); }))).sum() /
    //           target.cols();

    // mean squared error:
    cost = (target - activation_.activities.back())
               .unaryExpr([](Float x) -> Float { return pow(x, 2); })
               .sum() /
           (target.cols() * target.rows());

    MUST_EQ(target.rows(), activation_.activities.back().rows());
    MUST_EQ(target.cols(), activation_.activities.back().cols());

    // mean squared error:
    activity_derivatives.back() =
        (activation_.activities.back() - target) / (target.cols());

    // cross-entropy:
    //    activity_derivatives.back() =
    //        -target.cwiseQuotient(activation_.activities.back()) /
    //        target.cols();

    const size_t N = feed_forward_.layers.size();
    for (size_t i = 0; i < N; i++) {
      const size_t L = N - i - 1;
      const Layer& layer = feed_forward_.layers.at(L);
      const Matrix& weights = layer.weights;
      Matrix& weight_derivative = weight_derivatives.at(L);
      const Matrix& input = activation_.activities.at(L);
      Matrix& input_derivative = activity_derivatives.at(L);
      const Matrix& preoutput = activation_.preoutputs.at(L);
      Matrix& preoutput_derivative = preoutput_derivatives.at(L);
      const Matrix& output = activation_.activities.at(L + 1);
      const Matrix& output_derivative = activity_derivatives.at(L + 1);
      BackPropogatePreoutput(layer, preoutput, preoutput_derivative, output,
                             output_derivative);

      BackPropogateActivity(layer, weights, weight_derivative, input,
                            input_derivative, preoutput, preoutput_derivative);
    }
  }

  void Learn(Float learning_rate) {
    for (size_t i = 0; i < weight_derivatives.size(); ++i) {
      LearnMatrix(feed_forward_.layers.at(i).weights, weight_derivatives.at(i),
                  learning_rate);
    }
  }

 private:
  void LearnMatrix(Matrix& output, const Matrix& derivatives,
                   Float learning_rate) {
    output -= learning_rate * derivatives;
  }

  void BackPropogatePreoutput(const Layer& layer, const Matrix& preoutput,
                              Matrix& preoutput_derivative,
                              const Matrix& output_and_bias,
                              const Matrix& output_derivative_and_bias) {
    const Matrix output = output_and_bias.topRows(layer.layout.output);
    const Matrix output_derivative =
        output_derivative_and_bias.topRows(layer.layout.output);
    switch (layer.layout.activation_function) {
      case LINEAR:
        preoutput_derivative = output_derivative;
        break;
      case RECTLINEAR: {
        const auto& positives =
            (output.array() > Float(0)).template cast<Float>();
        preoutput_derivative = (positives * output_derivative.array()).matrix();
        break;
      }
      case SIGMOID:
        preoutput_derivative = (output_derivative.array() * output.array() *
                                (Float(1) - output.array())).matrix();
        break;
      case SOFTMAX: {
        for (size_t i = 0; i < layer.layout.output; ++i) {
          Array x = Array::Zero(1, batch_size_);
          for (size_t j = 0; j < layer.layout.output; ++j) {
            Array yi = output.row(i).array();
            Array yj = output.row(j).array();
            Array dC_dyj = output_derivative.row(j).array();

            // add dC_dyj * dyj/dxi
            if (i == j)
              x += dC_dyj * (yi - yi * yj);
            else
              x -= dC_dyj * yi * yj;
          }
          preoutput_derivative.row(i).array() = x;
        }
        break;
      }
      default:
        FAIL();
    }
  }

  void BackPropogateActivity(const Layer& layer, const Matrix& weights,
                             Matrix& weight_derivative, const Matrix& input,
                             Matrix& input_derivative, const Matrix& preoutput,
                             const Matrix& preoutput_derivative) {
    input_derivative = weights * preoutput_derivative;
    weight_derivative = (input * preoutput_derivative.transpose());
  }

  const size_t batch_size_;
  FeedForward& feed_forward_;
  const Activation& activation_;
};

}  // namespace neural
