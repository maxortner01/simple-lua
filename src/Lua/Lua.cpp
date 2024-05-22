extern "C"
{
#include "../extern/lua/lua-5.4.6/include/lua.h"
#include "../extern/lua/lua-5.4.6/include/lauxlib.h"
#include "../extern/lua/lua-5.4.6/include/lualib.h"
}

#define STATE reinterpret_cast<lua_State*>(L)