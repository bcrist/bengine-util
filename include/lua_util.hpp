#pragma once
#ifndef BE_UTIL_LUA_UTIL_HPP_
#define BE_UTIL_LUA_UTIL_HPP_

#include "lua_util_autolink.hpp"
#include <be/core/zlib.hpp>
#include <be/util/util_autolink.hpp>
#include <lua/lua.h>
#include <lua/lauxlib.h>

namespace be {
namespace belua {

///////////////////////////////////////////////////////////////////////////////
int open_util(lua_State* L);

extern const luaL_Reg util_module;

} // be::belua
} // be

#endif
