#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "luapp.h"

namespace lua {

class Allocator;

class State final {
 public:
  State();
  explicit State(const std::shared_ptr<Allocator>&);
  ~State();

  operator lua_State*() const { return m_state; }

  void DoString(const char*);

  // Call `luaL_openlibs`. This is mutually exclusive with the
  // Open* functions below.
  State& OpenAll();

  // Explicitly load in libraries rather than using `luaL_openlibs` to
  // load them all.
  State& OpenBase();
  State& OpenTable();
  State& OpenCoroutine();
  State& OpenIO();
  State& OpenPackage();
  State& OpenOS();
  State& OpenString();
  State& OpenMath();
  State& OpenUTF8();
  State& OpenDebug();

 private:
  std::shared_ptr<Allocator> m_alloc;
  lua_State* m_state;
};

}  // namespace lua