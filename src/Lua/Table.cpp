#include <SL/Lua/Table.hpp>
#include <SL/Lua/TypeMap.hpp>
#include <SL/Def.hpp>

#include "Lua.cpp"

#include <sstream>

namespace SL
{

/* Table::Data */
template<typename T>
Table::Data 
Table::Data::fromValue(const T& value)
{
    return Data {
        emplace(value),
        CompileTime::TypeMap<T>::LuaType
    };    
}
template SL_SYMBOL Table::Data Table::Data::fromValue(const SL::Number&);
template SL_SYMBOL Table::Data Table::Data::fromValue(const SL::String&);
template SL_SYMBOL Table::Data Table::Data::fromValue(const SL::Boolean&);
template SL_SYMBOL Table::Data Table::Data::fromValue(const SL::Function&);
template SL_SYMBOL Table::Data Table::Data::fromValue(const SL::Table&);
template SL_SYMBOL Table::Data Table::Data::fromValue(void* const&);

template<typename T>
std::shared_ptr<void>
Table::Data::emplace(const T& value)
{
    std::shared_ptr<void> ptr = std::shared_ptr<void>(
        new T(),
        [](void* ptr) { delete reinterpret_cast<T*>(ptr); }
    );
    *static_cast<T*>(ptr.get()) = value;
    return ptr;
}
template SL_SYMBOL std::shared_ptr<void> Table::Data::emplace(const SL::Number&);
template SL_SYMBOL std::shared_ptr<void> Table::Data::emplace(const SL::String&);
template SL_SYMBOL std::shared_ptr<void> Table::Data::emplace(const SL::Boolean&);
template SL_SYMBOL std::shared_ptr<void> Table::Data::emplace(const SL::Function&);
template SL_SYMBOL std::shared_ptr<void> Table::Data::emplace(const SL::Table&);

/* Table */

Table::Table(const Table::Map& map) :
    dictionary(map)
{   }

Table::Table(State L)
{
    fromStack(L);
}

const Table::Data& 
Table::getRaw(const std::string& name) const
{
    SL_ASSERT_ARGS(dictionary.count(name), "Error requesting raw data \"%s\"", name.c_str());
    return dictionary.at(name);
}

template<typename T>
void 
Table::each(std::function<void(uint32_t, T&)> lambda)
{
    uint32_t i = 1;
    while (hasValue(std::to_string(i)))
    {
        auto& val = get<T>(std::to_string(i));
        lambda(i++, val);
    }
}
template SL_SYMBOL void Table::each(std::function<void(uint32_t, SL::Number&)>);
template SL_SYMBOL void Table::each(std::function<void(uint32_t, SL::String&)>);
template SL_SYMBOL void Table::each(std::function<void(uint32_t, SL::Boolean&)>);
template SL_SYMBOL void Table::each(std::function<void(uint32_t, SL::Function&)>);
template SL_SYMBOL void Table::each(std::function<void(uint32_t, SL::Table&)>);
template SL_SYMBOL void Table::each(std::function<void(uint32_t, void*&)>);

template<typename T>
void 
Table::each(std::function<void(uint32_t, const T&)> lambda) const
{
    uint32_t i = 1;
    while (hasValue(std::to_string(i)))
    {
        const auto& val = get<T>(std::to_string(i));
        lambda(i++, val);
    }
}
template SL_SYMBOL void Table::each(std::function<void(uint32_t, const SL::Number&)>) const;
template SL_SYMBOL void Table::each(std::function<void(uint32_t, const SL::String&)>) const;
template SL_SYMBOL void Table::each(std::function<void(uint32_t, const SL::Boolean&)>) const;
template SL_SYMBOL void Table::each(std::function<void(uint32_t, const SL::Function&)>) const;
template SL_SYMBOL void Table::each(std::function<void(uint32_t, const SL::Table&)>) const;
template SL_SYMBOL void Table::each(std::function<void(uint32_t, void* const&)>) const;

template<typename T>
void Table::try_get(const std::string& name, std::function<void(T&)> lambda, std::optional<std::function<void()>> if_not)
{
    if (hasValue(name)) lambda(get<T>(name));
    else { if (if_not.has_value()) if_not.value()(); }
}
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(SL::Number&)>, std::optional<std::function<void()>>);
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(SL::String&)>, std::optional<std::function<void()>>);
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(SL::Boolean&)>, std::optional<std::function<void()>>);
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(SL::Function&)>, std::optional<std::function<void()>>);
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(SL::Table&)>, std::optional<std::function<void()>>);
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(void*&)>, std::optional<std::function<void()>>);

