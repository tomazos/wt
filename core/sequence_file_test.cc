#include "core/sequence_file.h"

#include "core/sequence_file_test.pb.h"
#include "gtest/gtest.h"

TEST(SequenceFileTest, SingleReadWrite) {
  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();

  {
    SequenceWriter writer(tmpfile, OVERWRITE);

    writer.WriteString("foo");
  }
  {
    SequenceReader reader(tmpfile);
    EXPECT_EQ(reader.ReadString().value(), "foo");
    EXPECT_FALSE(reader.ReadString());
  }

  filesystem::remove(tmpfile);
}

TEST(SequenceFileTest, MultipleReadWrite) {
  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();

  {
    SequenceWriter writer(tmpfile, OVERWRITE);

    for (int i = 0; i < 10000; i++) {
      SequenceFileTestProto proto;
      proto.set_data(std::string(i, char('a' + i % 26)));
      proto.set_code(i);
      writer.WriteMessage(proto);
    }
  }
  {
    SequenceReader reader(tmpfile);

    for (int i = 0; i < 10000; i++) {
      SequenceFileTestProto proto;
      EXPECT_TRUE(reader.ReadMessage(proto));
      EXPECT_EQ(proto.data(), std::string(i, char('a' + i % 26)));
      EXPECT_EQ(proto.code(), i);
    }

    SequenceFileTestProto proto;
    EXPECT_FALSE(reader.ReadMessage(proto));
  }

  filesystem::remove(tmpfile);
}
