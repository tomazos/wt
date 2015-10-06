#pragma once

#include "eigen/Eigen"
#include "audio/speechtext_visitor.h"

namespace audio {

template <typename Float>
Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic> GetSpeechSamples(
    size_t width, size_t nsamples, Float threshold);

template <typename TFloat>
class SpeechSampler : public SpeechTextVisitor {
 public:
  using Float = TFloat;
  using Vector = Eigen::Matrix<Float, Eigen::Dynamic, 1>;
  using Matrix = Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic>;

  SpeechSampler(Matrix& results, size_t width, size_t nsamples, Float threshold)
      : results_(results),
        width_(width),
        nsamples_(nsamples),
        threshold_(threshold) {
    results_.resize(width, nsamples);
  }

  ~SpeechSampler() { results_.conservativeResize(width_, next_sample); }

 private:
  Matrix& results_;
  const size_t width_;
  const size_t nsamples_;
  const Float threshold_;

  Mutex mu_;
  size_t next_sample = 0;

  bool operator()(int64 id, string_view written, const Wave& spoken) override {
    for (size_t i = 0u; i + width_ <= size_t(spoken.size()); i += width_) {
      Vector v = spoken.middleRows(i, width_).cast<Float>() / pow(2, 15);
      Float f = v.cwiseAbs().maxCoeff();
      if (v.maxCoeff() < threshold_) continue;
      LockGuard l(mu_);
      if (next_sample >= nsamples_) return false;
      results_.col(next_sample++) = v;
    }
    return true;
  }
};

template <typename Float>
Eigen::Matrix<Float, Eigen::Dynamic, Eigen::Dynamic> GetSpeechSamples(
    size_t width, size_t nsamples, float threshold) {
  Eigen::MatrixXf results;
  {
    SpeechSampler<float> sampler(results, width, nsamples, threshold);
    VisitSpeechText(sampler, 0, 1900000, 20);
  }
  Eigen::MatrixXf old_results = results;
  std::vector<size_t> I(nsamples);

  for (size_t i = 0; i < nsamples; ++i) I[i] = i;

  std::random_shuffle(I.begin(), I.end());
  for (size_t i = 0; i < nsamples; ++i) results.col(i) = old_results.col(I[i]);

  return results;
}

}  // namespace audio
