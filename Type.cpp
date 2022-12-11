#include "Type.h"
#include "State.h"
#include "luapp.h"

namespace lua {

std::string TypeName(State &state, int index) {

  std::string name;

  auto n = lua_typename(state, lua_type(state, index));
  if (!n) {
    name = "???";
  } else {
    name = n;
  }

  return name;
}
} // namespace lua