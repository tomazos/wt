#include "audio/srtproto_file.pb.h"

#include "boost/filesystem.hpp"

#include "core/env.h"
#include "core/must.h"
#include "core/file_functions.h"
#include "gtest/gtest.h"

TEST(SrtProtoTest, Parsing) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path srtproto_path =
      source_root / "audio/testdata/subtitles.srtproto";
  string srtproto_string = GetFileContents(srtproto_path);
  audio::SrtProtoFile srtproto;
  MUST(srtproto.ParseFromString(srtproto_string));
}
