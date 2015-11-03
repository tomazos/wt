#pragma once

#include <map>

#include "cm/allocator.h"

namespace cm {

class DebugAllocator : public Allocator {
 public:
  ~DebugAllocator();

  void* Create(BlockPurpose block_purpose, size_t size) override;

  void* Resize(void* old_block, size_t old_size, size_t new_size) override;

  void Destroy(void* block, size_t old_size) override;

 private:
  struct BlockInfo {
    BlockPurpose purpose;
    size_t size;
    char* data;
    void* ptr() const { return static_cast<void*>(data); }
  };

  void Add(const BlockInfo& block_info);

  void Remove(const BlockInfo& block_info);

  Mutex mutex_;
  std::map<void*, BlockInfo> blocks;
};

}  // namespace cm
