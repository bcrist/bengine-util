#pragma once
#ifndef BE_UTIL_LUA_FNV256_HPP_
#define BE_UTIL_LUA_FNV256_HPP_

#include <lua/lua.h>
#include <lua/lauxlib.h>

namespace be::belua {

///////////////////////////////////////////////////////////////////////////////
int open_fnv256(lua_State* L);

extern const luaL_Reg fnv256_module;

} // be::belua

#endif
