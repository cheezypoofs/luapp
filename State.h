#pragma once

#include <cstdint>
#include <memory>
#include "luapp.h"

namespace lua {

class Allocator;

class State final {
 public:
  // Instantiate a new default state with the std allocator.
  State();

  // Instantiate a state with a specified allocator.
  explicit State(const std::shared_ptr<Allocator>&);
  ~State();

  // Cast this to a lua_State* for easy use with the C API.
  operator lua_State*() const { return m_state; }

  // Perform luaL_dostring. Throw on returned error.
  void DoString(const char*);

  void DoStdin();
  void DoFile(const char*);

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