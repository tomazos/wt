#pragma once

#include "google/protobuf/message.h"
#include "xxua/value.h"

namespace xxua {

void TableToProto(const Value& table, protobuf::Message& message);

}  // namespace xxua
