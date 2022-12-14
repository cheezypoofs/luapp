#include "UserData.h"

namespace lua {

int UserData::MetaIndex(lua_State* L) const noexcept {
  const char* key = luaL_checkstring(L, 2);

  auto i = FindKey(key);
  if (!i) {
    return 0;
  }
  if (i->getter) {
    return i->getter(this, L);
  }
  if (i->fn) {
    return i->fn(L);
  }

  return 0;
}

int UserData::MetaNewIndex(lua_State* L) noexcept {
  const char* key = luaL_checkstring(L, 2);

  auto i = FindKey(key);
  if (i && i->setter) {
    return i->setter(this, L);
  }

  return 0;
}

const Index* UserData::FindKey(const char* key) const noexcept {
  auto indexes = Indexes();
  if (!indexes) {
    return nullptr;
  }
  auto it = indexes->find(key);
  if (it == indexes->end()) {
    return nullptr;
  }
  return &it->second;
}

}  // namespace lua