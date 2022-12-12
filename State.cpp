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

    return allocator->Realloc(ptr, osize, nsize);
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

} // namespace lua