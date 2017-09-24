#include "lua_fnv256.hpp"
#include <be/util/fnv.hpp>
#include <be/belua/lua_helpers.hpp>

namespace be::belua {

///////////////////////////////////////////////////////////////////////////////
const luaL_Reg fnv256_module { "be.fnv256", open_fnv256 };

namespace {

///////////////////////////////////////////////////////////////////////////////
const char* metatable() {
   return "class be.fnv256";
}

///////////////////////////////////////////////////////////////////////////////
SV parse_input(lua_State* L) {
   // if the first parameter is "be.fnv256", remove it.
   if (lua_gettop(L) >= 1 && lua_istable(L, 1)) {
      lua_getmetatable(L, 1);
      luaL_getmetatable(L, metatable());

      if (lua_rawequal(L, -1, -2)) {
         lua_remove(L, 1);
      }

      lua_pop(L, 2); // remove metatables
   }

   return belua::check_string_view(L, 1);
}

///////////////////////////////////////////////////////////////////////////////
int fnv256_0(lua_State* L) {
   belua::push_string(L, util::fnv256_0(parse_input(L)));
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fnv256_1(lua_State* L) {
   belua::push_string(L, util::fnv256_1(parse_input(L)));
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fnv256_1a(lua_State* L) {
   belua::push_string(L, util::fnv256_1a(parse_input(L)));
   return 1;
}

} // be::belua::()

///////////////////////////////////////////////////////////////////////////////
int open_fnv256(lua_State* L) {
   luaL_Reg fn[] {
      { "fnv0", fnv256_0 },
      { "fnv1", fnv256_1 },
      { "fnv1a", fnv256_1a },
      { nullptr, nullptr }
   };

   luaL_Reg meta[] {
      { "__call", fnv256_1a },
      { nullptr, nullptr }
   };

   // create class and and set class metatable
   luaL_newlib(L, fn);
   luaL_newmetatable(L, metatable());
   luaL_setfuncs(L, meta, 0);
   lua_setmetatable(L, -2);

   return 1;
}

} // be::belua
