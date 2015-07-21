#pragma once

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
