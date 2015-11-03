#pragma once

#include "google/protobuf/message.h"
#include "cm/value.h"

namespace cm {

void TableToProto(const Value& table, protobuf::Message& message);

}  // namespace cm
