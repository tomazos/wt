#pragma once

namespace xxua {

class Allocator {
 public:
  enum BlockPurpose { STRING, TABLE, FUNCTION, USERDATA, THREAD, OTHER };

  virtual void* Create(BlockPurpose block_purpose, size_t size) = 0;
  virtual void* Resize(void* old_block, size_t old_size, size_t new_size) = 0;
  virtual void Destroy(void* block, size_t old_size) = 0;

  virtual ~Allocator() = default;
};

}  // namespace xxua
