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

  void PushValue(int);

  void PushInteger(lua_Integer);
  std::optional<lua_Integer> MaybeGetInteger(int);
  lua_Integer GetInteger(int);

  void PushNumber(lua_Number);
  std::optional<lua_Number> MaybeGetNumber(int);
  lua_Number GetNumber(int);

  void PushString(const char *);
  const char *MaybeGetString(int);
  const char *GetString(int);

private:
  std::shared_ptr<Allocator> m_alloc;
  lua_State *m_state;
};

} // namespace lua