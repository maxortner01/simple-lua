cmake_minimum_required(VERSION 3.5)

project(main)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SL_CODE_COVERAGE "Build kcov to measure testing coverage" OFF)
option(SL_UNIT_TESTS "Build the unit tests" OFF)
option(SL_BUILD_LIB "Build the simple-lua library" ON)
if (SL_BUILD_LIB)
    set(LUA_ENABLE_TESTING OFF CACHE BOOL "disable testing in lua")
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

        add_executable(lua_file ${CMAKE_CURRENT_SOURCE_DIR}/tests/lua_file.cpp)
        target_link_libraries(lua_file PRIVATE simple-lua GTest::gtest_main)
        target_compile_definitions(lua_file PRIVATE LUA_FILE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/lua-files")

        include(GoogleTest)
        gtest_discover_tests(hello_test)
        gtest_discover_tests(lua_file)
        
        # For code coverage
        if (SL_CODE_COVERAGE)
            message("Including code coverage")
            ExternalProject_Add(kcov
                GIT_REPOSITORY https://github.com/SimonKagstrom/kcov.git
                DOWNLOAD_EXTRACT_TIMESTAMP ON
                SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/kcov"
                DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/kcov"
                BUILD_COMMAND "make"
                INSTALL_COMMAND ""
                GIT_TAG origin/master
            )

            add_custom_command(TARGET kcov POST_BUILD
                DEPENDS hello_test
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/kcov-prefix/src/kcov-build/src/kcov --include-path=../include ${CMAKE_CURRENT_BINARY_DIR}/cov_hello_test ${CMAKE_CURRENT_BINARY_DIR}/hello_test && echo "Hello" >> t1.txt
            )

            add_custom_command(TARGET kcov POST_BUILD
                DEPENDS lua_file
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/kcov-prefix/src/kcov-build/src/kcov --include-path=../include ${CMAKE_CURRENT_BINARY_DIR}/cov_lua_file ${CMAKE_CURRENT_BINARY_DIR}/lua_file && echo "Hello" >> t2.txt
            )

            add_custom_command(TARGET kcov
                DEPENDS t1.txt t2.txt
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/kcov-prefix/src/kcov-build/src/kcov --merge ${CMAKE_CURRENT_BINARY_DIR}/coverage ${CMAKE_CURRENT_BINARY_DIR}/cov_* && echo "Hello" >> t3.txt
            )
        endif()
    endif()
endif()

option(SL_BUILD_DOCS "Build the documentation" OFF)
if (SL_BUILD_DOCS)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        ExternalProject_Add(doxygenawesome
            GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen-awesome"
            DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen-awesome"
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
            CONFIGURE_COMMAND ""
            GIT_TAG origin/main
        )

        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        # note the option ALL which allows to build the docs together with the application
        add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
    else (DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endif()

if (SL_CODE_COVERAGE AND SL_BUILD_DOCS)
    add_custom_command(TARGET kcov
        DEPENDS t3.txt doc_doxygen
        COMMAND rsync -av ${CMAKE_CURRENT_BINARY_DIR}/coverage/. ${CMAKE_CURRENT_BINARY_DIR}/docs/html --exclude=index.html && 
            cp ${CMAKE_CURRENT_BINARY_DIR}/coverage/index.html ${CMAKE_CURRENT_BINARY_DIR}/docs/html/coverage.html &&
            python3 ${CMAKE_SOURCE_DIR}/docs/get_url.py ${CMAKE_CURRENT_BINARY_DIR}/docs/html # now we need to create the forward to the correct badge
    )
endif()