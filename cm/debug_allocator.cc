#include "xxua/debug_allocator.h"

#include <cstring>

#include "core/collection_functions.h"
#include "core/must.h"

namespace xxua {

DebugAllocator::~DebugAllocator() { MUST(blocks.empty()); }

void* DebugAllocator::Create(BlockPurpose block_purpose, size_t size) {
  LockGuard l(mutex_);
  BlockInfo block_info{block_purpose, size, new char[size]};
  Add(block_info);
  return block_info.ptr();
}

void* DebugAllocator::Resize(void* old_block, size_t old_size,
                             size_t new_size) {
  LockGuard l(mutex_);
  const BlockInfo* old_block_info = FindOrNull(blocks, old_block);
  MUST(old_block_info);
  MUST_EQ(old_block_info->size, old_size);
  BlockInfo new_block_info{old_block_info->purpose, new_size,
                           new char[new_size]};
  std::memcpy(new_block_info.data, old_block_info->data,
              std::min(old_size, new_size));
  Add(new_block_info);
  Remove(*old_block_info);
  return new_block_info.ptr();
}

void DebugAllocator::Destroy(void* block, size_t old_size) {
  LockGuard l(mutex_);
  const BlockInfo* block_info = FindOrNull(blocks, block);
  MUST(block_info);
  MUST_EQ(block_info->size, old_size);
  Remove(*block_info);
}

void DebugAllocator::Add(const BlockInfo& block_info) {
  MUST(Insert(blocks, block_info.ptr(), block_info));
}

void DebugAllocator::Remove(const BlockInfo& block_info) {
  delete[] block_info.data;
  blocks.erase(block_info.ptr());
}

}  // namespace xxua
