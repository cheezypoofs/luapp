#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "luapp.h"

#ifndef LUAPP_SMALL_MAP
#define LUAPP_SMALL_MAP std::unordered_map
#endif

namespace lua {
std::string TypeName(int luaTypeNum);
std::string TypeName(lua_State*, int index);

struct NilType final {
  static int TypeNum;
  static void Push(lua_State*);
  static bool Is(lua_State*, int index);
};

struct IntType final {
  typedef lua_Integer Type;
  static int TypeNum;
  static void Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

struct NumberType final {
  typedef lua_Number Type;
  static int TypeNum;
  static void Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

struct StringType final {
  typedef const char* Type;
  static int TypeNum;
  static void Push(lua_State*, Type);
  static std::optional<Type> MaybeGet(lua_State*, int index);
};

struct TableType final {
  static int TypeNum;
  static void Push(lua_State*, size_t narr, size_t nrec);
};

template <typename T>
typename T::Type GetType(lua_State* state, int index) {
  auto v = T::MaybeGet(state, index);
  if (!v) {
    throw std::runtime_error("could not resolve value with type " +
                             TypeName(state, index) + " at index " +
                             std::to_string(index) + " to type " +
                             TypeName(T::TypeNum));
  }
  return *v;
}

}  // namespace lua
