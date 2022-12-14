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
