#pragma once

#include <cassert>
#include <functional>
#include "Exception.h"
#include "State.h"
#include "Type.h"
#include "luapp.h"

namespace lua {

struct Index {
  std::function<int(const void*, lua_State*)> getter;
  std::function<int(void*, lua_State*)> setter;
  std::function<int(lua_State*)> fn;

  template <typename ReturnType, typename DataType>
  Index& Getter(typename ReturnType::Type (DataType::*f)() const noexcept) {
    assert("getter already set" && !getter);
    assert("cannot mix method with properties" && !fn);
    getter = [f](const void* self, lua_State* L) -> int {
      const auto& s = *static_cast<const DataType*>(self);
      auto result = (s.*f)();
      ReturnType::Push(L, result);
      return 1;
    };
    return *this;
  }

  template <typename SetType, typename DataType>
  Index& Setter(void (DataType::*f)(typename SetType::Type) noexcept) {
    assert("setter already set" && !setter);
    assert("cannot mix method with properties" && !fn);
    setter = [f](void* self, lua_State* L) -> int {
      auto& s = *static_cast<DataType*>(self);
      (s.*f)(GetType<SetType>(L, 2));
      return 0;
    };
    return *this;
  }

  // Method makes this Index a callable method bould to the instance.
  // The callback will have all arguments in the call with the exception that
  // you must ignored the value at index-1 as it will not be an instance of self
  // (you don't need it, we already called you as an instance). All other
  // arguments are as expected where 0 is not valid, 1 you ignore, and 2... are
  // the arguments.
  template <typename ReturnType, typename DataType>
  Index& Method(typename ReturnType::Type (DataType::*f)(lua_State*)
                    const noexcept) {
    typedef typename ReturnType::Type (DataType::*MethodType)(lua_State*)
        const noexcept;

    assert("cannot mix method with properties" && !getter && !setter);
    assert("method already set" && !fn);

    // `fn` is invoked on the __index of the method. We return a callable each
    // time it is invoked. This callable is not a function, rather it is a
    // generic table that contains:
    //  - A reference to `self` as a value (to keep a ref count on it)
    //  - A copy of `f` (which is just a value...a pointer to a method that will
    //  exist in the instance
    //      of `self`)
    //  - A single-entry meta table with __call implemented to make it callable.

    fn = [f](lua_State* L) -> int {
      // table as a functor
      lua_createtable(L, 0, 3);

      // t["f"] = f
      *static_cast<MethodType*>(lua_newuserdatauv(L, sizeof(MethodType), 0)) =
          f;
      lua_setfield(L, -2, "f");

      // t["self"] = self
      // this ensures a ref-counted binding to self
      lua_pushvalue(L, 1);  // todo: rework so this is more of a sure thing
      lua_setfield(L, -2, "self");

      // metatable
      {
        lua_createtable(L, 0, 1);

        // metatable["__call"] = <call impl>
        lua_pushcfunction(L, [](lua_State* L) -> int {
          /////
          // fetch `f`
          /////
          auto typ = lua_getfield(L, 1, "f");
          assert(typ == LUA_TUSERDATA);

          auto f =
              *(static_cast<MethodType*>(AssertNotNull(lua_touserdata(L, -1))));
          lua_pop(L, 1);

          /////
          // fetch `self`
          /////
          typ = lua_getfield(L, 1, "self");
          assert(typ == LUA_TUSERDATA);

          auto self =
              static_cast<DataType*>(luaL_checkudata(L, -1, DataType::Name));
          lua_pop(L, 1);

          // Now, call the "method"
          const auto& s = *static_cast<const DataType*>(self);
          auto result = (s.*f)(L);

          ReturnType::Push(L, result);
          return 1;
        });
        lua_setfield(L, -2, "__call");

        lua_setmetatable(L, -2);
      }

      // return the table (functor).
      return 1;
    };
    return *this;
  }
};

class UserData {
 public:
  virtual ~UserData() = default;

  typedef LUAPP_SMALL_MAP<std::string, Index> IndexMap;

  int MetaNewIndex(lua_State* L) noexcept {
    const char* key = luaL_checkstring(L, 2);
    std::cout << "NewIndex(" << key << ")" << std::endl;

    auto i = FindKey(key);
    if (i && i->setter) {
      return i->setter(this, L);
    }

    return 0;
  }

  int MetaIndex(lua_State* L) const noexcept {
    const char* key = luaL_checkstring(L, 2);
    std::cout << "Index(" << key << ")" << std::endl;

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

 protected:
  virtual const IndexMap* Indexes() const noexcept { return nullptr; }

 private:
  const Index* FindKey(const char* key) const noexcept {
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
};

template <typename DataType>
void RegisterUserData(State& state) {
  lua_register(state, DataType::Name, [](lua_State* L) -> int {
    DataType* dt = new (lua_newuserdatauv(L, sizeof(DataType), 0)) DataType(L);
    luaL_setmetatable(L, DataType::Name);
    return 1;
  });
  luaL_newmetatable(state, DataType::Name);

  // add finalizer `__gc` to meta table.
  lua_pushcfunction(state, [](lua_State* L) -> int {
    DataType* dt = static_cast<DataType*>(lua_touserdata(L, 1));
    dt->~DataType();
    return 0;
  });
  lua_setfield(state, -2, "__gc");

  // add __index to meta table
  lua_pushcfunction(state, [](lua_State* L) -> int {
    DataType* dt = static_cast<DataType*>(lua_touserdata(L, 1));
    return dt->MetaIndex(L);
  });
  lua_setfield(state, -2, "__index");

  // add __newindex to meta table
  lua_pushcfunction(state, [](lua_State* L) -> int {
    DataType* dt = static_cast<DataType*>(lua_touserdata(L, 1));
    return dt->MetaNewIndex(L);
  });
  lua_setfield(state, -2, "__newindex");

  // done. pop meta table.
  lua_pop(state, 1);
}
}  // namespace lua
