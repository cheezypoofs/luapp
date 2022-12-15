#pragma once

#include <utility>
#include "luapp.h"

namespace lua {

// PushedValue contains the data necessary to describe the result of
// a push action onto the stack. It serves the purpose of being informational
// but also of ensure some function calls in this library are more clear that they
// are expecting a value that was just pushed, usually because those calls plan to
// pop the value. A function that returns a PushedValue is assuming that you are
// managing that the value gets popped apppropriately either by placing it
// into a ScopedValue or by calling a function that will implicitly pop it.
struct PushedValue {
  lua_State* state = nullptr;
  int type = LUA_TNONE;
  size_t num = 0;
  int index = 0;

  explicit PushedValue(lua_State* s);
  PushedValue(lua_State*, int t);

  operator bool() const;
};

// ScopedValue is used when you want to manage popping a value off the stack on scope exit.
// This can be useful as a mmember of a class that manipulates objects (like a Table), but also
// for local access of a value that was recently pushed to inspect the value.
struct ScopedValue final {
  // explicit ScopedValue(lua_State* s);
  explicit ScopedValue(const PushedValue&);
  ~ScopedValue();

  ScopedValue(const ScopedValue&) = delete;
  ScopedValue& operator=(const ScopedValue&) = delete;

  ScopedValue(ScopedValue&& o) noexcept { *this = std::move(o); }

  ScopedValue& operator=(ScopedValue&& o) noexcept {
    std::swap(state, o.state);
    std::swap(num, o.num);
    std::swap(type, o.type);
    std::swap(index, o.index);
    return *this;
  }

  lua_State* state = nullptr;
  size_t num = 0;
  int type = LUA_TNONE;
  int index = 0;
};

}  // namespace lua