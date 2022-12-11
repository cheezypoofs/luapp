#include "Allocator.h"

#include <stdlib.h>

namespace lua {

void *StdAllocator::Alloc(size_t n) noexcept { return ::malloc(n); }

void *StdAllocator::Realloc(void *p, size_t n) noexcept {
  return ::realloc(p, n);
}

void StdAllocator::Free(void *p, size_t) noexcept { ::free(p); }
} // namespace lua