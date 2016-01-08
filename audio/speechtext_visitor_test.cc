#include "audio/speechtext_visitor.h"

#include <atomic>

#include "gtest/gtest.h"

namespace audio {

struct SpeechTextLengthTotaler : SpeechTextVisitor {
  std::atomic<int64> total_spoken_length, total_written_length;
  SpeechTextLengthTotaler() : total_spoken_length(0), total_written_length(0) {}

  bool operator()(int64 id, string_view written, const Wave& spoken) override {
    total_written_length += written.size();
    total_spoken_length += spoken.size();
    return true;
  }
};

TEST(SpeechTextVisitorTest, Basic) {
  database::sqlite::Initialize();
  SpeechTextLengthTotaler totaler;
  VisitSpeechText(totaler, 0, 10000, 20);
  EXPECT_EQ(totaler.total_written_length, 342244);
  EXPECT_EQ(totaler.total_spoken_length, 832980624 / 2);
}

}  // namespace audio
