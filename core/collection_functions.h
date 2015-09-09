#pragma once

#include <algorithm>
#include <sstream>

template <typename Collection, typename OutputIterator>
OutputIterator CopyCollection(Collection& collection,
                              OutputIterator output_iterator) {
  using std::begin;
  using std::end;
  return std::copy(begin(collection), end(collection), output_iterator);
}

template <typename Collection, typename Key = typename Collection::key_type,
          typename Value = typename Collection::mapped_type>
const Value* FindOrNull(const Collection& collection, const Key& key) {
  const auto it = collection.find(key);
  if (it != collection.end()) {
    return &it->second;
  } else {
    return nullptr;
  }
}

template <typename Collection, typename Key = typename Collection::key_type,
          typename Value = typename Collection::mapped_type>
const Value& FindOrThrow(const Collection& collection, const Key& key) {
  const auto it = collection.find(key);
  if (it != collection.end()) {
    return it->second;
  } else {
    std::ostringstream oss;
    oss << "key not found: " << key;
    throw std::logic_error(oss.str());
  }
}

template <typename Collection, typename Value = typename Collection::value_type>
bool Contains(const Collection& collection, const Value& value) {
  return std::find(std::begin(collection), std::end(collection), value) !=
         std::end(collection);
}

inline bool Contains(const string& subject, const string& object) {
  return subject.find(object) != string::npos;
}

inline bool Contains(const string& subject, const char* object) {
  return subject.find(object) != string::npos;
}

template <typename Collection, typename Key = typename Collection::key_type>
bool ContainsKey(const Collection& collection, const Key& key) {
  return collection.find(key) != collection.end();
}

template <typename Collection, typename UnaryPredicate>
bool AllOf(const Collection& collection, UnaryPredicate unary_predicate) {
  return all_of(std::begin(collection), std::end(collection), unary_predicate);
}

template <typename Collection, typename UnaryPredicate>
bool AnyOf(const Collection& collection, UnaryPredicate unary_predicate) {
  return any_of(std::begin(collection), std::end(collection), unary_predicate);
}

template <typename Collection, typename UnaryPredicate>
bool NoneOf(const Collection& collection, UnaryPredicate unary_predicate) {
  return none_of(std::begin(collection), std::end(collection), unary_predicate);
}

template <typename Collection, typename Value>
bool Insert(Collection& collection, Value&& value) {
  auto insert_result = collection.insert(std::forward<Value>(value));
  return insert_result.second;
}

template <typename Collection, typename Key = typename Collection::key_type,
          typename Value>
bool Insert(Collection& collection, const Key& key, Value&& value) {
  if (collection.find(key) == collection.end()) {
    collection[key] = std::forward<Value>(value);
    return true;
  } else {
    return false;
  }
}

template <typename Collection>
void Reverse(Collection& collection) {
  std::reverse(collection.begin(), collection.end());
}
