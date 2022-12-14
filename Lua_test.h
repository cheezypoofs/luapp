#include <gtest/gtest.h>
#include <iostream>
#include "Allocator.h"
#include "State.h"

namespace lua {
class LuaTestBase : public ::testing::Test {
 public:
  LuaTestBase()
      : m_alloc(std::make_shared<StdAllocator>()),
        m_memoryCounts(std::make_shared<CountingAllocator>(*m_alloc)),
        m_state(std::make_unique<State>(m_memoryCounts)) {}
  virtual ~LuaTestBase() = default;

  void TearDown() override {
    if (false) {
      std::cerr << "Memory stats..." << std::endl
                << "  Num Allocs    : " << m_memoryCounts->GetNumAllocs()
                << std::endl
                << "  Num Reallocs  : " << m_memoryCounts->GetNumReallocs()
                << std::endl
                << "  Num Frees     : " << m_memoryCounts->GetNumFrees()
                << std::endl
                << "  Live Pointers : "
                << m_memoryCounts->GetTotalLivePointers() << std::endl
                << "  Live Memory   : " << m_memoryCounts->GetTotalLiveMemory()
                << std::endl;
    }

    // Destroy state. Let's make sure memory is totally cleaned up.
    m_state.reset();

    ASSERT_EQ(0u, m_memoryCounts->GetTotalLiveMemory())
        << "Live memory remains (leak?)";
    ASSERT_EQ(0u, m_memoryCounts->GetTotalLivePointers())
        << "Live pointers remain (leak?)";

    ::testing::Test::TearDown();
  }

 protected:
  std::shared_ptr<StdAllocator> m_alloc;
  std::shared_ptr<CountingAllocator> m_memoryCounts;
  std::unique_ptr<State> m_state;
};

}  // namespace lua