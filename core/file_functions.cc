#include "core/file_functions.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "core/must.h"

string GetFileContents(const boost::filesystem::path& path) {
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
    if (read_result == -1) {
      THROW_ERRNO("read(", path, ")");
    }
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
