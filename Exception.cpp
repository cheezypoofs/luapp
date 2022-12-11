#include "Exception.h"

#include "State.h"
#include "luapp.h"
#include <string>

namespace lua {

namespace {
std::string MakeError(lua_State *state, const char *msg) {
  std::string err;

  if (msg) {
    err.append("lua error performing ");
    err.append(msg);
    err.append(1, ' ');
  }

  err.append(AssertNotNull(lua_tostring(state, -1)));

  return err;
}
} // namespace

Exception::Exception(State &state)
    : std::runtime_error(MakeError(state, nullptr)) {}

Exception::Exception(State &state, const char *reason)
    : std::runtime_error(MakeError(state, reason)) {}

} // namespace lua