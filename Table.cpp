#include "Table.h"

namespace lua {

int TableType::TypeNum = LUA_TTABLE;

PushedValue TableType::Create(lua_State* s, int narr, int nrec) {
  lua_createtable(s, narr, nrec);
  return PushedValue(s, TypeNum);
}

Table::Table(lua_State* state, int index)
    : m_state(state), m_index(index) {}

Table::Table(const PushedValue &pv)
    : m_state(pv.state), m_index(pv.index) {}

Table::Table(ScopedValue&& v) : m_state(v.state), m_index(v.index), m_value(std::move(v)) {}

Table::~Table() {}

void Table::Set(const PushedValue& key, const PushedValue& value) {

  assert(key.num == 1);
  assert(key.index == lua_gettop(m_state) - 1);
  assert(value.num == 1);
  assert(value.index == lua_gettop(m_state));

  lua_settable(m_state, m_index);
}

void Table::Set(const PushedValue& value, const char* key) {
  assert(value.num == 1);
  assert(value.index == lua_gettop(m_state));

  lua_setfield(m_state, m_index, key);
}

PushedValue Table::Get(const PushedValue& key) {
  return PushedValue(m_state, lua_gettable(m_state, m_index));
}

PushedValue Table::Get(const char* key) {
  return PushedValue(m_state, lua_getfield(m_state, m_index, key));
}

PushedValue Table::PushSelf() const {
    return PushValue(m_state, m_index);
}


}  // namespace lua