template<typename T>
void Table::try_get(const std::string& name, std::function<void(const T&)> lambda, std::optional<std::function<void()>> if_not) const
{
    if (hasValue(name)) lambda(get<T>(name));
    else { if (if_not.has_value()) if_not.value()(); }
}
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(const SL::Number&)>, std::optional<std::function<void()>>) const;
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(const SL::String&)>, std::optional<std::function<void()>>) const;
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(const SL::Boolean&)>, std::optional<std::function<void()>>) const;
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(const SL::Function&)>, std::optional<std::function<void()>>) const;
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(const SL::Table&)>, std::optional<std::function<void()>>) const;
template SL_SYMBOL void Table::try_get(const std::string&, std::function<void(void* const&)>, std::optional<std::function<void()>>) const;

void
Table::fromTable(const Table& table)
{
    dictionary = table.dictionary;
}

void 
Table::superimpose(const Table& table)
{
    superimpose(table.dictionary);
}

void 
Table::superimpose(const Map& map)
{
    for (const auto& p : map)
        dictionary.insert(p);
}

bool Table::hasValue(const std::string& name) const
{
    return dictionary.count(name);
}

template<typename T>
T& Table::get(const std::string& name)
{
    SL_ASSERT(dictionary.count(name), "Dictionary doesn't have key");
    return *static_cast<T*>(dictionary.at(name).data.get());
}
template SL_SYMBOL SL::Number&   Table::get(const std::string&);
template SL_SYMBOL SL::String&   Table::get(const std::string&);
template SL_SYMBOL SL::Boolean&  Table::get(const std::string&);
template SL_SYMBOL SL::Function& Table::get(const std::string&);
template SL_SYMBOL SL::Table&    Table::get(const std::string&);
template SL_SYMBOL void**&        Table::get(const std::string&);

template<typename T>
const T& Table::get(const std::string& name) const
{
    SL_ASSERT(dictionary.count(name), "Dictionary doesn't have key");
    return *static_cast<T*>(dictionary.at(name).data.get());
}
template SL_SYMBOL const SL::Number&   Table::get(const std::string&) const;
template SL_SYMBOL const SL::String&   Table::get(const std::string&) const;
template SL_SYMBOL const SL::Boolean&  Table::get(const std::string&) const;
template SL_SYMBOL const SL::Function& Table::get(const std::string&) const;
template SL_SYMBOL const SL::Table&    Table::get(const std::string&) const;
template SL_SYMBOL void** const&        Table::get(const std::string&) const;

template<typename T>
void Table::set(const std::string& name, const T& value)
{
    dictionary.insert(std::pair(name, Table::Data::fromValue(value)));
}
template SL_SYMBOL void Table::set(const std::string&, const SL::Number&);
template SL_SYMBOL void Table::set(const std::string&, const SL::String&);
template SL_SYMBOL void Table::set(const std::string&, const SL::Boolean&);
template SL_SYMBOL void Table::set(const std::string&, const SL::Function&);
template SL_SYMBOL void Table::set(const std::string&, const SL::Table&);

void Table::set(const std::string& name, void* value)
{
    dictionary.insert(std::pair(name, Table::Data::fromValue(value)));
}

const Table::Map&
Table::getMap() const
{ return dictionary; }

