#pragma once

namespace torrent {

struct AriaConfig {
  optional<filesystem::path> dir;
  optional<filesystem::path> input_file;
  optional<filesystem::path> log_file;
  optional<int> max_concurrent_downloads;
  optional<bool> check_integrity;
};

}  // namespace torrent
