#include "State.h"
#include <gtest/gtest.h>

namespace lua {
class LuaTestBase {
public:
  LuaTestBase() = default;
  virtual ~LuaTestBase() = default;

protected:
  State m_state;
};

} // namespace lua