void
Table::toStack(State L) const
{
    using namespace CompileTime;

    lua_newtable(STATE);

    for (const auto& p : dictionary)
    {
        lua_pushstring(STATE, p.first.c_str());

        switch (p.second.type)
        {
        case LUA_TNUMBER:  TypeMap<SL::Number> ::push(L, get<SL::Number> (p.first)); break;
        case LUA_TSTRING:  TypeMap<SL::String> ::push(L, get<SL::String> (p.first)); break;
        case LUA_TBOOLEAN: TypeMap<SL::Boolean>::push(L, get<SL::Boolean>(p.first)); break;
        case LUA_TTABLE:
        {
            const auto& table = get<SL::Table>(p.first);
            table.toStack(L);
            break;
        }
        case LUA_TFUNCTION: TypeMap<SL::Function>::push(L, get<SL::Function>(p.first)); break;
        default: TypeMap<void*>::push(L, get<void*>(p.first)); break; // Worried about this... everywhere else needs void** so why does void* work?
        }

        lua_settable(STATE, -3);
    }
}

void
Table::fromStack(State L)
{
    lua_pushnil(STATE);
    while (lua_next(STATE, -2) != 0)
    {
        const auto key = [&]()
        {
            switch (lua_type(STATE, -2))
            {
            case LUA_TSTRING: return std::string(lua_tostring(STATE, -2));
            case LUA_TNUMBER: return std::to_string((int)lua_tonumber(STATE, -2));
            default: SL_ASSERT(false, "Lua type mismatch");
            }
        }();
        
        std::shared_ptr<void> value;

        const auto count = lua_gettop(STATE);
        const auto type = lua_type(STATE, -1);
        switch(type)
        {
        case LUA_TNUMBER:   value = Data::emplace(static_cast<SL::Number>(lua_tonumber(STATE, -1))); break;
        case LUA_TSTRING:   value = Data::emplace(SL::String(lua_tostring(STATE, -1)));              break;
        case LUA_TBOOLEAN:  value = Data::emplace(lua_toboolean(STATE, -1));                          break;
        case LUA_TUSERDATA: value = Data::emplace(lua_touserdata(STATE, -1));                         break;
        case LUA_TTABLE:    value = Data::emplace(Table(STATE));                                      break;
        }
        
        dictionary.insert(std::pair(key, Data{ value, type }));
        if (count == lua_gettop(STATE)) lua_pop(STATE, 1);
    }
    lua_pop(STATE, 1);
}

std::string Table::toString(uint32_t indent) const
{
    const auto indent_string = [&]()
    {
        std::string r;
        for (uint32_t i = 0; i < indent; i++) r += " ";
        return r;
    }();

    std::stringstream ss;
    uint32_t index = 0;
    for (const auto& p : getMap())
    {
        if (index) ss << ",\n";
        
        using namespace SL::CompileTime;
        /**/ if (p.second.type == TypeMap<SL::String>::LuaType)
        {
            const SL::String* value = reinterpret_cast<SL::String*>(p.second.data.get());
            ss << indent_string << p.first << " = \"" << *value << "\"";
        }
        else if (p.second.type == TypeMap<SL::Number>::LuaType)
        {
            const SL::Number* value = reinterpret_cast<SL::Number*>(p.second.data.get());
            ss << indent_string << p.first << " = " << *value << "";
        }
        else if (p.second.type == TypeMap<SL::Boolean>::LuaType)
        {
            const SL::Boolean* value = reinterpret_cast<SL::Boolean*>(p.second.data.get());
            ss << indent_string << p.first << " = " << ( *value ? "true" : "false") << "";
        }
        else if (p.second.type == TypeMap<SL::Table>::LuaType)
        {
            const SL::Table* value = reinterpret_cast<SL::Table*>(p.second.data.get());
            ss << indent_string << p.first << " = {\n";
            ss << value->toString(indent + 2);
            ss << indent_string << "\n}";
        }
        
        index++;
    }
    return ss.str();
}

} // SL
