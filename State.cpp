#include "State.h"
#include "Allocator.h"
#include "Exception.h"
#include "Type.h"

namespace lua {

namespace {

void *allocator(void *ud, void *ptr, size_t osize, size_t nsize) {
  assert(ud);
  auto allocator = static_cast<Allocator *>(ud);

  if (ptr) {
    if (nsize == 0) {
      allocator->Free(ptr, osize);
      return nullptr;
    }

    return allocator->Realloc(ptr, nsize);
  }

  return allocator->Alloc(nsize);
}

} // namespace

State::State() : State(std::make_shared<StdAllocator>()) {}

State::State(const std::shared_ptr<Allocator> &a)
    : m_alloc(a), m_state(AssertNotNull(lua_newstate(allocator, m_alloc.get()),
                                        "luaL_newstate")) {}

State::~State() {
  assert(m_state);
  lua_close(m_state);
}

void State::DoString(const char *s) { ThrowIfError(luaL_dostring(m_state, s)); }

State &State::OpenBase() {
  ThrowIfZero(luaopen_base(m_state));
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenTable() {
  ThrowIfZero(luaopen_table(m_state));
  lua_pop(m_state, 1);
  return *this;
}

void State::PushInteger(lua_Integer i) { lua_pushinteger(m_state, i); }

void State::PushNumber(lua_Number n) { lua_pushnumber(m_state, n); }

void State::PushString(const char *s) { lua_pushstring(m_state, s); }

void State::PushValue(int index) { lua_pushvalue(m_state, index); }

std::optional<lua_Integer> State::MaybeGetInteger(int index) {
  if (!luaL_checkinteger(m_state, index)) {
    return std::optional<lua_Integer>();
  }
  return lua_tointeger(m_state, index);
}

lua_Integer State::GetInteger(int index) {
  auto i = MaybeGetInteger(index);
  if (!i) {
    throw std::runtime_error("expected integer at index " +
                             std::to_string(index) + " not " +
                             TypeName(*this, index));
  }
  return *i;
}

std::optional<lua_Number> State::MaybeGetNumber(int index) {
  if (!luaL_checknumber(m_state, index)) {
    return std::optional<lua_Number>();
  }
  return lua_tonumber(m_state, index);
}

lua_Number State::GetNumber(int index) {
  auto n = MaybeGetNumber(index);
  if (!n) {
    throw std::runtime_error("expected number at index " +
                             std::to_string(index) + " not " +
                             TypeName(*this, index));
  }
  return *n;
}

const char *State::MaybeGetString(int index) {
  if (!luaL_checkstring(m_state, index)) {
    return nullptr;
  }
  return lua_tostring(m_state, index);
}

const char *State::GetString(int index) {
  auto s = MaybeGetString(index);
  if (!s) {
    throw std::runtime_error("expected string at index " +
                             std::to_string(index) + " not " +
                             TypeName(*this, index));
  }
  return s;
}

} // namespace lua