#include "audio/wave_file.h"

#include <boost/filesystem.hpp>

#include "core/env.h"
#include "gtest/gtest.h"

TEST(WaveFileTest, Loading) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path short_wav_path = source_root / "audio/testdata/short.wav";
  audio::WaveFile wave_file(short_wav_path);

  size_t num_samples = 0;
  int64 sample_total = 0;
  for (int16 sample : wave_file) {
    EXPECT_EQ(sample, wave_file[num_samples++]);
    sample_total += sample;
  }
  EXPECT_EQ(wave_file.size(), num_samples);
  EXPECT_EQ(sample_total, -284307);
}

TEST(WaveFileTest, Saving) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path short_wav_path = source_root / "audio/testdata/short.wav";
  audio::WaveFile wave_file_original(short_wav_path);

  const int16* a = wave_file_original.begin();
  const int16* d = wave_file_original.end();
  const size_t third = (d - a) / 3;
  const int16* b = a + 1 * third;
  const int16* c = a + 2 * third;
  audio::WaveFile wave_file_middle_third(b, c);
  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();
  wave_file_middle_third.SaveTo(tmpfile);

  audio::WaveFile wave_file_loaded(tmpfile);

  const int16* b2 = wave_file_loaded.begin();
  const int16* c2 = wave_file_loaded.end();

  MUST_EQ(c - b, c2 - b2);
  for (size_t i = 0; i < third; i++) MUST_EQ(b[i], b2[i]);

  filesystem::remove(tmpfile);
}
