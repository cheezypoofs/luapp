#pragma once

#include <cstddef>
#include <cstdint>

namespace lua {

// Allocator is an interface for intercepting memory manipulations used by lua.
class Allocator {
 public:
  virtual ~Allocator() = default;

  virtual void* Alloc(size_t o) noexcept = 0;
  virtual void* Realloc(void*, size_t o, size_t n) noexcept = 0;
  virtual void Free(void*, size_t o) noexcept = 0;
};

// StdAllocator is an instance of Allocator that uses the std
// C malloc/realloc/free set of functions.
class StdAllocator : public Allocator {
 public:
  void* Alloc(size_t) noexcept override;
  void* Realloc(void*, size_t, size_t) noexcept override;
  void Free(void*, size_t) noexcept override;
};

// CountingAllocator is a thread-hostile Allocator that
// tracks memory statistics. It is meant to be used
// per State instance where thread safety doesn't matter.
class CountingAllocator : public Allocator {
 public:
  explicit CountingAllocator(Allocator& next);

  void* Alloc(size_t) noexcept override;
  void* Realloc(void*, size_t, size_t) noexcept override;
  void Free(void*, size_t) noexcept override;

  uint64_t GetNumAllocs() const { return m_numAllocs; }
  uint64_t GetNumReallocs() const { return m_numReallocs; }
  uint64_t GetNumFrees() const { return m_numFrees; }
  uint64_t GetTotalLivePointers() const { return m_totalLivePointers; }
  uint64_t GetTotalLiveMemory() const { return m_totalLiveMemory; }

 private:
  Allocator& m_next;
  uint64_t m_numAllocs = 0;
  uint64_t m_numReallocs = 0;
  uint64_t m_numFrees = 0;
  uint64_t m_totalLivePointers = 0;
  uint64_t m_totalLiveMemory = 0;
};

}  // namespace lua