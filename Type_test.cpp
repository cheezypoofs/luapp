#include "Type.h"
#include "Lua_test.h"

using namespace lua;

class TypeTest : public LuaTestBase {};

TEST_F(TypeTest, TestNil) {
  NilType::Push(*m_state);
  ASSERT_TRUE(NilType::Is(*m_state, -1));
  ASSERT_EQ(TypeName(*m_state, -1), "nil");
}

TEST_F(TypeTest, TestInt) {
  IntType::Push(*m_state, 99);
  ASSERT_EQ(GetType<IntType>(*m_state, -1), lua_Integer(99));
  ASSERT_EQ(TypeName(*m_state, -1), "number");
}

TEST_F(TypeTest, TestFloat) {
  NumberType::Push(*m_state, 99.9);
  ASSERT_NEAR(GetType<NumberType>(*m_state, -1), lua_Number(99.9), 0.001);
  ASSERT_EQ(TypeName(*m_state, -1), "number");
}

TEST_F(TypeTest, TestString) {
  StringType::Push(*m_state, "hello");
  ASSERT_EQ(std::string(GetType<StringType>(*m_state, -1)), "hello");
  ASSERT_EQ(TypeName(*m_state, -1), "string");
}
