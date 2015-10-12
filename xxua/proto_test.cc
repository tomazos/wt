#include "xxua/proto_test.pb.h"
#include "xxua/proto.h"

#include "gtest/gtest.h"

namespace xxua {

TEST(ProtoTest, Smoke) { ProtoTestMessage m; }

TEST(ProtoTest, Basic) {
  xxua::State state;
  xxua::Context context(state);

  ProtoTestMessage m;
  xxua::TableToProto(xxua::Compile(R"(
    return {
      optional_int64_field = 42,
    };
  )")({}).at(0),
                     m);

  EXPECT_EQ(m.optional_int64_field(), 42);
}

TEST(ProtoTest, Full) {
  xxua::State state;
  xxua::Context context(state);

  ProtoTestMessage m;
  xxua::TableToProto(xxua::Compile(R"(
    return {
      optional_double_field = 2.5,
      optional_float_field = 3.5,
      optional_int32_field = 42,
      optional_int64_field = 43,
      optional_uint32_field = 44,
      optional_uint64_field = 45,
      optional_sint32_field = 46,
      optional_sint64_field = 47,
      optional_fixed32_field = 48,
      optional_fixed64_field = 49,
      optional_sfixed32_field = 50,
      optional_sfixed64_field = 51,
      optional_bool_field = true,
      optional_string_field = "foo",
      optional_bytes_field = "bar",
      optional_enum_field = "ProtoTestEnumTwo",
      optional_message_field = { i = 13, s = "baz" },

      repeated_double_field = { 1.5, 2.5, 3.5 },
      repeated_float_field = { 4.5, 5.5 },
      repeated_int32_field = { 101, 102, 103 },
      repeated_int64_field = { 201, 202, 203 },
      repeated_uint32_field = { 301, 302, 303 },
      repeated_uint64_field = { 401, 402, 403 },
      repeated_sint32_field = { 501, 502, 503 },
      repeated_sint64_field = { 601, 602, 603 },
      repeated_fixed32_field = { 701, 702, 703 },
      repeated_fixed64_field = { 801, 802, 803 },
      repeated_sfixed32_field = { 901, 902, 903 },
      repeated_sfixed64_field = { 1001, 1002, 1003 },
      repeated_bool_field = { true, false, true, true, false },
      repeated_string_field = { "foo", "bar", "baz", "qux", "quux" },
      repeated_bytes_field = { "foo", "bar", "baz", "qux", "quux" },
      repeated_enum_field = { "ProtoTestEnumThree", "ProtoTestEnumOne", "ProtoTestEnumTwo", "ProtoTestEnumTwo"},
      repeated_message_field = {
        { i = 3, s = "three" },
        { i = 1, s = "one" },
        { i = 2, s = "two" },
        { i = 2, s = "two" },
      },
    };
  )")({}).at(0),
                     m);

  EXPECT_EQ(m.optional_double_field(), 2.5);
  EXPECT_EQ(m.optional_float_field(), 3.5f);
  EXPECT_EQ(m.optional_int32_field(), 42);
  EXPECT_EQ(m.optional_int64_field(), 43);
  EXPECT_EQ(m.optional_uint32_field(), 44u);
  EXPECT_EQ(m.optional_uint64_field(), 45u);
  EXPECT_EQ(m.optional_sint32_field(), 46);
  EXPECT_EQ(m.optional_sint64_field(), 47);
  EXPECT_EQ(m.optional_fixed32_field(), 48u);
  EXPECT_EQ(m.optional_fixed64_field(), 49u);
  EXPECT_EQ(m.optional_sfixed32_field(), 50);
  EXPECT_EQ(m.optional_sfixed64_field(), 51);
  EXPECT_EQ(m.optional_bool_field(), true);
  EXPECT_EQ(m.optional_string_field(), "foo");
  EXPECT_EQ(m.optional_bytes_field(), "bar");
  EXPECT_EQ(m.optional_enum_field(), ProtoTestEnumTwo);
  EXPECT_EQ(m.optional_message_field().i(), 13);
  EXPECT_EQ(m.optional_message_field().s(), "baz");

  EXPECT_EQ(m.repeated_double_field_size(), 3);
  EXPECT_EQ(m.repeated_double_field(0), 1.5);
  EXPECT_EQ(m.repeated_double_field(1), 2.5);
  EXPECT_EQ(m.repeated_double_field(2), 3.5);
  EXPECT_EQ(m.repeated_float_field_size(), 2);
  EXPECT_EQ(m.repeated_float_field(0), 4.5f);
  EXPECT_EQ(m.repeated_float_field(1), 5.5f);

#define TRF(kind, val1, val2, val3)               \
  EXPECT_EQ(m.repeated_##kind##_field_size(), 3); \
  EXPECT_EQ(m.repeated_##kind##_field(0), val1);  \
  EXPECT_EQ(m.repeated_##kind##_field(1), val2);  \
  EXPECT_EQ(m.repeated_##kind##_field(2), val3)

  TRF(int32, 101, 102, 103);
  TRF(int64, 201, 202, 203);
  TRF(uint32, 301u, 302u, 303u);
  TRF(uint64, 401u, 402u, 403u);
  TRF(sint32, 501, 502, 503);
  TRF(sint64, 601, 602, 603);
  TRF(fixed32, 701u, 702u, 703u);
  TRF(fixed64, 801u, 802u, 803u);
  TRF(sfixed32, 901, 902, 903);
  TRF(sfixed64, 1001, 1002, 1003);

  EXPECT_EQ(m.repeated_bool_field_size(), 5);
  EXPECT_TRUE(m.repeated_bool_field(0));
  EXPECT_FALSE(m.repeated_bool_field(1));
  EXPECT_TRUE(m.repeated_bool_field(2));
  EXPECT_TRUE(m.repeated_bool_field(3));
  EXPECT_FALSE(m.repeated_bool_field(4));

  EXPECT_EQ(m.repeated_string_field_size(), 5);
  EXPECT_EQ(m.repeated_string_field(0), "foo");
  EXPECT_EQ(m.repeated_string_field(1), "bar");
  EXPECT_EQ(m.repeated_string_field(2), "baz");
  EXPECT_EQ(m.repeated_string_field(3), "qux");
  EXPECT_EQ(m.repeated_string_field(4), "quux");

  EXPECT_EQ(m.repeated_bytes_field_size(), 5);
  EXPECT_EQ(m.repeated_bytes_field(0), "foo");
  EXPECT_EQ(m.repeated_bytes_field(1), "bar");
  EXPECT_EQ(m.repeated_bytes_field(2), "baz");
  EXPECT_EQ(m.repeated_bytes_field(3), "qux");
  EXPECT_EQ(m.repeated_bytes_field(4), "quux");

  EXPECT_EQ(m.repeated_enum_field_size(), 4);
  EXPECT_EQ(m.repeated_enum_field(0), ProtoTestEnumThree);
  EXPECT_EQ(m.repeated_enum_field(1), ProtoTestEnumOne);
  EXPECT_EQ(m.repeated_enum_field(2), ProtoTestEnumTwo);
  EXPECT_EQ(m.repeated_enum_field(3), ProtoTestEnumTwo);

  EXPECT_EQ(m.repeated_message_field_size(), 4);
  EXPECT_EQ(m.repeated_message_field(0).i(), 3);
  EXPECT_EQ(m.repeated_message_field(1).i(), 1);
  EXPECT_EQ(m.repeated_message_field(2).i(), 2);
  EXPECT_EQ(m.repeated_message_field(3).i(), 2);
  EXPECT_EQ(m.repeated_message_field(0).s(), "three");
  EXPECT_EQ(m.repeated_message_field(1).s(), "one");
  EXPECT_EQ(m.repeated_message_field(2).s(), "two");
  EXPECT_EQ(m.repeated_message_field(3).s(), "two");
}

}  // namespace xxua
