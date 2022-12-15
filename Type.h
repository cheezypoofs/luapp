#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "Value.h"
#include "luapp.h"

// todo: I want to find a way to let someone override stl data
// types with higher performing 3rd party ones.
#ifndef LUAPP_SMALL_MAP
#define LUAPP_SMALL_MAP std::unordered_map
#endif

namespace lua {
std::string TypeName(int luaTypeNum);
std::string TypeName(lua_State*, int index);

struct Void {};
struct NoneType final {
  typedef Void Type;
  static int TypeNum;
  static PushedValue Push(lua_State*, const Void& = Void());
  static bool Is(lua_State*, int index);
};

struct NilType final {
  static int TypeNum;
  static PushedValue Push(lua_State*);
  static bool Is(lua_State*, int index);
};

struct IntType final {
  typedef lua_Integer Type;
  static int TypeNum;
  static PushedValue Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

struct NumberType final {
  typedef lua_Number Type;
  static int TypeNum;
  static PushedValue Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

struct StringType final {
  typedef const char* Type;
  static int TypeNum;
  static PushedValue Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

inline PushedValue PushValue(lua_State* state, int index) {
  lua_pushvalue(state, index);
  return PushedValue(state, lua_type(state, -1));
}

inline PushedValue PushValue(const ScopedValue& sv) {
  return PushValue(sv.state, sv.index);
}

// GetValue inspects the actual primitive value given by T
// (assuming T implements `MaybeGet`).
template <typename T>
typename T::Type GetValue(lua_State* state, int index) {
  auto v = T::MaybeGet(state, index);
  if (!v) {
    throw std::runtime_error("could not resolve value with type " +
                             TypeName(state, index) + " at index " +
                             std::to_string(index) + " to type " +
                             TypeName(T::TypeNum));
  }
  return *v;
}

// GetValue inspects the primitive in the PushedValue.
template <typename T>
typename T::Type GetValue(const PushedValue& pv) {
  return GetValue<T>(pv.state, pv.index);
}

// GetValue inspects the primitive in the ScopedValue.
template <typename T>
typename T::Type GetValue(const ScopedValue& sv) {
  return GetValue<T>(sv.state, sv.index);
}

}  // namespace lua
