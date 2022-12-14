#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace lua {

class State;

class Exception : public std::runtime_error {
 public:
  explicit Exception(const std::string& what);
};

class RuntimeException : public Exception {
 public:
  // An Exception where the error is expected to have been pushed onto the
  // stack.
  explicit RuntimeException(State&);
};

template <typename PointerType>
PointerType* AssertNotNull(PointerType* p) {
  assert(p);
  return p;
}

template <typename PointerType>
PointerType* AssertNotNull(PointerType* p, const char* operation) {
  assert(operation && p);
  return p;
}

#define LuaStringExpr(x) #x
#define LuaExpandExpr(x) LuaStringExpr(x)

#define ThrowIfError(e)                                                    \
  if ((e)) {                                                               \
    throw Exception(std::string("failed lua call ") + LuaExpandExpr((e))); \
  }

#define ThrowIfZero(e)                                                     \
  if (!(e)) {                                                              \
    throw Exception(std::string("failed lua call ") + LuaExpandExpr((e))); \
  }

}  // namespace lua