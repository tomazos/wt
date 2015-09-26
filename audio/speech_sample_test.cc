#include "audio/speech_sample.h"

#include "gtest/gtest.h"

namespace audio {

TEST(SpeechSampleTest, Get) {
  Eigen::MatrixXf m = GetSpeechSamples<float32>(120, 100, 0);
  EXPECT_EQ(m.rows(), 120);
  EXPECT_EQ(m.cols(), 100);
  EXPECT_LE(m.maxCoeff(), 1);
  EXPECT_GE(m.maxCoeff(), -1);
}

}  // namespace audio
