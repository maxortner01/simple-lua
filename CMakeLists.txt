cmake_minimum_required(VERSION 3.5)

project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

option(SL_UNIT_TESTS "Build the unit tests" OFF)
if (SL_UNIT_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(hello_test ${CMAKE_CURRENT_SOURCE_DIR}/tests/hello_test.cpp)
    target_link_libraries(hello_test PRIVATE simple-lua GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(hello_test)
endif()