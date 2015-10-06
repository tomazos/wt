#include <iostream>

#include "audio/audio_functions.h"
#include "audio/speechtext_visitor.h"
#include "core/regex.h"
#include "core/must.h"

namespace audio {

struct SpeechTextTransformer : SpeechTextVisitor {
  Regex good_written;
  size_t count = 0;

  SpeechTextTransformer()
      : good_written(R"(^(\<i\>)?[-A-Za-z0-9 \.\,\?\'\!\"\n]+(\<\/i\>)?$)") {}

  bool operator()(int64 id, string_view written, const Wave& spoken) override {
    if (!good_written.Matches(written)) return true;

    std::cout << id << ". " << written << std::endl;
    count++;

    std::cout << "[ORIGINAL]" << std::endl;
    PlaySound(spoken.data(), spoken.data() + spoken.size());

    Wave transformed;

    transformed = spoken;

    //    size_t N = transformed.size();

    //    for (size_t i = 0; i + 100 <= N; i += 100) {
    //      Eigen::VectorXd v = transformed.segment(i, 100).cast<float64>();
    //      v.unaryExpr([](double x) { return x / (1 << 15); });

    //      Eigen::VectorXd w = v;

    //      Eigen::VectorXcd w1 = w.cast<std::complex<float64>>();

    //      Eigen::VectorXcd w2 =
    //          w1

    //              w.unaryExpr([](double x) { return x * (1 << 15); });

    //      transformed.segment(i, 100) = w.cast<int16>();
    //    }

    std::cout << "[TRANSFORMED]" << std::endl;
    PlaySound(transformed.data(), transformed.data() + transformed.size());
    return true;
  }
};

}  // namespace audio

void Main() {
  audio::SpeechTextTransformer transformer;
  audio::VisitSpeechText(transformer, 1000, 1000, 1);
  LOGEXPR(transformer.count);
}
