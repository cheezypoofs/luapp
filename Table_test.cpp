#include "Table.h"
#include "Lua_test.h"

using namespace lua;

class TableTest : public LuaTestBase {};

TEST_F(TableTest, TestGetMissing) {
  Table t1(ScopedValue(TableType::Create(*m_state, 0, 0)));
  Table t2(ScopedValue(TableType::Create(*m_state, 0, 0)));
  Table t3(ScopedValue(TableType::Create(*m_state, 0, 0)));

  ScopedValue nothing(t2.Get("nothing"));
  EXPECT_EQ(nothing.type, NilType::TypeNum);

  t2.Set(IntType::Push(*m_state, 99), "99");
  ScopedValue nn(t2.Get("99"));
  EXPECT_EQ(nn.type, IntType::TypeNum);
  EXPECT_EQ(GetValue<IntType>(nn), lua_Integer(99));

  t2.Set(NilType::Push(*m_state), "99");
  EXPECT_EQ(ScopedValue(t2.Get("99")).type, NilType::TypeNum);

  t3.Set(StringType::Push(*m_state, "key"),
         StringType::Push(*m_state, "value"));

  auto value = ScopedValue(t3.Get("key"));
  EXPECT_EQ(value.type, StringType::TypeNum);
  EXPECT_EQ(GetValue<StringType>(value), std::string("value"));
}
