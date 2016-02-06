#pragma once

#include "Text.h"

struct SemObject;
// 1. Current dynamically allocated classes derived from SemObject are
// registered with a memory pool.
// 2. When SemObject::delete_all() is called, they are all deleted

struct PooledBase {
  struct AllocationInfo {
    void* start = nullptr;
    size_t length = 0;
    PooledBase* ptr = nullptr;
  };

  static map<void*, AllocationInfo, greater<void*>> allocations;

  static void* operator new(size_t size) {
    void* p = ::operator new(size);

    AllocationInfo& allocation = allocations[p];

    allocation.start = p;
    allocation.length = size;

    return p;
  }

  static void operator delete(void* p, size_t) {
    auto it = allocations.find(p);

    if (it == allocations.end())
      throw logic_error("internal error, delete of non-existent allocation");

    allocations.erase(it);

    ::operator delete(p);
  }

  PooledBase() {
    auto it = allocations.lower_bound(this);

    if (it == allocations.end()) return;

    AllocationInfo& allocation = it->second;

    const char* p = (const char*)this;
    const char* q = (const char*)allocation.start;
    size_t n = allocation.length;

    if (p < q + n && !allocation.ptr) allocation.ptr = this;
  }

  virtual ~PooledBase() {}
};

struct Pooled : virtual public PooledBase {
  static void delete_all() {
    while (!allocations.empty()) {
      PooledBase* ptr = allocations.begin()->second.ptr;

      if (!ptr)
        throw logic_error("internal error, SemObjectBase didn't register");

      delete ptr;
    }
  }

  virtual void write(ostream& o) const { o << "unknown@" << (void*)this; }
};

inline ostream& operator<<(ostream& o, const Pooled& p) {
  p.write(o);
  return o;
}
inline ostream& operator<<(ostream& o, const Pooled* p) {
  if (p)
    p->write(o);
  else
    o << "null";
  return o;
}
