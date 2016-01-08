#include <iostream>

#include "core/hex.h"
#include "core/must.h"
#include "database/postgresql/connection.h"
#include "gtest/gtest.h"

namespace database {
namespace postgresql {

template <typename T>
void TestReadWrite(const string& sqltype, T val, const string& sqlvalue) {
  Connection connection(
      "postgresql://postgres:klaseddOcs0@localhost?dbname=cppsrc");
  connection.ExecuteCommand("drop table if exists test_table");
  connection.ExecuteCommand(
      EncodeAsString("create table test_table (my_x ", sqltype, ")"));

  connection.ExecuteCommand(
      EncodeAsString("insert into test_table (my_x) values ($1)"), val);
  Result result = connection.ExecuteCommand(
      EncodeAsString("select my_x from test_table where my_x = ", sqlvalue));
  EXPECT_EQ(result.NumRows(), 1);
  EXPECT_EQ(result.NumCols(), 1);
  optional<T> field = result.Get<T>(0, 0);
  EXPECT_EQ(field.value(), val);

  connection.ExecuteCommand("drop table test_table");
}

TEST(PostgresqlTest, Bool) {
  TestReadWrite("boolean", true, "true");
  TestReadWrite("boolean", false, "false");
}

TEST(PostgresqlTest, Ints) {
  TestReadWrite<int16>("int2", 0, "0");
  TestReadWrite<int16>("int2", 12345, "12345");
  TestReadWrite<int16>("int2", -12345, "-12345");
  TestReadWrite<int16>("int2", 32767, "32767");
  TestReadWrite<int16>("int2", -32768, "-32768");
  TestReadWrite<int32>("int4", 2147483647, "2147483647");
  TestReadWrite<int32>("int4", -2147483648, "-2147483648");
  TestReadWrite<int64>("int8", 9223372036854775807, "9223372036854775807");
  TestReadWrite<int64>("int8", -9223372036854775807 - 1,
                       "-9223372036854775808");
}

TEST(PostgresqlTest, Floats) {
  TestReadWrite<float32>("float4", 4.5f, "4.5");
  TestReadWrite<float64>("float8", 4.5, "4.5");
}

TEST(PostgresqlTest, Bytea) {
  for (int i = 0; i < 256; ++i) {
    uint8 a[1] = {uint8(i)};
    string h = "E'\\\\x" + ByteArrayToHexString(a, 1) + "'";

    TestReadWrite<bytea>("bytea", bytea{string((char*)a, 1)}, h);
  }
}

TEST(PostgresqlTest, Text) { TestReadWrite<string>("text", "abc", "'abc'"); }

}  // namespace sqlite
}  // namespace database
