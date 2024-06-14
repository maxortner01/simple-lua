#include <gtest/gtest.h>

#include <SL/Lua.hpp>

#ifndef LUA_FILE_DIR
#define LUA_FILE_DIR "."
#endif

// Open a lua file
TEST(LuaFile, FileOpen) {
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);
}

TEST(LuaFile, GetGlobals) {
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);

    const auto res = runtime.getGlobal<SL::Table>("TestTable");
    EXPECT_TRUE(res);
}

TEST(LuaFile, GetTableValues)
{
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);

    const auto res = runtime.getGlobal<SL::Table>("TestTable");
    EXPECT_TRUE(res);

    EXPECT_EQ(res->get<SL::String>("name"), "Test");
    
    const auto& sub = res->get<SL::Table>("sub");
    EXPECT_FLOAT_EQ(sub.get<SL::Number>("number"), 4.5f);
}

TEST(LuaFile, RunLuaFunction)
{
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);

    const auto res = runtime.template runFunction<SL::Number>("AddTwo", 2.f);
    EXPECT_TRUE(res);
    EXPECT_FLOAT_EQ(std::get<0>(*res), 4.f);
}

struct Suite : SL::Lib::Base
{
    Suite() : Base("suite", { { "run", run }, { "addToValue", addToValue } })
    {   }

    static int addToValue(SL::State state)
    {
        auto [ obj, value ] = extractArgs<SL::Table, SL::Number>(state);
        SL::CompileTime::TypeMap<SL::Number>::push(state, obj.get<SL::Number>("value") + value);
        return 1;
    }

    static int run(SL::State state)
    {
        const auto args = extractArgs<SL::Number>(state);
        SL::CompileTime::TypeMap<SL::Number>::push(state, std::get<0>(args) + 2);
        return 1;
    }
};

TEST(LuaFile, RunCppFunction)
{
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);

    {
        const auto res = runtime.registerFunction("Global", "CppAddTwo", Suite::run);
        EXPECT_TRUE(res);
    }
    {
        const auto res = runtime.template runFunction<SL::Number>("CallGlobalFunction", 2.f);
        EXPECT_TRUE(res);
        EXPECT_FLOAT_EQ(std::get<0>(*res), 4.f);
    }
}

TEST(LuaFile, RunCppLib)
{
    SL::Runtime runtime(LUA_FILE_DIR "/test_a.lua");
    EXPECT_TRUE(runtime);

    Suite suite;
    auto table = suite.asTable();
    table.set("value", 2.f);
    const auto res = runtime.template runFunction<SL::Table>("TestLib", table);
    EXPECT_TRUE(res);
    EXPECT_FLOAT_EQ(std::get<0>(*res).get<SL::Number>("value"), 4.f);
}