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

int NilType::TypeNum = LUA_TNIL;

void NilType::Push(lua_State* state) {
  lua_pushnil(state);
}

bool NilType::Is(lua_State* state, int index) {
  return lua_isnil(state, index);
}
///

int IntType::TypeNum = LUA_TNUMBER;

void IntType::Push(lua_State* state, lua_Integer i) {
  lua_pushinteger(state, i);
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

void NumberType::Push(lua_State* state, lua_Number n) {
  lua_pushnumber(state, n);
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

void StringType::Push(lua_State* state, const char* s) {
  lua_pushstring(state, s);
}

std::optional<const char*> StringType::MaybeGet(lua_State* state, int index) {
  if (!luaL_checkstring(state, index)) {
    return {};
  }
  return lua_tostring(state, index);
}

///

int TableType::TypeNum = LUA_TTABLE;

void TableType::Push(lua_State* state, size_t narr, size_t nrec) {
  assert(narr >= 0);
  assert(nrec >= 0);
  lua_createtable(state, static_cast<int>(narr), static_cast<int>(nrec));
}

}  // namespace lua