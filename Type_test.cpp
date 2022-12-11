#include "Lua_test.h"
#include "Type.h"

using namespace lua;

class TypeTest : public LuaTestBase, public ::testing::Test {};

TEST_F(TypeTest, TestInt) {
  m_state.PushInteger(99);
  auto i = m_state.MaybeGetInteger(-1);
  ASSERT_TRUE(i);
  ASSERT_EQ(*i, lua_Integer(99));
  ASSERT_EQ(TypeName(m_state, -1), "number");
}

TEST_F(TypeTest, TestFloat) {
  m_state.PushNumber(99.9);
  auto d = m_state.MaybeGetNumber(-1);
  ASSERT_TRUE(d);
  ASSERT_NEAR(*d, lua_Number(99.9), 0.001);
  ASSERT_EQ(TypeName(m_state, -1), "number");
}

TEST_F(TypeTest, TestString) {
  m_state.PushString("hello");
  auto s = m_state.MaybeGetString(-1);
  ASSERT_TRUE(s);
  ASSERT_EQ(std::string(s), "hello");
  ASSERT_EQ(TypeName(m_state, -1), "string");
}
