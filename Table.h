#pragma once

#include "Type.h"

namespace lua {

struct TableType final {
  static int TypeNum;
  static PushedValue Create(lua_State*, int narr, int nrec);
};

// Table provides getter/setter access to a table object in lua.
// To constructors push an existing or new table onto the stack and manage
// it with a ScopedValue. The Get/Set operations can be performed on that
// instance and the table will be removed from the stack on destruction.
// If you need to use the Table as a value to another call, perhaps because you
// are setting this table into another table, you can use PushSelf.
class Table final {
 public:
  // Table opens a view to a table already on the stack (perhaps a function argument).
  // It does not manage popping the value on destruction.
  Table(lua_State*, int index);
  // Table opens a view to a table created on the stack. It does not manage popping the value
  // on destruction.
  explicit Table(const PushedValue&);
  // Table accepts ownership of a ScopedValue and will pop on destruction.
  explicit Table(ScopedValue&&);
  ~Table();

  PushedValue PushSelf() const;

  void Set(const PushedValue& key, const PushedValue& value);
  void Set(const PushedValue&, const char* key);

  PushedValue Get(const PushedValue& key);
  PushedValue Get(const char* key);

 private:
  lua_State* m_state = nullptr;
  int m_index = 0;

  // m_value is not used, it just manages lifetime if we need to.
  ScopedValue m_value;
};
}  // namespace lua