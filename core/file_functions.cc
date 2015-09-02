#include "core/file_functions.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <fcntl.h>

#include "core/must.h"

string GetFileContents(const filesystem::path& path) {
  const uintmax_t file_size_result = file_size(path);
  MUST_NE(static_cast<uintmax_t>(-1), file_size_result, "file_size (", path,
          ") failed");
  MUST_LE(file_size_result, uintmax_t(std::numeric_limits<ssize_t>::max()),
          "file too large");

  string result(size_t(file_size_result), '\0');
  const string pathname = path.string();
  int fd = open(pathname.c_str(), O_RDONLY);
  if (fd == -1) THROW_ERRNO("open(", path, ")");

  char* buf = &result[0];
  size_t bytes_remaining = ssize_t(file_size_result);

  while (bytes_remaining > 0) {
    ssize_t read_result = read(fd, buf, bytes_remaining);
    MUST_NE(0, read_result, "unexpected end of file");
    if (read_result == -1) THROW_ERRNO("read(", path, ")");
    MUST_GT(read_result, 0);
    size_t bytes_read = size_t(read_result);
    MUST_LE(bytes_read, bytes_remaining);
    buf += bytes_read;
    bytes_remaining -= bytes_read;
  }

  int close_result = close(fd);
  if (close_result != 0) THROW_ERRNO("close(", path, ")");
  return result;
}

void SetFileContents(const filesystem::path& path, const string& content) {
  const string pathname = path.string();
  int fd =
      open(pathname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) THROW_ERRNO("open(", path, ")");

  const char* start = content.data();
  const char* end = start + content.size();
  while (start != end) {
    ssize_t write_result = write(fd, start, end - start);
    if (write_result == -1) THROW_ERRNO("write(", path, ")");
    MUST_LE(write_result, end - start);
    start += write_result;
  }

  int close_result = close(fd);
  if (close_result != 0) THROW_ERRNO("close(", path, ")");
}

void GetFileAttribute(const filesystem::path& path, const string& name,
                      protobuf::Message& message) {
  const string path_string = path.string();
  ssize_t getxattr_result =
      getxattr(path_string.c_str(), name.c_str(), nullptr, 0);
  if (getxattr_result == -1) {
    if (errno == ENODATA)
      return;
    else
      THROW_ERRNO("getxattr(", path, ", ", name, ")");
  }
  size_t size = getxattr_result;
  std::vector<char> buf(size);
  getxattr_result =
      getxattr(path_string.c_str(), name.c_str(), buf.data(), size);
  if (getxattr_result == -1) THROW_ERRNO("getxattr(", path, ", ", name, ")");
  if (size_t(getxattr_result) != size)
    FAIL("size mismatch: getxattr(", path_string, ", ", name, ")");
  if (!message.ParseFromArray(buf.data(), buf.size()))
    FAIL("unable to parse message: getxattr(", path, ", ", name, ")");
}

void SetFileAttribute(const filesystem::path& path, const string& name,
                      const protobuf::Message& message) {
  string buf;
  if (!message.SerializeToString(&buf))
    FAIL("can't serialize: ", message.DebugString());
  int setxattr_result = setxattr(path.string().c_str(), name.c_str(),
                                 buf.data(), buf.size(), 0 /* flags */);
  if (setxattr_result != 0) THROW_ERRNO("setxattr(", path, ", ", name, ")");
}

int64 LastModificationTime(const filesystem::path& p) {
  struct stat s;
  int stat_result = stat(p.string().c_str(), &s);
  if (stat_result != 0) THROW_ERRNO("stat(", p, ")");
  return int64(s.st_mtim.tv_sec) * 1'000'000'000 + s.st_mtim.tv_nsec;
}
