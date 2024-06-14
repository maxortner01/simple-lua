cmake_minimum_required(VERSION 3.5)

## LUA
set(LUA_SOURCES
    ${SL_SOURCE_DIR}/src/Lua/TypeMap.cpp
    ${SL_SOURCE_DIR}/src/Lua/Lib.cpp
    ${SL_SOURCE_DIR}/src/Lua/Runtime.cpp
    ${SL_SOURCE_DIR}/src/Lua/Table.cpp)

add_library(simple-lua SHARED ${LUA_SOURCES})

target_compile_definitions(simple-lua PRIVATE SL_BUILD)
target_link_libraries(simple-lua PRIVATE lua_static)
target_include_directories(simple-lua 
    PUBLIC
        ${SL_SOURCE_DIR}/include 
    PRIVATE
        ${LUA_INCLUDE_DIR}
)