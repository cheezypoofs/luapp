#include "Allocator.h"

// #include <iostream>
#include <stdlib.h>

namespace lua {

void *StdAllocator::Alloc(size_t n) noexcept {
  auto p = ::malloc(n);
  //    std::cout << "malloc(" << n << ") = " << p << std::endl;
  return p;
}

void *StdAllocator::Realloc(void *p, size_t n) noexcept {
  auto newP = ::realloc(p, n);
  //    std::cout << "realloc(" << p << "," << n << ") = " << newP << std::endl;
  return newP;
}

void StdAllocator::Free(void *p, size_t) noexcept {
  //    std::cout << "free(" << p << ")" << std::endl;
  ::free(p);
}
} // namespace lua