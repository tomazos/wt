#include "audio/wave_file.h"

#include "core/file_functions.h"

namespace audio {

WaveFile::WaveFile(filesystem::path source) : data_(GetFileContents(source)) {
  SetupFormat();
}

WaveFile::WaveFile(const int16* begin_sample, const int16* end_sample) {
  static constexpr uint8 header[] = {
      0x52, 0x49, 0x46, 0x46,  // RIFF
      0xDE, 0xAD, 0xBE, 0xEF,  // riff size
      0x57, 0x41, 0x56, 0x45,  // WAVE
      0x66, 0x6d, 0x74, 0x20,  // fmt
      0x10, 0x00, 0x00, 0x00,  // fmt size
      0x01, 0x00,              // audio format
      0x01, 0x00,              // nchannels
      0xe0, 0x2e, 0x00, 0x00,  // sample rate
      0xc0, 0x5d, 0x00, 0x00,  // byte rate
      0x02, 0x00,              // block align
      0x10, 0x00,              // bits per sample
      0x64, 0x61, 0x74, 0x61,  // data
      0xDE, 0xAD, 0xBE, 0xEF,  // size
  };
  constexpr size_t riff_size_pos = 4;
  constexpr size_t data_size_pos = 40;
  uint32 data_size = uint32((end_sample - begin_sample) * 2);
  uint32 riff_size = uint32(data_size + sizeof(header) - 8);
  data_ = string(sizeof(header) + data_size, '\0');
  uint8* buf = (uint8*)&data_[0];
  memcpy(buf, header, sizeof(header));
  memcpy(buf + sizeof(header), begin_sample, data_size);
  memcpy(buf + riff_size_pos, &riff_size, 4);
  memcpy(buf + data_size_pos, &data_size, 4);
  begin_sample_ = (int16*)(buf + sizeof(header));
  end_sample_ = begin_sample_ + (end_sample - begin_sample);
}

void WaveFile::SaveTo(filesystem::path p) { SetFileContents(p, data_); }

void WaveFile::SetupFormat() {
  MUST_EQ(data_.substr(0, 4), "RIFF");
  uint32 length = Read<uint32>(4);
  MUST_EQ(length + 8, data_.size());
  MUST_EQ(data_.substr(8, 4), "WAVE");
  size_t subchunk = 12;
  while (subchunk < data_.size()) {
    string_view tag = string_view(data_).substr(subchunk, 4);
    size_t length = Read<uint32>(subchunk + 4);
    DUMPEXPR(tag);
    DUMPEXPR(subchunk);
    DUMPEXPR(length);
    if (tag == "fmt ") {
      const size_t fmt = subchunk + 8;
      const uint16 audio_format = Read<uint16>(fmt);
      const uint16 num_channels = Read<uint16>(fmt + 2);
      const uint32 sample_rate = Read<uint32>(fmt + 4);
      const uint32 byte_rate = Read<uint32>(fmt + 8);
      const uint16 block_align = Read<uint16>(fmt + 12);
      const uint16 bits_per_sample = Read<uint16>(fmt + 14);
      MUST_EQ(audio_format, 1 /*PCM*/);
      MUST_EQ(num_channels, 1 /*mono*/);
      MUST_EQ(sample_rate, kSampleRate);
      MUST_EQ(byte_rate, sample_rate * num_channels * bits_per_sample / 8);
      MUST_EQ(block_align, num_channels * bits_per_sample / 8);
      MUST_EQ(bits_per_sample, 16);
    } else if (tag == "data") {
      const size_t start_data_offset = subchunk + 8;
      MUST(begin_sample_ == nullptr);
      begin_sample_ = (const int16*)(data_.data() + start_data_offset);
      end_sample_ = begin_sample_ + (length / 2);
    }
    subchunk += 8 + length + (length % 2 /*padding*/);
  }
  MUST_EQ(subchunk, data_.size());
}

}  // namespace audio
