#include "cm/proto.h"

#include "google/protobuf/descriptor.h"

namespace cm {

using protobuf::EnumDescriptor;
using protobuf::EnumValueDescriptor;
using protobuf::Message;
using protobuf::Descriptor;
using protobuf::FieldDescriptor;
using protobuf::Reflection;

namespace {

void CheckFieldType(const FieldDescriptor* field_descriptor, Type type,
                    const Value& value) {
  if (value.type() != type)
    FAIL(TypeToString(value.type()), " found where ", TypeToString(type),
         " expected in ", field_descriptor->full_name());
}

template <typename CppType, typename ReflectionMemberFunction>
void ValueToSimpleField(ReflectionMemberFunction reflection_member_function,
                        Type type, Message& message,
                        const Reflection* reflection,
                        const FieldDescriptor* field_descriptor,
                        const Value& value) {
  CheckFieldType(field_descriptor, type, value);

  (reflection->*reflection_member_function)(&message, field_descriptor,
                                            CppType(value));
}

template <typename ReflectionMemberFunction>
void ValueToEnumField(ReflectionMemberFunction reflection_member_function,
                      Message& message, const Reflection* reflection,
                      const FieldDescriptor* field_descriptor,
                      const Value& value) {
  CheckFieldType(field_descriptor, Type::STRING, value);

  const string enumerator_string = string(value);

  const EnumDescriptor* enum_descriptor = field_descriptor->enum_type();

  const EnumValueDescriptor* enumerator_descriptor =
      enum_descriptor->FindValueByName(enumerator_string);

  if (enumerator_descriptor == nullptr)
    FAIL("no such enumerator ", enumerator_string, " in ",
         enum_descriptor->full_name());

  (reflection->*reflection_member_function)(&message, field_descriptor,
                                            enumerator_descriptor);
}

template <typename ReflectionMemberFunction>
void ValueToMessageField(ReflectionMemberFunction reflection_member_function,
                         Message& message, const Reflection* reflection,
                         const FieldDescriptor* field_descriptor,
                         const Value& value) {
  CheckFieldType(field_descriptor, Type::TABLE, value);

  Message* field = (reflection->*reflection_member_function)(
      &message, field_descriptor, nullptr);
  TableToProto(value, *field);
}

}  // namespace

void TableToProto(const Value& table, Message& message) {
  const Descriptor* message_descriptor = message.GetDescriptor();
  const Reflection* message_reflection = message.GetReflection();
  MUST(table.type() == Type::TABLE);
  for (const std::pair<Value, Value>& field : table) {
    const Value& field_key = field.first;
    const Value& field_value = field.second;
    MUST(field_key.type() == Type::STRING);
    const string field_name = string(field_key);
    const FieldDescriptor* field_descriptor =
        message_descriptor->FindFieldByName(field_name);
    if (!field_descriptor)
      FAIL("no such field ", field_name, " in ",
           message_descriptor->full_name());
    if (!field_descriptor->is_repeated()) {
      switch (field_descriptor->cpp_type()) {
        case FieldDescriptor::CPPTYPE_INT32:
          ValueToSimpleField<int64>(&Reflection::SetInt32, Type::INTEGER,
                                    message, message_reflection,
                                    field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_INT64:
          ValueToSimpleField<int64>(&Reflection::SetInt64, Type::INTEGER,
                                    message, message_reflection,
                                    field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_UINT32:
          ValueToSimpleField<int64>(&Reflection::SetUInt32, Type::INTEGER,
                                    message, message_reflection,
                                    field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_UINT64:
          ValueToSimpleField<int64>(&Reflection::SetUInt64, Type::INTEGER,
                                    message, message_reflection,
                                    field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_FLOAT:
          ValueToSimpleField<float64>(&Reflection::SetFloat, Type::FLOAT,
                                      message, message_reflection,
                                      field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_DOUBLE:
          ValueToSimpleField<float64>(&Reflection::SetDouble, Type::FLOAT,
                                      message, message_reflection,
                                      field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_BOOL:
          ValueToSimpleField<bool>(&Reflection::SetBool, Type::BOOLEAN, message,
                                   message_reflection, field_descriptor,
                                   field_value);
          break;

        case FieldDescriptor::CPPTYPE_STRING:
          ValueToSimpleField<string>(&Reflection::SetString, Type::STRING,
                                     message, message_reflection,
                                     field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_ENUM:
          ValueToEnumField(&Reflection::SetEnum, message, message_reflection,
                           field_descriptor, field_value);
          break;

        case FieldDescriptor::CPPTYPE_MESSAGE:
          ValueToMessageField(&Reflection::MutableMessage, message,
                              message_reflection, field_descriptor,
                              field_value);
          break;

        default:
          FAIL("Unknown enum field");
      }

    } else {
      MUST(field_value.type() == Type::TABLE, field_descriptor->full_name());
      for (const Value& repeated_value : field_value.ToSequence()) {
        switch (field_descriptor->cpp_type()) {
          case FieldDescriptor::CPPTYPE_INT32:
            ValueToSimpleField<int64>(&Reflection::AddInt32, Type::INTEGER,
                                      message, message_reflection,
                                      field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_INT64:
            ValueToSimpleField<int64>(&Reflection::AddInt64, Type::INTEGER,
                                      message, message_reflection,
                                      field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_UINT32:
            ValueToSimpleField<int64>(&Reflection::AddUInt32, Type::INTEGER,
                                      message, message_reflection,
                                      field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_UINT64:
            ValueToSimpleField<int64>(&Reflection::AddUInt64, Type::INTEGER,
                                      message, message_reflection,
                                      field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_FLOAT:
            ValueToSimpleField<float64>(&Reflection::AddFloat, Type::FLOAT,
                                        message, message_reflection,
                                        field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_DOUBLE:
            ValueToSimpleField<float64>(&Reflection::AddDouble, Type::FLOAT,
                                        message, message_reflection,
                                        field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_BOOL:
            ValueToSimpleField<bool>(&Reflection::AddBool, Type::BOOLEAN,
                                     message, message_reflection,
                                     field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_STRING:
            ValueToSimpleField<string>(&Reflection::AddString, Type::STRING,
                                       message, message_reflection,
                                       field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_ENUM:
            ValueToEnumField(&Reflection::AddEnum, message, message_reflection,
                             field_descriptor, repeated_value);
            break;

          case FieldDescriptor::CPPTYPE_MESSAGE:
            ValueToMessageField(&Reflection::AddMessage, message,
                                message_reflection, field_descriptor,
                                repeated_value);
            break;

          default:
            FAIL("Unknown enum field");
        }
      }
    }
  }
}

// SetInt32(Message * message, const FieldDescriptor * field, int32 value) const
// = 0
// SetInt64(Message * message, const FieldDescriptor * field, int64 value) const
// = 0
// SetUInt32(Message * message, const FieldDescriptor * field, uint32 value)
// const = 0
// SetUInt64(Message * message, const FieldDescriptor * field, uint64 value)
// const = 0
// SetFloat(Message * message, const FieldDescriptor * field, float value) const
// = 0
// SetDouble(Message * message, const FieldDescriptor * field, double value)
// const = 0
// SetBool(Message * message, const FieldDescriptor * field, bool value) const =
// 0
// SetString(Message * message, const FieldDescriptor * field, const string &
// value) const = 0
// SetEnum(Message * message, const FieldDescriptor * field, const
// EnumValueDescriptor * value) const = 0
// SetEnumValue(Message * message, const FieldDescriptor * field, int value)
// const
// virtual Message*	MutableMessage(Message * message, const FieldDescriptor
// * field, MessageFactory * factory = NULL) const = 0

// AddInt32(Message * message, const FieldDescriptor * field, int32 value) const
// = 0
// virtual void
// AddInt64(Message * message, const FieldDescriptor * field, int64 value) const
// = 0
// virtual void
// AddUInt32(Message * message, const FieldDescriptor * field, uint32 value)
// const = 0
// virtual void
// AddUInt64(Message * message, const FieldDescriptor * field, uint64 value)
// const = 0
// virtual void
// AddFloat(Message * message, const FieldDescriptor * field, float value) const
// = 0
// virtual void
// AddDouble(Message * message, const FieldDescriptor * field, double value)
// const = 0
// virtual void
// AddBool(Message * message, const FieldDescriptor * field, bool value) const =
// 0
// virtual void
// AddString(Message * message, const FieldDescriptor * field, const string &
// value) const = 0
// virtual void
// AddEnum(Message * message, const FieldDescriptor * field, const
// EnumValueDescriptor * value) const = 0
// virtual void
// AddEnumValue(Message * message, const FieldDescriptor * field, int value)
// const
// Set an enum field's value with an integer rather than EnumValueDescriptor.
// more...
// virtual Message *
// AddMessage(Message * message, const FieldDescriptor * field,

}  // namespace cm
