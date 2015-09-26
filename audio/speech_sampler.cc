#include "audio/speech_sample.h"

#include "audio/audio_functions.h"
#include "core/must.h"
#include "main/args.h"

void Main(const std::vector<string>& args) {
  MUST_EQ(args.size(), 3u);
  size_t width = std::stoul(args[0]);
  size_t nsamples = std::stoul(args[1]);
  float32 threshold = std::stof(args[2]);
  Eigen::MatrixXf m =
      audio::GetSpeechSamples<float32>(width, nsamples, threshold);
  Eigen::MatrixXf m2;
  m2.resize(width, nsamples);
  std::vector<size_t> v(nsamples);
  for (size_t i = 0; i < v.size(); ++i) v[i] = i;
  std::random_shuffle(v.begin(), v.end());
  for (size_t i = 0; i < v.size(); ++i) m2.col(i) = m.col(v[i]);
  Eigen::Matrix<int16, Eigen::Dynamic, Eigen::Dynamic> i16 =
      (m2 * std::pow(2, 15)).cast<int16>();
  audio::PlaySound(i16.data(), i16.data() + (i16.rows() * i16.cols()));
}
