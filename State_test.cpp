#include "Lua_test.h"

using namespace lua;

class StateTest : public LuaTestBase {};

TEST_F(StateTest, TestRunWithBase) {
  m_state->OpenBase().DoString(
      u8R"(
        print("hello")
    )");
}