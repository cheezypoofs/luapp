#pragma once

#include "luapp.h"
#include <optional>
#include <stdexcept>
#include <string>

namespace lua {
class State;
std::string TypeName(int luaTypeNum);
std::string TypeName(State &, int index);

struct NilType final {
  static int TypeNum;
  static void Push(State &);
  static bool Is(State &, int index);
};

struct IntType final {
  typedef lua_Integer Type;
  static int TypeNum;
  static void Push(State &, Type);
  static std::optional<Type> MaybeGet(State &, int index);
};

struct NumberType final {
  typedef lua_Number Type;
  static int TypeNum;
  static void Push(State &, Type);
  static std::optional<Type> MaybeGet(State &, int index);
};

struct StringType final {
  typedef const char *Type;
  static int TypeNum;
  static void Push(State &, Type);
  static std::optional<Type> MaybeGet(State &, int index);
};

template <typename T> typename T::Type GetType(State &state, int index) {
  auto v = T::MaybeGet(state, index);
  if (!v) {
    throw std::runtime_error("could not resolve value with type " +
                             TypeName(state, index) + " at index " +
                             std::to_string(index) + " to type " +
                             TypeName(T::TypeNum));
  }
  return *v;
}

} // namespace lua
