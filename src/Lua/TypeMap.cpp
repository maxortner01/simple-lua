#include <SL/Lua/TypeMap.hpp>

#include "Lua.cpp"

namespace SL
{
namespace CompileTime
{    
    int TypeMap<void*>::LuaType = LUA_TUSERDATA;
    template<> int TypeMap<SL::Number>::LuaType   = LUA_TNUMBER;
    template<> int TypeMap<SL::String>::LuaType   = LUA_TSTRING;
    template<> int TypeMap<SL::Function>::LuaType = LUA_TFUNCTION;
    template<> int TypeMap<SL::Boolean>::LuaType  = LUA_TBOOLEAN;
    template<> int TypeMap<SL::Table>::LuaType    = LUA_TTABLE;
    
    bool
    TypeMap<void*>::check(State L)
    {
        return lua_isuserdata(reinterpret_cast<lua_State*>(L), -1);
    }

    void 
    TypeMap<void*>::push(State L, void* val)
    {
        auto* ptr = lua_newuserdata(reinterpret_cast<lua_State*>(L), sizeof(void*));
        *reinterpret_cast<void**>(ptr) = val;
    }

    void* 
    TypeMap<void*>::construct(State L)
    {
        return *reinterpret_cast<void**>(lua_touserdata(reinterpret_cast<lua_State*>(L), -1));
    }

    template<>
    bool 
    TypeMap<SL::Number>::check(State L)
    {
        return lua_isnumber(reinterpret_cast<lua_State*>(L), -1);
    }

    template<>
    void
    TypeMap<SL::Number>::push(State L, const SL::Number& number)
    {
        lua_pushnumber(reinterpret_cast<lua_State*>(L), number);
    }

    template<>
    SL::Number 
    TypeMap<SL::Number>::construct(State L)
    {
        return static_cast<SL::Number>(lua_tonumber(reinterpret_cast<lua_State*>(L), -1));
    }


    template<>
    bool 
    TypeMap<SL::String>::check(State L)
    {
        return lua_isstring(reinterpret_cast<lua_State*>(L), -1);
    }
    
    template<>
    void
    TypeMap<SL::String>::push(State L, const SL::String& string)
    {
        lua_pushstring(reinterpret_cast<lua_State*>(L), string.c_str());
    }

    template<>
    SL::String 
    TypeMap<SL::String>::construct(State L)
    {
        return SL::String(lua_tostring(reinterpret_cast<lua_State*>(L), -1));
    }
    

    template<>
    bool
    TypeMap<SL::Function>::check(State L)
    {
        return lua_iscfunction(STATE, -1);
    }

    template<>
    void
    TypeMap<SL::Function>::push(State L, const SL::Function& function)
    {
        lua_pushcfunction(reinterpret_cast<lua_State*>(L), reinterpret_cast<lua_CFunction>(function));
    }

    template<>
    SL::Function
    TypeMap<SL::Function>::construct(State L)
    {
        return (SL::Function)lua_tocfunction(reinterpret_cast<lua_State*>(L), -1);
    }


    template<>
    bool 
    TypeMap<SL::Boolean>::check(State L)
    {
        return lua_isboolean(reinterpret_cast<lua_State*>(L), -1);
    }

    template<>
    void
    TypeMap<SL::Boolean>::push(State L, const SL::Boolean& boolean)
    {
        lua_pushboolean(reinterpret_cast<lua_State*>(L), boolean);
    }

    template<>
    SL::Boolean 
    TypeMap<SL::Boolean>::construct(State L)
    {
        return lua_toboolean(reinterpret_cast<lua_State*>(L), -1);
    }

    
    template<>
    void
    TypeMap<SL::Table>::push(State L, const Table& val)
    {
        val.toStack(L);
    }

    template<>
    SL::Table
    TypeMap<SL::Table>::construct(State L)
    {
        return SL::Table(L);
    }
    
    template<>
    bool
    TypeMap<SL::Table>::check(State L)
    {
        return lua_istable(reinterpret_cast<lua_State*>(L), -1);
    }

}
}
