#pragma once
#ifndef BE_UTIL_LUA_FS_HPP_
#define BE_UTIL_LUA_FS_HPP_

#include "lua_util_autolink.hpp"
#include <be/util/util_autolink.hpp>
#include <lua/lua.h>
#include <lua/lauxlib.h>

namespace be {
namespace belua {

///////////////////////////////////////////////////////////////////////////////
int open_fs(lua_State* L);

extern const luaL_Reg fs_module;

} // be::belua
} // be

#endif
