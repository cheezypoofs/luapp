#include "Exception.h"

#include <string>
#include "State.h"
#include "luapp.h"

namespace lua {

namespace {
std::string MakeRuntimeError(lua_State* state, const char* msg) {
  std::string err;

  if (msg) {
    err.append("lua error performing ");
    err.append(msg);
    err.append(1, ' ');
  }

  err.append(AssertNotNull(lua_tostring(state, -1)));

  return err;
}
}  // namespace

Exception::Exception(const std::string& what) : std::runtime_error(what) {}

RuntimeException::RuntimeException(State& state)
    : Exception(MakeRuntimeError(state, nullptr)) {}

}  // namespace lua