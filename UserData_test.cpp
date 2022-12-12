#include "Lua_test.h"
#include "Type.h"
#include "UserData.h"

using namespace lua;

class UserDataTest : public LuaTestBase {};

namespace {
class CustomType : public UserData {
public:
  static const char *Name;

  CustomType(lua_State *) { std::cout << "CustomType()" << std::endl; }
  ~CustomType() { std::cout << "~CustomType()" << std::endl; }

protected:
  void SetThing(const char *v) noexcept { m_thing = v; }

  const char *GetThing() const noexcept { return m_thing.c_str(); }

  const char *Hello() const noexcept { return "hello"; }

  const char *Speak(lua_State *L) const noexcept { return "bark"; }

  const char *Echo(lua_State *L) const noexcept { return luaL_checkstring(L, 2); }

  const IndexMap *Indexes() const noexcept override { return &indexes; }

private:
  static IndexMap indexes;
  std::string m_thing;
};
const char *CustomType::Name = "CustomType";

UserData::IndexMap CustomType::indexes = {
    // A simple property
    {"hello", Index().Getter<StringType>(&CustomType::Hello)},
    // A 0-arg method
    {"speak", Index().Method<StringType>(&CustomType::Speak)},
    // A 1-arg method
    {"echo", Index().Method<StringType>(&CustomType::Echo)},
    // A getter/setter property
    {"thing", Index()
                  .Setter<StringType>(&CustomType::SetThing)
                  .Getter<StringType>(&CustomType::GetThing)},
};

} // namespace

TEST_F(UserDataTest, TestCustomType) {
  RegisterUserData<CustomType>(*m_state);

  m_state->OpenBase();
  m_state->DoString(u8R"(
local f = CustomType()
print(f.hello)
print(f.hello)
print(f.speak())
print(f.echo("meow"))
f.thing="thing"
print(f.thing)
    )");
}
