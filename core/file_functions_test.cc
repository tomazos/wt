#include "core/file_functions.h"

#include <boost/filesystem.hpp>

#include "core/must.h"
#include "gtest/gtest.h"
#include "core/file_functions_test.pb.h"

TEST(FileFunctionsTest, FileAttributes) {
  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();

  SetFileContents(tmpfile, string(1'000'000, 'a'));
  auto last_mod = LastModificationTime(tmpfile);
  EXPECT_EQ(GetFileContents(tmpfile), string(1'000'000, 'a'));

  {
    TestFileAttributes attributes;
    GetFileAttribute(tmpfile, "user.testattrs", attributes);
    EXPECT_FALSE(attributes.has_x());
    attributes.set_x(42);
    SetFileAttribute(tmpfile, "user.testattrs", attributes);
  }
  EXPECT_EQ(LastModificationTime(tmpfile), last_mod);
  Sleep(10ms);
  SetFileContents(tmpfile, string(1'000'000, 'b'));
  EXPECT_GT(int64(LastModificationTime(tmpfile)), int64(last_mod));
  EXPECT_EQ(GetFileContents(tmpfile), string(1'000'000, 'b'));
  {
    TestFileAttributes attributes;
    GetFileAttribute(tmpfile, "user.testattrs", attributes);
    EXPECT_TRUE(attributes.has_x());
    EXPECT_EQ(attributes.x(), 42);
  }

  EXPECT_TRUE(filesystem::remove(tmpfile));
}
