#include "audio/srtproto_file.pb.h"
#include "core/file_functions.h"
#include "core/must.h"
#include "main/args.h"

void Main(const std::vector<string>& args) {
  MUST_EQ(args.size(), 1u);
  filesystem::path p = args[0];
  string s = GetFileContents(p);
  audio::SrtProtoFile srtproto;
  MUST(srtproto.ParseFromString(s));
  int64 count = 0;
  int64 total_diff = 0;
  int64 total_len = 0;
  for (const audio::Subtitle& subtitle : srtproto.subtitles()) {
    count++;
    int64 diff = subtitle.end_ms() - subtitle.begin_ms();
    total_diff += diff;
    int64 len = subtitle.text().size();
    total_len += len;
    std::cout << subtitle.DebugString() << std::endl;
  }
  std::cout << std::endl;
  std::cout << count << " subtitles" << std::endl;
  if (count > 0) {
    std::cout << total_diff / count << "ms average" << std::endl;
    std::cout << total_len / count << "chars average" << std::endl;
  }
}
