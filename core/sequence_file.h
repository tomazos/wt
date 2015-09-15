#pragma once

#include <boost/filesystem.hpp>

#include "core/file_functions.h"

class SequenceWriter {
 public:
  SequenceWriter(const filesystem::path& pathname, FileWriteMode mode);
  SequenceWriter(SequenceWriter&&);
  SequenceWriter& operator=(SequenceWriter&&);
  ~SequenceWriter();

  void WriteString(string_view s);
  void WriteMessage(const protobuf::Message& message);

 private:
  void WriteInteger(bigint n);
  void WriteData(const void* buf, size_t len);

  int fd_;

  SequenceWriter(const SequenceWriter&) = delete;
  SequenceWriter& operator=(const SequenceWriter&) = delete;
};

class SequenceReader {
 public:
  SequenceReader(const filesystem::path& pathname);
  SequenceReader(SequenceReader&&);
  SequenceReader& operator=(SequenceReader&&);
  ~SequenceReader();

  optional<string> ReadString();
  [[gnu::warn_unused_result]] bool ReadMessage(protobuf::Message& message);

 private:
  optional<bigint> ReadInteger();
  void ReadData(void* buf, size_t len);

  int fd_;

  SequenceReader(const SequenceReader&) = delete;
  SequenceReader& operator=(const SequenceReader&) = delete;
};
