#pragma once

#include <cassert>
#include <functional>
#include "Exception.h"
#include "State.h"
#include "Type.h"
#include "luapp.h"

namespace lua {

namespace {

template <typename A>
struct ArgGuesser {
  typedef struct CannotGuessType Type;
};

template <>
struct ArgGuesser<const char*> {
  typedef StringType Type;
};

template <>
struct ArgGuesser<void> {
  typedef NoneType Type;
};

template <>
struct ArgGuesser<lua_Integer> {
  typedef IntType Type;
};

template <>
struct ArgGuesser<lua_Number> {
  typedef NumberType Type;
};

template <typename ReturnType, typename DataType>
struct MethodCall0 {

  typedef typename ReturnType::Type (DataType::*MethodType)() noexcept;

  static typename ReturnType::Type Call(DataType& s, MethodType f,
                                        lua_State* L) noexcept {
    return (s.*f)();
  }
};

template <typename ReturnType, typename DataType, typename Arg0>
struct MethodCall1 {

  typedef typename ReturnType::Type (DataType::*MethodType)(Arg0) noexcept;

  static typename ReturnType::Type Call(DataType& s, MethodType f,
                                        lua_State* L) noexcept {
    auto arg0 = GetType<typename ArgGuesser<Arg0>::Type>(L, 2);

    return (s.*f)(arg0);
  }
};

template <typename ReturnType, typename DataType, typename Arg0, typename Arg1>
struct MethodCall2 {

  typedef
      typename ReturnType::Type (DataType::*MethodType)(Arg0, Arg1) noexcept;

  static typename ReturnType::Type Call(DataType& s, MethodType f,
                                        lua_State* L) noexcept {
    auto arg0 = GetType<typename ArgGuesser<Arg0>::Type>(L, 2);
    auto arg1 = GetType<typename ArgGuesser<Arg1>::Type>(L, 3);

    return (s.*f)(arg0, arg1);
  }
};
}  // namespace

// Index represents an attribute of your custom UserData type.
// Use the Getter/Setter/Method builders to initialize the instance
// in your UserData::IndexMap.
struct Index {
  std::function<int(const void*, lua_State*)> getter;
  std::function<int(void*, lua_State*)> setter;
  std::function<int(lua_State*)> fn;

  // Getter makes this Index a property that is readable.
  // ReturnType is one of the Type helpers that describes type handling
  // (like StringType).
  // It assumes your class method is something like:
  //  const char* GetFoo() const noexcept { ... }
  // to expose something like:
  //  local f = x.foo
  template <typename ReturnType, typename DataType>
  Index& Getter(typename ReturnType::Type (DataType::*f)() const noexcept) {
    assert("getter already set" && !getter);
    assert("cannot mix method with properties" && !fn);
    getter = [f](const void* self, lua_State* L) -> int {
      const auto& s = *static_cast<const DataType*>(self);
      auto result = (s.*f)();
      return ReturnType::Push(L, result);
    };
    return *this;
  }

  // Setter is the complement to Getter. It assumes your method is something like
  //  void SetFoo(const char*) noexcept { ... }
  // to expose something like:
  //  `x.foo = "boo"`
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

  // Method* makes this Index a callable method bould to the instance.
  // The callback will have all arguments in the call with the exception that
  // you must ignored the value at index-1 as it will not be an instance of self
  // (you don't need it, we already called you as an instance). All other
  // arguments are as expected where 0 is not valid, 1 you ignore, and 2... are
  // the arguments.
  //
  // note: This assumes your method is a form like:
  //  const char* DoSomething(lua_State*) noexcept { ... }
  //
  // use NoneType as ReturnType for void

  template <typename ReturnType, typename DataType>
  Index& Method0(typename ReturnType::Type (DataType::*f)() noexcept) {
    return MethodX<MethodCall0<ReturnType, DataType>, ReturnType, DataType>(f);
  }

  template <typename ReturnType, typename DataType, typename Arg0>
  Index& Method1(typename ReturnType::Type (DataType::*f)(Arg0) noexcept) {
    return MethodX<MethodCall1<ReturnType, DataType, Arg0>, ReturnType,
                   DataType>(f);
  }

 private:
  // MethodX is used by the Method* calls.
  template <typename MethodCaller, typename ReturnType, typename DataType,
            typename MethodType>
  Index& MethodX(MethodType f) {

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

          auto& s = *static_cast<DataType*>(self);
          auto result = MethodCaller::Call(s, f, L);
          return ReturnType::Push(L, result);
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

    auto i = FindKey(key);
    if (i && i->setter) {
      return i->setter(this, L);
    }

    return 0;
  }

  int MetaIndex(lua_State* L) const noexcept {
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

 protected:
  // Indexes (sic: It sounds better than indices) is an optional override
  // where you can declare the available Index instances used by
  // __index and __newindex (MetaIndex and MetaNewIndex).
  // You should use a statically initialized instance and return its address
  // if your type is meant to expose properties and methods.
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

namespace {
template <typename DataType, typename NewCaller>
void RegisterUserDataX(State& state) {
  static_assert(std::is_base_of<UserData, DataType>::value);

  lua_register(state, DataType::Name, [](lua_State* L) -> int {
    auto dt = NewCaller::Call(L);
    assert(dt);
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
}  // namespace

template <typename DataType>
struct NewCall0 {
  static DataType* Call(lua_State* L) noexcept {
    return new (lua_newuserdatauv(L, sizeof(DataType), 0)) DataType();
  }
};

template <typename DataType, typename Arg0>
struct NewCall1 {
  static DataType* Call(lua_State* L) noexcept {
    auto arg0 = GetType<typename ArgGuesser<Arg0>::Type>(L, 1);
    return new (lua_newuserdatauv(L, sizeof(DataType), 0)) DataType(arg0);
  }
};

template <typename DataType, typename Arg0, typename Arg1>
struct NewCall2 {
  static DataType* Call(lua_State* L) noexcept {
    auto arg0 = GetType<typename ArgGuesser<Arg0>::Type>(L, 1);
    auto arg1 = GetType<typename ArgGuesser<Arg1>::Type>(L, 2);
    return new (lua_newuserdatauv(L, sizeof(DataType), 0)) DataType(arg0, arg1);
  }
};

// RegisterUserData* calls take care of registering your type with lua
// so that it can be instantiated, indexed, etc. Your type must be derived
// from UserData. The different variants allow for constructors with arguments.
template <typename DataType>
void RegisterUserData0(State& state) {
  return RegisterUserDataX<DataType, NewCall0<DataType>>(state);
}

template <typename DataType, typename Arg0>
void RegisterUserData1(State& state) {
  return RegisterUserDataX<DataType, NewCall1<DataType, Arg0>>(state);
}

template <typename DataType, typename Arg0, typename Arg1>
void RegisterUserData2(State& state) {
  return RegisterUserDataX<DataType, NewCall2<DataType, Arg0, Arg1>>(state);
}

}  // namespace lua
