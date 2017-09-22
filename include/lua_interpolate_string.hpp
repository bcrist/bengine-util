#pragma once
#ifndef BE_UTIL_LUA_INTERPOLATE_STRING_HPP_
#define BE_UTIL_LUA_INTERPOLATE_STRING_HPP_

#include <lua/lua.h>
#include <lua/lauxlib.h>

namespace be {
namespace belua {

///////////////////////////////////////////////////////////////////////////////
int open_interpolate_string(lua_State* L);

extern const luaL_Reg interpolate_string_module;

} // be::belua
} // be

#endif
