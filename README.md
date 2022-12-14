# luapp
An exploration of wrapping the lua C API with c++ constructs.

(currently a personal WIP)

## Motivation

Extensibility is great. Safe and controlled extensbility is even more great. [Lua](https://www.lua.org/) is a well-known extension language and already has a C-based interpretter that supports a wide breadth of target platforms. It's easy to sandbox and doesn't suffer a deluge of poorly-performing or dependency-ridden libraries or builtins.

There are several existing c++ wrappers around lua, but I didn't really love any of them. In some cases, the performance or safety seemed suspect at first glance or the usage experience didn't strike me as optimal. Truth be told, I was also looking for an excuse to see if I could model things myself :)

## Quick Usage

The [./simple_lua.cpp] file shows a quick example of how one might use the library to setup a standard CLI interpreter. The real power of the library comes with embedding and extending, not really in this way.

```cpp

#include <iostream>
#include "State.h"
#include "luapp.h"

int main(int argc, const char** argv) {
  try {
    lua::State state;
    state.OpenAll();

    if (argc <= 1) {
      state.DoStdin();
    } else {
      state.DoFile(argv[1]);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
```
