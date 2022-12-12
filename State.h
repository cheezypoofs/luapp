#pragma once

#include "luapp.h"
#include <cstdint>
#include <memory>
#include <optional>

namespace lua {

class Allocator;

class State final {
public:
  State();
  explicit State(const std::shared_ptr<Allocator> &);
  ~State();

  operator lua_State *() const { return m_state; }

  void DoString(const char *);

  // helpers around built-in capabilities
  State &OpenBase();
  State &OpenTable();

private:
  std::shared_ptr<Allocator> m_alloc;
  lua_State *m_state;
};

} // namespace lua