#include "xxua/value.h"

#include <iterator>

#include "core/must.h"
#include "xxua/api.h"
#include "xxua/context.h"

namespace xxua {

namespace {

bool IsRegistered(Type type) {
  switch (type) {
    case Type::NONE:
    case Type::NIL:
    case Type::BOOLEAN:
    case Type::INTEGER:
    case Type::FLOAT:
    case Type::LIGHT_USERDATA:
      return false;
    case Type::STRING:
    case Type::TABLE:
    case Type::LCLOSURE:
    case Type::CFUNCTION:
    case Type::CCLOSURE:
    case Type::USERDATA:
      return true;
    default:
      FAIL("unknown type");
  }
}

}  // namespace

Value::Value() : type_(Type::NIL) {}

Value::Value(bool b) : boolean_(b), type_(Type::BOOLEAN) {}

Value::Value(char i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(signed char i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(unsigned char i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(short i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(unsigned short i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(int i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(unsigned int i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(long i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(unsigned long i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(long long i) : integer_(i), type_(Type::INTEGER) {}
Value::Value(unsigned long long i) : integer_(i), type_(Type::INTEGER) {}

Value::Value(float f) : float_(f), type_(Type::FLOAT) {}
Value::Value(double f) : float_(f), type_(Type::FLOAT) {}

Value::Value(make_pointer_tag, void* p)
    : light_userdata_(p), type_(Type::LIGHT_USERDATA) {}

Value::Value(string_view sv) : type_(Type::STRING) {
  PushString(sv);
  PopRegister(Context::Current());
}

Value::Value(std::initializer_list<std::pair<Value, Value>> il)
    : type_(Type::TABLE) {
  State* registered = Context::Current();
  PushNewTable();
  for (auto& kv : il) {
    kv.first.PushSelf(registered);
    kv.second.PushSelf(registered);
    PopField(-3);
  }
  PopRegister(registered);
}

Value::Value(const Function& func) {
  PushFunction(Context::Current(), [func]() {
    State* registered = Context::Current();
    const int nargs = StackSize();
    Values args(nargs);
    for (int i = 0; i < nargs; ++i) args[i] = Value(registered, i + 1);
    Values results = func(args);
    const int nresults = results.size();
    for (const Value& result : results) result.PushSelf(registered);
    return nresults;
  });
  type_ = GetType(-1);
  PopRegister(Context::Current());
}

Value Compile(string_view code) {
  LoadFromString(code);
  Value result(Context::Current(), -1);
  Pop();
  return result;
}

Value Global() {
  PushGlobalTable();
  Value result(Context::Current(), -1);
  Pop();
  return result;
}

void Value::Assign(const Value& source) {
  if (IsRegistered(source.type_)) MUST(source.registered_ != nullptr);

  type_ = source.type_;
  switch (type_) {
    case Type::NONE:
    case Type::NIL:
      return;

    case Type::BOOLEAN:
      boolean_ = source.boolean_;
      break;

    case Type::INTEGER:
      integer_ = source.integer_;
      break;

    case Type::FLOAT:
      float_ = source.float_;
      break;

    case Type::LIGHT_USERDATA:
      light_userdata_ = source.light_userdata_;
      break;

    case Type::STRING:
    case Type::TABLE:
    case Type::LCLOSURE:
    case Type::CFUNCTION:
    case Type::CCLOSURE:
    case Type::USERDATA:
      if (IsRegistered(source.type_)) MUST(source.registered_ != nullptr);
      source.PushSelf();
      PopRegister(source.registered_);
      break;

    default:
      FAIL("unknown type");
  }
}

Value::Value(const Value& source) { Assign(source); }
Value::Value(Value&& source) {
  Assign(source);
  source.clear();
}

Value& Value::operator=(const Value& source) {
  if (&source != this) {
    clear();
    Assign(source);
  }
  return *this;
}

Value& Value::operator=(Value&& source) {
  if (IsRegistered(source.type_)) MUST(source.registered_ != nullptr);
  if (&source != this) {
    clear();
    Assign(source);
    source.clear();
  }
  return *this;
}

Value::Value(State* state, State::Index index) : type_(state->GetType(index)) {
  switch (type_) {
    case Type::NONE:
    case Type::NIL:
      return;

    case Type::BOOLEAN:
      boolean_ = state->ToBoolean(index);
      break;

    case Type::INTEGER:
      integer_ = state->ToInteger(index);
      break;

    case Type::FLOAT:
      float_ = state->ToFloat(index);
      break;

    case Type::LIGHT_USERDATA:
      light_userdata_ = state->ToUserdata(index);
      break;

    case Type::STRING:
    case Type::TABLE:
    case Type::LCLOSURE:
    case Type::CFUNCTION:
    case Type::CCLOSURE:
    case Type::USERDATA:
      state->PushCopy(index);
      PopRegister(state);
      break;

    default:
      FAIL("unknown type");
  }
}

Value::~Value() { clear(); }

bool Value::empty() const {
  switch (type_) {
    case Type::NONE:
    case Type::NIL:
      return true;

    case Type::BOOLEAN:
    case Type::INTEGER:
    case Type::FLOAT:
    case Type::LIGHT_USERDATA:
    case Type::STRING:
    case Type::TABLE:
    case Type::LCLOSURE:
    case Type::CFUNCTION:
    case Type::CCLOSURE:
    case Type::USERDATA:
      return false;

    default:
      FAIL("unknown type");
  }
}

Value::operator bool() const {
  MUST(type() == Type::BOOLEAN);
  return boolean_;
}

Value::operator char() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator signed char() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator unsigned char() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator short() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator unsigned short() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator int() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator unsigned int() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator long() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator unsigned long() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator long long() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator unsigned long long() const {
  MUST(type() == Type::INTEGER);
  return integer_;
}

Value::operator float() const {
  MUST(type() == Type::FLOAT);
  return float_;
}

Value::operator double() const {
  MUST(type() == Type::FLOAT);
  return float_;
}

Value::operator void*() const {
  MUST(type() == Type::LIGHT_USERDATA);
  return light_userdata_;
}

Value::operator string() const {
  MUST(type() == Type::STRING);
  PushSelf();
  MUST(GetType(-1) == Type::STRING);
  string result = ToString(-1).to_string();
  registered_->Pop();
  return result;
}

template <typename F>
bool CompareValues(F f, const Value& a, const Value& b) {
  State* registered = nullptr;
  if (IsRegistered(a.type_))
    registered = a.registered_;
  else if (IsRegistered(b.type_))
    registered = b.registered_;
  else
    registered = Context::Current();

  a.PushSelf(registered);
  b.PushSelf(registered);
  bool result = f(registered);
  Pop(2);
  return result;
}

template <typename F>
bool CompareValues2(F f, const Value& a, const Value& b) {
  return CompareValues(
      [&](State* registered) { return (registered->*f)(-2, -1); }, a, b);
}

template <typename F>
bool CompareValues3(F f, const Value& a, const Value& b) {
  return CompareValues([&](State* registered) {
    return (registered->*f)(-2, -1, false /*raw*/);
  }, a, b);
}

bool operator==(const Value& a, const Value& b) {
  return CompareValues3(&State::EQ, a, b);
}
bool operator!=(const Value& a, const Value& b) {
  return CompareValues3(&State::NE, a, b);
}
bool operator<(const Value& a, const Value& b) {
  return CompareValues2(&State::LT, a, b);
}
bool operator>(const Value& a, const Value& b) {
  return CompareValues2(&State::GT, a, b);
}
bool operator<=(const Value& a, const Value& b) {
  return CompareValues2(&State::LE, a, b);
}
bool operator>=(const Value& a, const Value& b) {
  return CompareValues2(&State::GE, a, b);
}

std::ostream& operator<<(std::ostream& os, const Value& v) {
  switch (v.type_) {
    case Type::NONE:
      os << "none";
      break;

    case Type::NIL:
      os << "null";
      break;

    case Type::BOOLEAN:
      if (bool(v)) {
        os << "true";
      } else {
        os << "false";
      }
      break;

    case Type::INTEGER:
      os << int64(v);
      break;

    case Type::FLOAT:
      os << float64(v) << "f";
      break;

    case Type::LIGHT_USERDATA:
      os << reinterpret_cast<uintptr_t>((void*)v) << "p";
      break;

    case Type::STRING:
      os << "\"" << string(v) << "\"";
      break;

    case Type::TABLE:
      os << "table";
      break;

    case Type::LCLOSURE:
      os << "lclosure";
      break;

    case Type::CFUNCTION:
      os << "cfunction";
      break;

    case Type::CCLOSURE:
      os << "cclosure";
      break;

    case Type::USERDATA:
      os << "userdata";
      break;

    default:
      FAIL("unknown type");
  }
  return os;
}

Value Value::operator[](Value key) const {
  MUST(type() == Type::TABLE);
  PushSelf();
  key.PushSelf(registered_);
  registered_->PushField(-2);
  Value result(registered_, -1);
  Pop(2);
  return result;
}

Value::Iterator Value::begin() const {
  MUST(type() == Type::TABLE);
  return {*this};
}

Value::Iterator Value::end() const { return {}; }

Values Value::ToSequence() const {
  Values result;
  for (int64 i = 0; true; ++i) {
    Value v = (*this)[i];
    if (v.empty()) break;
    result.push_back(v);
  }
  size_t t = 0;
  for (const auto& v : *this) {
    t++;
  }
  MUST_EQ(result.size(), t);
  return result;
}

Values Value::operator()(const Values& args) const {
  MUST(IsRegistered(type_));
  State* registered = registered_;
  int top = StackSize();
  PushSelf(registered);
  for (const Value& arg : args) arg.PushSelf(registered);

  CallMultret(args.size());

  const int nresults = StackSize() - top;
  Values results(nresults);
  for (int i = 0; i < nresults; i++)
    results[i] = Value(registered, -nresults + i);
  Pop(nresults);
  return results;
}

void Value::clear() {
  if (IsRegistered(type_)) {
    Deregister();
  }
  type_ = Type::NIL;
}

void Value::insert(Value key, Value val) {
  MUST(type_ == Type::TABLE);
  PushSelf();
  key.PushSelf(registered_);
  val.PushSelf(registered_);
  registered_->PopField(-3);
  Pop();
}

void Value::PopRegister(State* registered) {
  volatile Type popping_type = GetType(-1);
  //  LOG("PopRegister a ", TypeToString(popping_type));
  MUST(IsRegistered(popping_type));
  MUST(popping_type == type_);
  registered_ = registered;
  PushThisPointer();
  //  LOG(" registered = ", registered_);
  registered_->PushCopy(-2);
  registered_->PopField(State::REGISTRY);
  registered_->Pop();
}

void Value::PushThisPointer() const {
  //  LOG(reinterpret_cast<uintptr_t>(this), " at 3 this");
  //  LOG(reinterpret_cast<uintptr_t>(registered_), " at 3 reg");
  void* const this_code = const_cast<void*>(static_cast<const void*>(this));
  //  LOG("  pushing this = ", reinterpret_cast<uintptr_t>(this_code));
  MUST(registered_ != nullptr);
  registered_->PushLightUserdata(this_code);
}

void Value::PushSelf(State* expected_registered) const {
  if (IsRegistered(type_)) MUST(registered_ != nullptr);
  switch (type_) {
    case Type::NONE:
    case Type::NIL:
      PushNil();
      break;

    case Type::BOOLEAN:
      PushBoolean(boolean_);
      break;

    case Type::INTEGER:
      PushInteger(integer_);
      break;

    case Type::FLOAT:
      PushFloat(float_);
      break;

    case Type::LIGHT_USERDATA:
      PushLightUserdata(light_userdata_);
      break;

    case Type::STRING:
    case Type::TABLE:
    case Type::LCLOSURE:
    case Type::CFUNCTION:
    case Type::CCLOSURE:
    case Type::USERDATA: {
      MUST(registered_ != nullptr);
      if (expected_registered != nullptr)
        MUST(expected_registered == registered_,
             "values from two different states used together");
      //      LOG(reinterpret_cast<uintptr_t>(this), " at 2 this");
      //      LOG(reinterpret_cast<uintptr_t>(registered_), " at 2 reg");

      //      LOG("PushSelf of ", TypeToString(type_));
      PushThisPointer();
      registered_->PushField(State::REGISTRY);
      volatile Type seen_type = GetType(-1);
      //      LOG("  pushed_type = ", TypeToString(seen_type));
      MUST(seen_type == type_);
      break;
    }
    default:
      FAIL("unknown type");
  }
}

void Value::Deregister() {
  if (IsRegistered(type_)) MUST(registered_ != nullptr);
  //  LOG("Deregister of ", TypeToString(type_));
  PushThisPointer();
  registered_->PushNil();
  registered_->PopField(State::REGISTRY);
  registered_ = nullptr;
}

}  // namespace xxua
