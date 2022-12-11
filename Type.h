#pragma once

#include <string>

namespace lua {
class State;
std::string TypeName(State &, int index);
} // namespace lua
