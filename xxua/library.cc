#include "xxua/library.h"

#include <map>

#include "core/collection_functions.h"
#include "core/must.h"
#include "xxua/api.h"

namespace xxua {

static std::map<string, std::function<int()>>& GetFunctions(
    const char* funcset) {
  static std::map<string, std::map<string, std::function<int()>>> fs;
  return fs[funcset];
}

struct FunctionRegistrar {
  FunctionRegistrar(const char* funcset, const string& name,
                    std::function<int()> f) {
    MUST(::Insert(GetFunctions(funcset), name, f));
  }
};

#define REGISTER_FUNCTION(funcset, f)                          \
  static int f##__function();                                  \
  FunctionRegistrar register__##f(funcset, #f, f##__function); \
  static int f##__function()

#define GLOBAL_FUNCTION(f) REGISTER_FUNCTION("global", f)
#define STRING_FUNCTION(f) REGISTER_FUNCTION("string", f)

namespace library {

GLOBAL_FUNCTION(throw) {
  Concat(StackSize());
  Throw();
}

GLOBAL_FUNCTION(keyvals) {
  if (GetType(1) != Type::TABLE) Error("expected table");
  PushFunction([] {
    if (GetType(1) != Type::TABLE) Error("expected table");
    if (GetType(2) == Type::NONE) Error("expected 2 parameters");
    if (Next(1)) {
      if (StackSize() != 3) Error("next internal error");
      return 2;
    } else
      return 0;
  });
  PushCopy(1);
  PushNil();
  return 3;
}

GLOBAL_FUNCTION(strlen) {
  if (GetType(1) != Type::STRING) Error("strlen expected string");
  LEN(1);
  return 1;
}

[[noreturn]] static void ThrowInvalidCast(string_view enclosing, int arg) {
  Throw(EncodeAsString("unexpected arg to ", enclosing, ": ",
                       TypeToString(GetType(arg))));
}

static void AppendArg(const string& enclosing, std::ostream& os, int arg) {
  switch (GetType(arg)) {
    case Type::STRING:
      os << ToString(arg);
      return;
    case Type::INTEGER:
      os << ToInteger(arg);
      return;
    case Type::FLOAT:
      os << ToFloat(arg);
      return;
    case Type::BOOLEAN:
      os << (ToBoolean(arg) ? "1" : "0");
      return;
    default:
      ThrowInvalidCast(enclosing, arg);
  }
}

GLOBAL_FUNCTION(string) {
  if (StackSize() != 1) Throw("invalid num args to string");
  if (GetType(1) == Type::STRING)
    return 1;
  else {
    std::ostringstream oss;
    AppendArg("string", oss, 1);
    PushString(oss.str());
    return 1;
  }
}

GLOBAL_FUNCTION(cat) {
  int args = StackSize();
  std::ostringstream oss;
  for (int i = 1; i <= args; ++i) {
    AppendArg("cat", oss, i);
  }
  PushString(oss.str());
  return 1;
}

GLOBAL_FUNCTION(join) {
  if (GetType(1) != Type::STRING) Throw("first arg to join not string");
  string_view delim = ToString(1);
  int args = StackSize();
  std::ostringstream oss;
  for (int i = 2; i <= args - 1; ++i) {
    AppendArg("join", oss, i);
    oss << delim;
  }
  AppendArg("join", oss, args);
  PushString(oss.str());
  return 1;
}

GLOBAL_FUNCTION(split) {
  if (StackSize() == 1) PushString(" ");
  if (StackSize() != 2) Throw("invalid num args to split");
  if (GetType(1) != Type::STRING) Throw("first arg to split not string");
  if (GetType(2) != Type::STRING) Throw("second arg to split not string");
  string_view subject = ToString(1);
  string_view delim = ToString(2);
  if (delim.empty()) Throw("delim arg in split empty");
  size_t pos = 0;
  int nresults = 0;
  while (true) {
    const size_t next_delim = subject.find(delim, pos);
    if (next_delim != string::npos) {
      PushString(subject.substr(pos, next_delim - pos));
      pos = next_delim + delim.size();
      nresults++;
    } else {
      PushString(subject.substr(pos));
      nresults++;
      return nresults;
    }
  }
}

STRING_FUNCTION(size) {
  if (StackSize() != 1) Throw("invalid num args to string:size");
  if (GetType(1) != Type::STRING) Throw("string:size expected string");
  PushInteger(ToString(1).size());
  return 1;
}

STRING_FUNCTION(empty) {
  if (StackSize() != 1) Throw("invalid num args to string:empty");
  if (GetType(1) != Type::STRING) Throw("string:empty expected string");
  PushBoolean(ToString(1).empty());
  return 1;
}

STRING_FUNCTION(find) {
  if (StackSize() == 2) PushInteger(1);
  if (StackSize() != 3 || GetType(1) != Type::STRING ||
      GetType(2) != Type::STRING || GetType(3) != Type::INTEGER)
    Throw("invalid arguments to string:find");
  const string_view subject = ToString(1);
  const string_view target = ToString(2);
  const int pos = ToInteger(3) - 1;
  const auto result = subject.find(target, pos);
  if (result == string::npos)
    return 0;
  else {
    PushInteger(result + 1);
    return 1;
  }
}

STRING_FUNCTION(at) {
  if (StackSize() != 2 || GetType(1) != Type::STRING ||
      GetType(2) != Type::INTEGER)
    Throw("invalid arguments to string:at");

  const string_view subject = ToString(1);
  const size_t pos = ToInteger(2) - 1;
  if (pos < 0 || pos >= subject.size()) Error("string:at index out of range");
  PushInteger(subject[pos]);
  return 1;
}

STRING_FUNCTION(substr) {
  if (GetType(1) != Type::STRING || GetType(2) != Type::INTEGER)
    Throw("invalid arguments to string:substr");
  string_view subject = ToString(1);
  size_t pos = ToInteger(2) - 1;
  size_t count = (StackSize() == 2 ? string::npos : ToInteger(3));
  PushString(subject.substr(pos, count));
  return 1;
}

}  // namespace library

static void PopFunctionsToTable(const char* funcset) {
  for (auto kv : GetFunctions(funcset)) {
    const string& name = kv.first;
    std::function<int()> f = kv.second;

    PushString(name);
    PushFunction(f);
    PopField(-3);
  }
}

static void InstallGlobals() {
  PushGlobalTable();
  PopFunctionsToTable("global");
  Pop();
}

static void InstallStrings() {
  PushString("");
  if (!PushMetatable(-1)) {
    PushNewTable();
  }
  PushString("__index");
  PushField(-2);
  if (GetType(-1) != Type::TABLE) {
    Pop();
    PushNewTable();
  }
  PushString("__index");
  Insert(-2);
  PopFunctionsToTable("string");
  PopField(-3);
  PopMetatable(-2);
  Pop();
}

void InstallStandardLibrary() {
  InstallGlobals();
  InstallStrings();
}

}  // namespace xxua
