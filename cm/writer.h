#pragma once

#include <ostream>

namespace xxua {

class Writer {
 public:
  virtual void Write(const void* data, size_t size) = 0;

  virtual ~Writer() = default;
};

class OutputStreamWriter : public Writer {
 public:
  OutputStreamWriter(std::ostream& output_stream)
      : output_stream_(output_stream) {}

  void Write(const void* data, size_t size) override {
    output_stream_.write(static_cast<const char*>(data), size);
  }

 private:
  std::ostream& output_stream_;
};

}  // namespace xxua
