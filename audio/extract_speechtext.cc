#include "main/args.h"

#include "audio/srtproto_file.pb.h"
#include "audio/wave_file.h"
#include "core/must.h"
#include "core/sequence_file.h"

void Main(const std::vector<string>& args) {
  if (args.size() != 3)
    FAIL("usage: extract_speechtext <wav> <srtproto> <speechtext>");
  filesystem::path wav_path = args[0];
  filesystem::path srtproto_path = args[1];
  filesystem::path speechtext_path = args[2];
  audio::WaveFile wavefile(wav_path);
  audio::SrtProtoFile srtproto;
  MUST(srtproto.ParseFromString(GetFileContents(srtproto_path)));
  int64 skips = 0;
  SequenceWriter writer(speechtext_path, OVERWRITE);

  for (const audio::Subtitle& subtitle : srtproto.subtitles()) {
    int64 begin_sample = subtitle.begin_ms() * audio::kSampleRate / 1000;
    begin_sample -= 6000;
    int64 end_sample = subtitle.end_ms() * audio::kSampleRate / 1000;
    end_sample += 6000;
    const string& text = subtitle.text();

    if (begin_sample >= end_sample || begin_sample < 0 ||
        end_sample > int64(wavefile.size())) {
      skips++;
      continue;
    }
    const int16* begin_wave = wavefile.begin() + begin_sample;
    const int16* end_wave = wavefile.begin() + end_sample;
    const char* wave_data = (const char*)begin_wave;
    size_t wave_len = (end_wave - begin_wave) * 2;

    audio::SpeechText speechtext;
    speechtext.set_wave(wave_data, wave_len);
    speechtext.set_text(text);
    writer.WriteMessage(speechtext);
  }
}
