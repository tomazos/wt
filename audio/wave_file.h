#pragma once

#include <type_traits>

#include "boost/filesystem.hpp"
#include "core/must.h"

namespace audio {

static constexpr uint32 kSampleRate = 12000;

struct WaveFile {
  WaveFile(filesystem::path source);
  WaveFile(const int16* begin_sample, const int16* end_sample);

  void SaveTo(filesystem::path dest);

  int16 operator[](size_t sample) const { return begin_sample_[sample]; }
  const int16* begin() const { return begin_sample_; }
  const int16* end() const { return end_sample_; }
  size_t size() const { return end_sample_ - begin_sample_; }

 private:
  void SetupFormat();

  template <typename T>
  T Read(size_t offset) {
    STATIC_ASSERT(std::is_trivial<T>::value);
    MUST_LE(offset + sizeof(T), data_.size());

    T t;
    memcpy(&t, data_.data() + offset, sizeof(T));
    return t;
  }

  string data_;
  const int16* begin_sample_ = nullptr;
  const int16* end_sample_ = nullptr;

  WaveFile(const WaveFile&) = delete;
  WaveFile(WaveFile&&) = delete;
  WaveFile& operator=(const WaveFile&) = delete;
  WaveFile& operator=(WaveFile&&) = delete;
};

}  // namespace audio
