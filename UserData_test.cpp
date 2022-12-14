#include "UserData.h"
#include "Lua_test.h"
#include "Type.h"

using namespace lua;

class UserDataTest : public LuaTestBase {};

namespace {
class CustomType : public UserData {
 public:
  static const char* Name;

  explicit CustomType(lua_Integer i) {
    std::cout << "CustomType(" << i << ")" << std::endl;
  }
  ~CustomType() { std::cout << "~CustomType()" << std::endl; }

 protected:
  Void Noop() noexcept { return Void(); }

  void SetThing(const char* v) noexcept { m_thing = v; }

  const char* GetThing() const noexcept { return m_thing.c_str(); }

  const char* Hello() const noexcept { return "hello"; }

  const char* Speak() noexcept { return "bark"; }

  const char* Echo(const char* e) noexcept { return e; }

  const IndexMap* Indexes() const noexcept override { return &indexes; }

 private:
  static IndexMap indexes;
  std::string m_thing;
};
const char* CustomType::Name = "CustomType";

UserData::IndexMap CustomType::indexes = {
    // A simple property
    {"hello", Index().Getter<StringType>(&CustomType::Hello)},
    // A 0-arg method
    {"speak", Index().Method0<StringType>(&CustomType::Speak)},
    // A 1-arg method
    {"echo", Index().Method1<StringType>(&CustomType::Echo)},
    // A void method
    {"noop", Index().Method0<NoneType>(&CustomType::Noop)},
    // A getter/setter property
    {"thing", Index()
                  .Setter<StringType>(&CustomType::SetThing)
                  .Getter<StringType>(&CustomType::GetThing)},
};

}  // namespace

TEST_F(UserDataTest, TestCustomType) {
  RegisterUserData1<CustomType, lua_Integer>(*m_state);

  m_state->OpenBase();
  m_state->DoString(
      u8R"(
local f = CustomType(99)
print(f.hello)
print(f.hello)
print(f.speak())
print(f.echo("meow"))
f.thing="thing"
print(f.thing)
local n = f.noop()
print(n)

-- should be able to kill `f` and `s` is still valid
local s = f.speak
f = nil
print(s())
    )");
}
