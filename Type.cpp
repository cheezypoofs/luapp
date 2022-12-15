#include "Type.h"
#include "State.h"
#include "luapp.h"

namespace lua {

std::string TypeName(lua_State* state, int index) {

  std::string name;

  auto n = lua_typename(state, lua_type(state, index));
  assert(n);
  name = n;

  return name;
}

std::string TypeName(int luaNum) {
  return lua_typename(nullptr, luaNum);
}

///

int NoneType::TypeNum = LUA_TNONE;

PushedValue NoneType::Push(lua_State*, const Void&) {
  return {0, TypeNum};
}

bool NoneType::Is(lua_State* state, int index) {
  return lua_isnone(state, index);
}

///

int NilType::TypeNum = LUA_TNIL;

PushedValue NilType::Push(lua_State* state) {
  lua_pushnil(state);
  return PushedValue(state, TypeNum);
}

bool NilType::Is(lua_State* state, int index) {
  return lua_isnil(state, index);
}
///

int IntType::TypeNum = LUA_TNUMBER;

PushedValue IntType::Push(lua_State* state, lua_Integer i) {
  lua_pushinteger(state, i);
  return PushedValue(state, TypeNum);
}

std::optional<lua_Integer> IntType::MaybeGet(lua_State* state, int index) {
  int isnum = 0;
  lua_Integer i = lua_tointegerx(state, index, &isnum);
  if (!isnum) {
    return {};
  }
  return i;
}

///

int NumberType::TypeNum = LUA_TNUMBER;

PushedValue NumberType::Push(lua_State* state, lua_Number n) {
  lua_pushnumber(state, n);
  return PushedValue(state, TypeNum);
}

std::optional<lua_Number> NumberType::MaybeGet(lua_State* state, int index) {
  int isnum = 0;
  lua_Number n = lua_tonumberx(state, index, &isnum);
  if (!isnum) {
    return {};
  }
  return n;
}

///

int StringType::TypeNum = LUA_TSTRING;

PushedValue StringType::Push(lua_State* state, const char* s) {
  lua_pushstring(state, s);
  return PushedValue(state, TypeNum);
}

std::optional<const char*> StringType::MaybeGet(lua_State* state, int index) {
  if (!luaL_checkstring(state, index)) {
    return {};
  }
  return lua_tostring(state, index);
}

}  // namespace lua