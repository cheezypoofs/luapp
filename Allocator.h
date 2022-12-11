#pragma once

#include <cstddef>

namespace lua {

class Allocator {
public:
  virtual ~Allocator() = default;

  virtual void *Alloc(size_t) noexcept = 0;
  virtual void *Realloc(void *, size_t) noexcept = 0;
  virtual void Free(void *, size_t) noexcept = 0;
};

// StdAllocator is an instance of Allocator that uses the std
// C malloc/realloc/free set of functions.
class StdAllocator : public Allocator {
public:
  void *Alloc(size_t) noexcept override;
  void *Realloc(void *, size_t) noexcept override;
  void Free(void *, size_t) noexcept override;
};

} // namespace lua