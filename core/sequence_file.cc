#include "core/sequence_file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "core/must.h"

SequenceWriter::SequenceWriter(const filesystem::path& pathname,
                               FileWriteMode mode)
    : fd_(open(pathname.string().c_str(), O_WRONLY | O_CREAT | O_APPEND |
                                              (mode == OVERWRITE ? O_TRUNC : 0),
               S_IRUSR | S_IWUSR)) {
  if (fd_ == -1) THROW_ERRNO("open ", pathname);
}

SequenceWriter::SequenceWriter(SequenceWriter&& that) : fd_(that.fd_) {
  that.fd_ = -1;
}

SequenceWriter& SequenceWriter::operator=(SequenceWriter&& that) {
  fd_ = that.fd_;
  that.fd_ = -1;
  return *this;
}

SequenceWriter::~SequenceWriter() {
  if (fd_ != -1) {
    int close_result = close(fd_);
    if (close_result != 0) THROW_ERRNO("close");
  }
}

void SequenceWriter::WriteData(const void* buf, size_t len) {
  const char* p = (const char*)buf;
  const char* const end = p + len;
  while (p < end) {
    ssize_t write_result = write(fd_, buf, end - p);
    if (write_result < 0) THROW_ERRNO("write");
    p += write_result;
  }
}

void SequenceWriter::WriteString(string_view value) {
  std::string i = pack_bigint(value.size());
  WriteData(i.data(), i.size());
  WriteData(value.data(), value.size());
}

void SequenceWriter::WriteMessage(const protobuf::Message& message) {
  std::string s;
  if (!message.SerializeToString(&s)) {
    FAIL("Unabled to serialize: ", message.DebugString());
  }
  WriteString(s);
}

SequenceReader::SequenceReader(const filesystem::path& pathname)
    : fd_(open(pathname.string().c_str(), O_RDONLY)) {
  if (fd_ == -1) THROW_ERRNO("open ", pathname);
}

SequenceReader::SequenceReader(SequenceReader&& that) : fd_(that.fd_) {
  that.fd_ = -1;
}

SequenceReader& SequenceReader::operator=(SequenceReader&& that) {
  fd_ = that.fd_;
  that.fd_ = -1;
  return *this;
}

SequenceReader::~SequenceReader() {
  if (fd_ != -1) {
    int close_result = close(fd_);
    if (close_result != 0) THROW_ERRNO("close");
  }
}

void SequenceReader::ReadData(void* buf, size_t len) {
  if (len == 0) return;
  char* ptr = static_cast<char*>(buf);
  const char* end = ptr + len;
  while (ptr < end) {
    ssize_t received_bytes = read(fd_, ptr, end - ptr);
    if (received_bytes < 0) THROW_ERRNO("read");
    if (received_bytes == 0) FAIL("Unexpected end-of-stream.");
    ptr += received_bytes;
  }
}

optional<bigint> SequenceReader::ReadInteger() {
  return unpack_bigint([&]() -> optional<uint8> {
    uint8 m;
    ssize_t read_result = read(fd_, &m, 1);
    if (read_result == 0)
      return nullopt;
    else if (read_result == -1)
      THROW_ERRNO("read");
    else
      return m;
  });
}

optional<string> SequenceReader::ReadString() {
  optional<bigint> string_length = ReadInteger();
  if (!string_length) return nullopt;
  MUST_LE(*string_length, std::numeric_limits<size_t>::max());
  string result(size_t(*string_length), '\0');
  ReadData(&result[0], result.size());
  return result;
}

[[gnu::warn_unused_result]] bool SequenceReader::ReadMessage(
    protobuf::Message& message) {
  optional<string> message_string = ReadString();
  if (!message_string) return false;
  const bool parse_success = message.ParseFromString(*message_string);
  if (!parse_success) {
    FAIL("ParseFromString failed.");
  }
  return true;
}
