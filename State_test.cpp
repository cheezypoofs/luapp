#include "Lua_test.h"

using namespace lua;

class StateTest : public LuaTestBase, public ::testing::Test {};

TEST_F(StateTest, TestRunWithBase) {
  m_state.OpenBase().DoString(u8R"(
        print("hello")
    )");
}