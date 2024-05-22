cmake_minimum_required(VERSION 3.8)

project(main)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/lua)

## LUA
set(LUA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lua/TypeMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lua/Lib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lua/Runtime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lua/Table.cpp)

add_library(simple-lua SHARED ${LUA_SOURCES})

target_compile_definitions(simple-lua PRIVATE SL_BUILD)
target_link_libraries(simple-lua PRIVATE lua_static)
target_include_directories(simple-lua 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include 
    PRIVATE
        ${LUA_INCLUDE_DIR}
)