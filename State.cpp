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

State &State::OpenAll() {
  luaL_openlibs(m_state);
  return *this;
}

State &State::OpenBase() {
  luaL_requiref(m_state, LUA_GNAME, luaopen_base, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenTable() {
  luaL_requiref(m_state, LUA_TABLIBNAME, luaopen_table, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenCoroutine() {
  luaL_requiref(m_state, LUA_COLIBNAME, luaopen_coroutine, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenIO() {
  luaL_requiref(m_state, LUA_IOLIBNAME, luaopen_io, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenPackage() {
  luaL_requiref(m_state, LUA_LOADLIBNAME, luaopen_package, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenOS() {
  luaL_requiref(m_state, LUA_OSLIBNAME, luaopen_os, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenString() {
  luaL_requiref(m_state, LUA_STRLIBNAME, luaopen_string, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenMath() {
  luaL_requiref(m_state, LUA_MATHLIBNAME, luaopen_math, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenUTF8() {
  luaL_requiref(m_state, LUA_UTF8LIBNAME, luaopen_utf8, 1);
  lua_pop(m_state, 1);
  return *this;
}

State &State::OpenDebug() {
  luaL_requiref(m_state, LUA_DBLIBNAME, luaopen_debug, 1);
  lua_pop(m_state, 1);
  return *this;
}

} // namespace lua