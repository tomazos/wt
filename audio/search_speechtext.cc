#include "main/args.h"

#include "audio/audio_functions.h"
#include "audio/srtproto_file.pb.h"
#include "audio/wave_file.h"
#include "core/must.h"
#include "core/sequence_file.h"

void Main(const std::vector<string>& args) {
  if (args.size() < 2) FAIL("usage: search_speechtext <query> <speechtext>");
  string query = args[0];
  filesystem::path speechtext_path = args[1];
  SequenceReader reader(speechtext_path);

  size_t count = 1;
  while (true) {
    audio::SpeechText speechtext;
    if (!reader.ReadMessage(speechtext)) break;

    const string& text = speechtext.text();
    const string& wave_bytes = speechtext.wave();

    MUST_EQ(wave_bytes.size() % 2, 0u);
    const int16* begin_wave = (const int16*)wave_bytes.data();
    const int16* end_wave = begin_wave + wave_bytes.size() / 2;

    if (text.find(query) != string::npos) {
      std::cout << count++ << ". " << text << std::endl;
      audio::PlaySound(begin_wave, end_wave);
    }
  }
}
