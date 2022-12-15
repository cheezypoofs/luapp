#pragma once

#include "Type.h"

namespace lua {

struct TableType final {
  static int TypeNum;
  static PushedValue Create(lua_State*, int narr, int nrec);
};

class Table final {
 public:
  Table(lua_State*, int index);
  explicit Table(ScopedValue&&);
  ~Table();

  void Release() { m_state = nullptr; }

  void Set(const PushedValue& key, const PushedValue& value);
  void Set(const PushedValue&, const char* key);

  PushedValue Get(const PushedValue& key);
  PushedValue Get(const char* key);

 private:
  lua_State* m_state = nullptr;
  ScopedValue m_value;
};
}  // namespace lua