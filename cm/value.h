#pragma once

#include <vector>

#include "xxua/api.h"
#include "xxua/context.h"
#include "xxua/state.h"
#include "xxua/type.h"

namespace xxua {

class Value;

using Values = std::vector<Value>;
using Function = std::function<Values(const Values& args)>;

template <typename T, typename... Args>
Value MakeObject(Args&&... args);

template <typename F>
Value MakeFunction(F&& f);

template <typename T>
Value MakePointer(T*);

Value Compile(string_view code);

Value Global();

template <typename T>
Value ObjectMetatable();

bool operator==(const Value& a, const Value& b);
bool operator!=(const Value& a, const Value& b);
bool operator<(const Value& a, const Value& b);
bool operator>(const Value& a, const Value& b);
bool operator<=(const Value& a, const Value& b);
bool operator>=(const Value& a, const Value& b);

std::ostream& operator<<(std::ostream& os, const Value& v);

class Value {
 public:
  // structors
  // ---------

  // null
  Value();

  // boolean
  Value(bool);

  // integer
  Value(char);
  Value(signed char);
  Value(unsigned char);
  Value(short);
  Value(unsigned short);
  Value(int);
  Value(unsigned int);
  Value(long);
  Value(unsigned long);
  Value(long long);
  Value(unsigned long long);

  // float
  Value(float);
  Value(double);

  // string
  Value(string_view);
  Value(const string& s) : Value(string_view(s)) {}
  Value(const char* s) : Value(string_view(s)) {}

  // table
  Value(std::initializer_list<std::pair<Value, Value>>);

  // value
  Value(const Value&);
  Value(Value&&);

  ~Value();

  // querying
  // --------
  Type type() const { return type_; }

  // null
  bool empty() const;

  // boolean
  explicit operator bool() const;

  // integer
  explicit operator char() const;
  explicit operator signed char() const;
  explicit operator unsigned char() const;
  explicit operator short() const;
  explicit operator unsigned short() const;
  explicit operator int() const;
  explicit operator unsigned int() const;
  explicit operator long() const;
  explicit operator unsigned long() const;
  explicit operator long long() const;
  explicit operator unsigned long long() const;

  // float
  explicit operator float32() const;
  explicit operator float64() const;

  // string
  explicit operator string() const;

  // light userdata
  explicit operator void*() const;

  // objects
  template <class T>
  bool is() const;

  template <class T>
  T& as() const;

  template <class T>
  T& ObjectCast(Value& operand);

  // table
  Value operator[](Value key) const;

  Values ToSequence() const;

  struct Iterator;
  Iterator begin() const;
  Iterator end() const;

  // function
  Values operator()(const Values& args) const;

  // mutation
  // --------
  Value& operator=(const Value&);
  Value& operator=(Value&&);

  void clear();

  void insert(Value key, Value val);

  // object
  template <typename T, typename... Args>
  void emplace(Args&&... args);

 private:
  // function
  Value(const Function&);

  // light userdata
  enum make_pointer_tag {};
  Value(make_pointer_tag, void*);

  Value(State* state, State::Index index);

  void Assign(const Value&);
  void PopRegister(State* registered);
  void PushThisPointer() const;
  void PushSelf(State* expected_registered = nullptr) const;
  void Deregister();

  union {
    bool boolean_;
    int64 integer_;
    float64 float_;
    void* light_userdata_;
    State* registered_;
  };
  Type type_;

  template <typename F>
  friend bool CompareValues(F f, const Value& a, const Value& b);
  friend std::ostream& operator<<(std::ostream& os, const Value& v);
  template <typename T>
  friend Value MakePointer(T* p);
  template <typename F>
  friend Value MakeFunction(F&& f);
  friend Value Compile(string_view code);
  friend Value Global();
  template <typename T>
  friend Value ObjectMetatable();
};

template <typename T, typename... Args>
Value MakeObject(Args&&... args) {
  Value v;
  v.emplace<T>(std::forward<Args>(args)...);
  return v;
}

template <typename F>
Value MakeFunction(F&& f) {
  return {Function(std::forward<F>(f))};
}

template <typename T>
Value MakePointer(T* p) {
  return {Value::make_pointer_tag(), (void*)p};
}

class Value::Iterator {
 public:
  Iterator() {}

  std::pair<Value, Value> operator*() const { return keyval_; }
  const std::pair<Value, Value>* operator->() const { return &keyval_; }
  Value::Iterator& operator++() {
    PushNext();
    return (*this);
  }

  Value::Iterator operator++(int) {
    Value::Iterator result(*this);
    operator++();
    return result;
  }

  friend bool operator==(const Iterator& a, const Iterator& b) {
    return a.table_ == b.table_ && a.keyval_ == b.keyval_;
  }

  friend bool operator!=(const Iterator& a, const Iterator& b) {
    return !(a == b);
  }

 private:
  Iterator(const Value& table) : table_(table) { PushNext(); }

  void PushNext() {
    State* registered = table_.registered_;
    table_.PushSelf(registered);
    keyval_.first.PushSelf(registered);
    if (registered->Next(-2)) {
      keyval_.first = Value(registered, -2);
      keyval_.second = Value(registered, -1);
      registered->Pop(3);
    } else {
      table_.clear();
      keyval_.first.clear();
      keyval_.second.clear();
      registered->Pop(1);
    }
  }

  Value table_;
  std::pair<Value, Value> keyval_;
  friend class Value;
};

template <class T>
bool Value::is() const {
  if (type_ != Type::USERDATA) return false;
  PushSelf();
  void* userdata = registered_->ToUserdata(-1);
  registered_->Pop();
  return (IsTypeId<T>(userdata));
}

template <class T>
T& Value::as() const {
  MUST(type_ == Type::USERDATA);
  PushSelf();
  T& result = ToObject<T>(registered_, -1);
  registered_->Pop();
  return result;
}

template <typename T, typename... Args>
void Value::emplace(Args&&... args) {
  clear();
  EmplaceObject<T>(Context::Current(), std::forward<Args>(args)...);
  type_ = Type::USERDATA;
  PopRegister(Context::Current());
}

template <typename T>
Value ObjectMetatable() {
  State* registered = Context::Current();
  PushObjectMetatable<T>(registered);
  Value result(registered, -1);
  Pop();
  return result;
}

}  // namespace xxua
