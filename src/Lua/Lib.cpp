#include <SL/Lua/Lib.hpp>
#include <SL/Lua/Runtime.hpp>

#include "Lua.cpp"

namespace SL::Lib
{

void Base::registerFunctions(SL::Runtime& runtime) const
{
    for (auto& p : _funcs)
        runtime.registerFunction(_name, p.first, p.second);
}

SL::Table Base::asTable() const
{
    SL::Table table;

    for (auto& p : _funcs)
        table.set(p.first, p.second);

    return table;
}

Base::Base(
    const std::string& name,
    const Base::Map& funcs) :
        _name(name),
        _funcs(funcs)
{   }

namespace detail
{
    std::size_t __getTop(State L)
    {
        return lua_gettop(STATE);
    }

    bool __isNil(State L)
    {
        return lua_isnil(STATE, -1);
    }

    void __pop(State L, uint32_t n)
    {
        lua_pop(STATE, static_cast<int32_t>(n));
    }
}

} // SL::Lib