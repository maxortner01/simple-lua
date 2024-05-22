#pragma once

#include "Lua.hpp"
#include "Table.hpp"

namespace SL
{
    /* Types */

    using String = std::string;
    using Number = float;
    using Boolean = bool;
    using Function = int(*)(SL::State);

namespace CompileTime
{
    template<typename T>
    struct TypeMap
    {
        static int LuaType;

        static bool
        check(State L);

        static void
        push(State L, const T& val);

        static T
        construct(State L);
    };

    template<>
    struct TypeMap<void*>
    {
        static int LuaType;

        static bool
        check(State L);

        static void
        push(State L, void* val);

        static void*
        construct(State L);
    };
} // CompileTime

} // SL
