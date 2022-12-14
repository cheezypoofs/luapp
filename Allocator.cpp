#include "Allocator.h"

#include <stdlib.h>
#include <cassert>

namespace lua {

void* StdAllocator::Alloc(size_t n) noexcept {
  assert(n);
  auto p = ::malloc(n);
  //    std::cout << "malloc(" << n << ") = " << p << std::endl;
  return p;
}

void* StdAllocator::Realloc(void* p, size_t o, size_t n) noexcept {
  assert(n);
  if (o >= n) {
    return p;
  }
  auto newP = ::realloc(p, n);
  //    std::cout << "realloc(" << p << "," << n << ") = " << newP << std::endl;
  return newP;
}

void StdAllocator::Free(void* p, size_t) noexcept {
  //    std::cout << "free(" << p << ")" << std::endl;
  ::free(p);
}

///

CountingAllocator::CountingAllocator(Allocator& next) : m_next(next) {}

void* CountingAllocator::Alloc(size_t n) noexcept {
  m_numAllocs++;
  auto p = m_next.Alloc(n);
  if (p) {
    m_totalLivePointers++;
  }
  m_totalLiveMemory += n;
  return m_next.Alloc(n);
}

void* CountingAllocator::Realloc(void* p, size_t o, size_t n) noexcept {
  m_numReallocs++;

  int64_t diff = static_cast<int64_t>(n) - static_cast<int64_t>(o);
  if (diff < 0) {
    assert(-diff <= static_cast<int64_t>(m_totalLiveMemory));
  }
  m_totalLiveMemory -= o;
  m_totalLiveMemory += n;
  return m_next.Realloc(p, o, n);
}

void CountingAllocator::Free(void* p, size_t o) noexcept {
  m_numFrees++;
  assert(m_totalLivePointers);
  assert(o <= m_totalLiveMemory);
  m_totalLiveMemory -= o;
  m_totalLivePointers--;
  return m_next.Free(p, o);
}

}  // namespace lua