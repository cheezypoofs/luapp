#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace lua {

class State;

class Exception : public std::runtime_error {
public:
  explicit Exception(State &);
  Exception(State &, const char *what);
};

template <typename PointerType> PointerType *AssertNotNull(PointerType *p) {
  assert(p);
  return p;
}

template <typename PointerType>
PointerType *AssertNotNull(PointerType *p, const char *msg) {
  assert(msg && p);
  return p;
}

template <typename PointerType>
PointerType *ThrowIfNull(State &state, PointerType *p, const char *msg) {
  if (p) {
    return p;
  }

  throw Exception(state, msg);
}

inline void ThrowIfError(State &state, int e) {
  if (!e) {
    throw Exception(state);
  }
}

inline void ThrowIfError(State &state, int e, const char *msg) {
  if (!e) {
    throw Exception(state, msg);
  }
}

} // namespace lua