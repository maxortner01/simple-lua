cmake_minimum_required(VERSION 3.8)

project(main)

set(CMAKE_CXX_STANDARD 17)

find_package(Lua REQUIRED)

## LUA
set(LUA_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Lua/TypeMap.cpp
    ${CMAKE_SOURCE_DIR}/src/Lua/Lib.cpp
    ${CMAKE_SOURCE_DIR}/src/Lua/Runtime.cpp
    ${CMAKE_SOURCE_DIR}/src/Lua/Table.cpp)

add_library(simple-lua SHARED ${LUA_SOURCES})

target_link_libraries(simple-lua PRIVATE ${LUA_LIBRARIES})
target_include_directories(simple-lua PRIVATE
    ${CMAKE_SOURCE_DIR}/include 
    ${LUA_INCLUDE_DIR})