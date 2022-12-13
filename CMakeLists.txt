
cmake_minimum_required(VERSION 3.1)
project(luapp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    "/usr/local/include"
    "../3rdParty/lua"
)

link_directories(
    "/usr/local/lib"
    "../3rdParty/lua"
)

add_library(
    luapp

    luapp.h
    Allocator.h
    Allocator.cpp
    Exception.h
    Exception.cpp
    State.h
    State.cpp
    Type.h
    Type.cpp
    UserData.h
)

add_executable(
    luapp_test

    luapp_test.cpp
    State_test.cpp
    Type_test.cpp
    UserData_test.cpp
)

target_link_libraries(
    luapp_test
    luapp
    gtest
    lua
)
