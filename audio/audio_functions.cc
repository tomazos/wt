#include "audio/audio_functions.h"

#include "audio/wave_file.h"
#include "core/must.h"
#include "core/process.h"

namespace audio {

void PlaySound(const int16* begin_wave, const int16* end_wave) {
  WaveFile wave_file(begin_wave, end_wave);
  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();

  wave_file.SaveTo(tmpfile);
  ExecuteShellCommand("play '", tmpfile.string(), "' 2> /dev/null");
  filesystem::remove(tmpfile);
}

}  // namespace audio
