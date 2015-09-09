#include "core/sha3.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "core/env.h"
#include "core/file_functions.h"
#include "core/hex.h"
#include "gtest/gtest.h"

#define EXPECT_SHA(f, input, output)                     \
  EXPECT_EQ(ByteArrayToHexString(f(string_view(input))), \
            ByteArrayToHexString(HexStringToByteArray(output)))

#define EXPECT_SHAKE(f, input, output_len, output)                   \
  EXPECT_EQ(ByteArrayToHexString(f(string_view(input), output_len)), \
            ByteArrayToHexString(HexStringToByteArray(output)))

TEST(Sha3Test, Empty) {
  EXPECT_SHA(
      SHA3_256, "",
      "A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A");

  EXPECT_SHA(SHA3_224, "",
             "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");

  EXPECT_SHA(
      SHA3_256, "",
      "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");
  EXPECT_SHA(SHA3_384, "",
             "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac"
             "3713831264adb47fb6bd1e058d5f004");

  EXPECT_SHA(SHA3_512, "",
             "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a61"
             "5b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26");

  EXPECT_SHAKE(
      SHAKE128, "", 256 / 8,
      "7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26");

  EXPECT_SHAKE(
      SHAKE256, "", 512 / 8,
      "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4dd"
      "d8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be");

  EXPECT_SHAKE(
      SHAKE128, "The quick brown fox jumps over the lazy dog", 256 / 8,
      "f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e");

  EXPECT_SHAKE(
      SHAKE128, "The quick brown fox jumps over the lazy dof", 256 / 8,
      "853f4538be0db9621a6cea659a06c1107b1f83f02b13d18297bd39d7411cf10c");
}

using ByteArray = std::vector<uint8>;

void ParseTestFile(filesystem::path testfile,
                   std::function<void(std::map<string, string>)> on_entry) {
  string testdata = GetFileContents(testfile);
  EXPECT_GT(testdata.size(), 0u);
  std::map<string, string> current_entry;
  Scan(testdata, "\n", [&](string_view line) {
    if (line[0] == '#')
      return;
    else if (line.empty()) {
      if (current_entry.empty()) return;

      auto entry = current_entry;
      current_entry.clear();
      on_entry(entry);
    } else {
      size_t pos = line.find(" = ");
      ASSERT_NE(pos, string::npos);
      current_entry[line.substr(0, pos).to_string()] =
          line.substr(pos + 3).to_string();
    }
  });

  if (!current_entry.empty()) on_entry(current_entry);
}

template <typename F>
void ShakeFileTest(F shake, filesystem::path testfile) {
  ParseTestFile(testfile, [&](std::map<string, string> entry) {
    ASSERT_EQ(entry.size(), 3u);
    ASSERT_TRUE(entry.count("Len"));
    ASSERT_TRUE(entry.count("Msg"));
    ASSERT_TRUE(entry.count("Squeezed"));
    int len = std::stoi(entry["Len"]);
    if (len == 0u || len % 8 != 0u) return;
    auto msg = HexStringToByteArray(entry["Msg"]);
    auto squeezed = entry["Squeezed"];

    EXPECT_EQ(ByteArrayToHexString(shake(msg, squeezed.size() / 2)), squeezed);
  });
}

template <typename F>
void ShaFileTest(F sha, filesystem::path testfile) {
  ParseTestFile(testfile, [&](std::map<string, string> entry) {
    ASSERT_EQ(entry.size(), 3u);
    ASSERT_TRUE(entry.count("Len"));
    ASSERT_TRUE(entry.count("Msg"));
    ASSERT_TRUE(entry.count("MD"));
    int len = std::stoi(entry["Len"]);
    if (len == 0u || len % 8 != 0u) return;
    auto msg = HexStringToByteArray(entry["Msg"]);
    ASSERT_EQ(int(msg.size()), len / 8);

    EXPECT_EQ(ByteArrayToHexString(sha(msg)), entry["MD"]);
  });
}

TEST(Sha3Test, Files) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");

  ShakeFileTest([](const ByteArray& input, int len) {
    return SHAKE128(input, len);
  }, source_root / "core/testdata/ShortMsgKAT_SHAKE128.txt");
  ShakeFileTest([](const ByteArray& input, int len) {
    return SHAKE256(input, len);
  }, source_root / "core/testdata/ShortMsgKAT_SHAKE256.txt");
  ShaFileTest([](const ByteArray& input) { return SHA3_224(input); },
              source_root / "core/testdata/ShortMsgKAT_SHA3-224.txt");
  ShaFileTest([](const ByteArray& input) { return SHA3_256(input); },
              source_root / "core/testdata/ShortMsgKAT_SHA3-256.txt");
  ShaFileTest([](const ByteArray& input) { return SHA3_384(input); },
              source_root / "core/testdata/ShortMsgKAT_SHA3-384.txt");
  ShaFileTest([](const ByteArray& input) { return SHA3_512(input); },
              source_root / "core/testdata/ShortMsgKAT_SHA3-512.txt");
}
