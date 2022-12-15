#include "Value.h"
#include <cassert>
#include "Type.h"

namespace lua {
PushedValue::PushedValue(lua_State* s) : PushedValue(s, lua_type(s, -1)) {}

PushedValue::PushedValue(lua_State* s, int t)
    : state(s),
      type(t),
      num(type >= LUA_TNIL && type < LUA_NUMTYPES ? 1 : 0),
      index(num ? lua_gettop(state) : 0) {}

///

ScopedValue::ScopedValue(const PushedValue& pv)
    : state(pv.state), num(pv.num), type(pv.type), index(pv.index) {
  // otherwise, the destructor pop will be wrong.
  assert("ScopedValue should use PushedValue from top of stack" &&
         index == lua_gettop(state));
}

ScopedValue::~ScopedValue() {
  if (index) {
    assert("stack in incorrect state" && lua_gettop(state) == index);
    luaL_checktype(state, -1, type);
    lua_pop(state, 1);
  }
}

PushedValue ScopedValue::PushSelf() const {
  return PushValue(state, index);
}

}  // namespace lua
