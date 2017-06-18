#pragma once
#ifndef BE_UTIL_LUA_UTIL_HPP_
#define BE_UTIL_LUA_UTIL_HPP_

#include "util_lua_autolink.hpp"
#include <be/util/zlib.hpp>
#include <be/util/util_autolink.hpp>
#include <be/util/util_compression_autolink.hpp>
#include <lua/lua.h>
#include <lua/lauxlib.h>

namespace be::belua {

///////////////////////////////////////////////////////////////////////////////
int open_util(lua_State* L);

extern const luaL_Reg util_module;

} // be::belua

#endif
