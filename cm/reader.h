#pragma once

#include <istream>

#include "core/must.h"

namespace cm {

class Reader {
 public:
  struct Buffer {
    const char* data;
    size_t size;
  };

  static constexpr Buffer Done{nullptr, 0};

  virtual Buffer Read() = 0;

  virtual ~Reader() = default;
};

template <size_t kBufferSize = 4096>
class InputStreamReader : public Reader {
 public:
  InputStreamReader(std::istream& input_stream) : input_stream_(input_stream) {}

  Buffer Read() {
    input_stream_.read(buffer_, kBufferSize);
    size_t bytes_read = input_stream_.gcount();
    if (bytes_read != 0) return {buffer_, bytes_read};

    MUST(input_stream_.eof());
    return Done;
  }

 private:
  std::istream& input_stream_;
  char buffer_[kBufferSize];
};

}  // namespace cm
