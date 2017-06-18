#include "lua_util.hpp"
#include <be/util/zlib.hpp>
#include <be/core/filesystem.hpp>
#include <be/util/pointer_to_string.hpp>

namespace be::belua {

///////////////////////////////////////////////////////////////////////////////
const luaL_Reg util_module { "be.util", open_util };

namespace {

///////////////////////////////////////////////////////////////////////////////
bool util_is_valid_varname(const char* s) {
   if (!s) {
      return false;
   }

   char c = *s;
   if (!(c >= 'a' && c <= 'z' ||
         c >= 'A' && c <= 'Z' ||
         c == '_')) {
      return false;
   }

   ++s;
   c = *s;

   while (c) {
      c = *s;
      if (!(c >= 'a' && c <= 'z' ||
            c >= 'A' && c <= 'Z' ||
            c >= '0' && c <= '9' ||
            c == '_')) {
         return false;
      }
      ++s;
      c = *s;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
int util_print_r_get_first(lua_State* L) {
   bool meta_obj = lua_gettop(L) == 1; // called with 2 params when this is main object
   bool key_obj = lua_gettop(L) == 2 && !lua_toboolean(L, 2);  // when second parameter is false, this is a key object
   int type = lua_type(L, 1);

   const char* tstr = "unknown";
   switch (type) {
      case LUA_TNIL:             tstr = "nil"; break;
      case LUA_TBOOLEAN:         tstr = "boolean"; break;
      case LUA_TSTRING:          tstr = "string"; break;
      case LUA_TNUMBER:          tstr = "number"; break;
      case LUA_TTABLE:           tstr = "table"; break;
      case LUA_TFUNCTION:        tstr = "function"; break;
      case LUA_TTHREAD:          tstr = "thread"; break;
      case LUA_TUSERDATA:        tstr = "userdata"; break;
      case LUA_TLIGHTUSERDATA:   tstr = "lightuserdata"; break;
      default: break;
   }

   switch (type) {
      case LUA_TTABLE:
         if (meta_obj) {
            S s;

            lua_pushvalue(L, LUA_REGISTRYINDEX);
            int index = lua_gettop(L);
            bool meta_name_found = false;
            lua_pushnil(L);
            while (!meta_name_found && lua_next(L, index) != 0) {
               // -2 key (metatable name)
               // -1 value (metatable)

               if (lua_type(L, -2) == LUA_TSTRING && lua_rawequal(L, -1, 1)) {
                  s = lua_tostring(L, -2);
                  lua_pop(L, 1); // we found the metatable, so pop the key and value
                  meta_name_found = true;
               }

               lua_pop(L, 1); // remove value, keep key for next
            }

            lua_pop(L, 1); // pop the registry

            if (!meta_name_found) {
               s = "table(";
               s += util::get_pointer_address_string(lua_topointer(L, 1));
               s += ')';
            }

            if (lua_getmetatable(L, 1)) {
               s += " --> ";
               lua_pushcfunction(L, util_print_r_get_first);
               lua_insert(L, -2);
               lua_call(L, 1, 1);
               s += lua_tostring(L, -1);
               lua_pop(L, 1);
            }
            lua_pushstring(L, s.c_str());

         } else if (key_obj) {
            S s = "[table(";
            s += util::get_pointer_address_string(lua_topointer(L, 1));
            s += ")]";
            lua_pushstring(L, s.c_str());

         } else { // regular obj
            S s = "{ -- ";

            if (luaL_callmeta(L, 1, "__tostring")) {
               lua_pushcfunction(L, util_print_r_get_first);
               lua_insert(L, -2);
               lua_call(L, 1, 1);
               s += lua_tostring(L, -1);
               s += ' ';
               lua_pop(L, 1);
            }

            s += '(';
            s += util::get_pointer_address_string(lua_topointer(L, 1));
            s += ')';

            if (lua_getmetatable(L, 1)) {
               s += " --> ";
               lua_pushcfunction(L, util_print_r_get_first);
               lua_insert(L, -2);
               lua_call(L, 1, 1);
               s += lua_tostring(L, -1);
               lua_pop(L, 1);
            }
            lua_pushstring(L, s.c_str());
         }
         break;

      case LUA_TFUNCTION:
      case LUA_TTHREAD:
      case LUA_TUSERDATA:
      case LUA_TLIGHTUSERDATA: {
         S s;
         if (key_obj) {
            s += '[';
         }
         s += tstr;
         s += '(';
         s += util::get_pointer_address_string(lua_topointer(L, 1));
         s += ')';
         if (key_obj) {
            s += ']';
         }

         if (type == LUA_TUSERDATA && !key_obj && lua_getmetatable(L, 1)) {
            if (!meta_obj && luaL_callmeta(L, 1, "__tostring")) {
               lua_pushcfunction(L, util_print_r_get_first);
               lua_insert(L, -2);
               lua_call(L, 1, 1);
               s += " -- ";
               s += lua_tostring(L, -1);
               lua_pop(L, 1);
            }

            s += " --> ";
            lua_pushcfunction(L, util_print_r_get_first);
            lua_insert(L, -2);
            lua_call(L, 1, 1);
            s += lua_tostring(L, -1);
            lua_pop(L, 1);
         }

         lua_pushstring(L, s.c_str());
         break;
      }

      case LUA_TSTRING:
         lua_getglobal(L, "string");
         lua_getfield(L, -1, "format");
         if (key_obj) {
            lua_pushliteral(L, "[%q]");
         } else {
            lua_pushliteral(L, "%q");
         }
         lua_pushvalue(L, 1);
         lua_call(L, 2, 1);
         break;

      case LUA_TNUMBER:
         if (key_obj) {
            S s = "[";
            s += luaL_tolstring(L, 1, nullptr);
            lua_pop(L, 1);
            s += ']';
            lua_pushstring(L, s.c_str());
         } else {
            luaL_tolstring(L, 1, nullptr);
         }
         break;

      case LUA_TBOOLEAN:
         if (key_obj) {
            if (lua_toboolean(L, 1)) {
               lua_pushliteral(L, "[true]");
            } else {
               lua_pushliteral(L, "[false]");
            }
         } else {
            if (lua_toboolean(L, 1)) {
               lua_pushliteral(L, "true");
            } else {
               lua_pushliteral(L, "false");
            }
         }
         break;

      default:
         if (key_obj) {
            S s = "[";
            s += tstr;
            s += ']';
            lua_pushstring(L, s.c_str());
         } else {
            lua_pushstring(L, tstr);
         }
         break;
   }
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int util_print_r_impl(lua_State* L) {
   // 1  prefix for first line
   // 2  prefix for subsequent lines (tables only)
   // 3  object to print
   // 4  tables already visited

   lua_settop(L, 4);
   bool already_visited = false;
   int type = lua_type(L, 3);
   if (type == LUA_TTABLE) {
      if (lua_type(L, 4) != LUA_TTABLE) {
         lua_createtable(L, 0, 0);
         lua_copy(L, -1, 4);
      }

      lua_pushvalue(L, 3);
      lua_rawget(L, 4);
      already_visited = !lua_isnil(L, -1);
      lua_pop(L, 1);

      // add this table to the already visited table
      lua_pushvalue(L, 3);
      lua_pushboolean(L, 1);
      lua_rawset(L, 4);
   }

   lua_getglobal(L, "print");
   S s = lua_tostring(L, 1);

   lua_pushcfunction(L, util_print_r_get_first);
   lua_pushvalue(L, 3);
   lua_pushboolean(L, already_visited ? 0 : 1); // dont look through metatables if we've already seen this object
   lua_call(L, 2, 1);   // util_print_r_get_first()
   s += lua_tostring(L, -1);
   lua_pop(L, 1);
   lua_pushstring(L, s.c_str());
   lua_call(L, 1, 0);   // print()

   if (type == LUA_TTABLE && !already_visited) {
      lua_pushnil(L);
      while (lua_next(L, 3) != 0) {
         // -2 key
         // -1 value

         s = lua_tostring(L, 2);
         s += "   ";

         // get key string
         if (lua_type(L, -2) == LUA_TSTRING && util_is_valid_varname(lua_tostring(L, -2))) {
            lua_pushvalue(L, -2);
         } else {
            lua_pushcfunction(L, util_print_r_get_first);
            lua_pushvalue(L, -3);   // key
            lua_pushboolean(L, 0);
            lua_call(L, 2, 1);
         }

         size_t extra;
         size_t saved_size = s.size();
         s += lua_tolstring(L, -1, &extra);
         s += " = ";
         extra += 3;
         if (extra > 16) {
            extra = 3;
         }

         char extrabuf[17] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'
         };
         extrabuf[extra] = '\0';

         lua_pop(L, 1); // pop key string
         lua_pushcfunction(L, util_print_r_impl);
         lua_pushstring(L, s.c_str());
         s.resize(saved_size);
         s += extrabuf;
         lua_pushstring(L, s.c_str());
         lua_pushvalue(L, -4);   // value
         lua_pushvalue(L, 4);    // visited elements
         lua_call(L, 4, 0);

         lua_pop(L, 1); // remove value, keep key for next
      }

      lua_getglobal(L, "print");
      s = lua_tostring(L, 2);
      s += '}';
      lua_pushstring(L, s.c_str());
      lua_call(L, 1, 0);
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////////
int util_sprint_r_impl(lua_State* L) {
   // 1  prefix for first line
   // 2  prefix for subsequent lines (tables only)
   // 3  object to print
   // 4  tables already visited

   lua_settop(L, 4);
   bool already_visited = false;
   int type = lua_type(L, 3);
   if (type == LUA_TTABLE) {
      if (lua_type(L, 4) != LUA_TTABLE) {
         lua_createtable(L, 0, 0);
         lua_copy(L, -1, 4);
      }

      lua_pushvalue(L, 3);
      lua_rawget(L, 4);
      already_visited = !lua_isnil(L, -1);
      lua_pop(L, 1);

      // add this table to the already visited table
      lua_pushvalue(L, 3);
      lua_pushboolean(L, 1);
      lua_rawset(L, 4);
   }

   
   S output = lua_tostring(L, 1);

   lua_pushcfunction(L, util_print_r_get_first);
   lua_pushvalue(L, 3);
   lua_pushboolean(L, already_visited ? 0 : 1); // dont look through metatables if we've already seen this object
   lua_call(L, 2, 1);   // util_print_r_get_first()
   output += lua_tostring(L, -1);
   lua_pop(L, 1);

   output.append(1, '\n');

   if (type == LUA_TTABLE && !already_visited) {
      lua_pushnil(L);
      while (lua_next(L, 3) != 0) {
         // -2 key
         // -1 value

         S temp = lua_tostring(L, 2);
         temp += "   ";

         // get key string
         if (lua_type(L, -2) == LUA_TSTRING && util_is_valid_varname(lua_tostring(L, -2))) {
            lua_pushvalue(L, -2);
         } else {
            lua_pushcfunction(L, util_print_r_get_first);
            lua_pushvalue(L, -3);   // key
            lua_pushboolean(L, 0);
            lua_call(L, 2, 1);
         }

         size_t extra;
         size_t saved_size = temp.size();
         temp += lua_tolstring(L, -1, &extra);
         temp += " = ";
         extra += 3;
         if (extra > 16) {
            extra = 3;
         }

         char extrabuf[17] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'
         };
         extrabuf[extra] = '\0';

         lua_pop(L, 1); // pop key string
         lua_pushcfunction(L, util_sprint_r_impl);
         lua_pushstring(L, temp.c_str());
         temp.resize(saved_size);
         temp += extrabuf;
         lua_pushstring(L, temp.c_str());
         lua_pushvalue(L, -4);   // value
         lua_pushvalue(L, 4);    // visited elements
         lua_call(L, 4, 1);
         output.append(lua_tostring(L, -1));
         lua_pop(L, 2); // remove string, value, keep key for next
      }

      output.append(lua_tostring(L, 2));
      output.append(1, '}');
   }

   lua_pushstring(L, output.c_str());

   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int util_print_r(lua_State* L) {
   int count = lua_gettop(L);

   lua_pushcfunction(L, util_print_r_impl);
   for (int i = 1; i <= count; ++i) {
      lua_pushvalue(L, -1);   // copy util_print_r_impl
      lua_pushliteral(L, ""); // prefix first line with this
      lua_pushvalue(L, -1);   // prefix subsequent lines with this
      lua_pushvalue(L, i);    // the actual value to print
      lua_call(L, 3, 0);
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////
int util_sprint_r(lua_State* L) {
   int count = lua_gettop(L);

   lua_pushcfunction(L, util_sprint_r_impl);
   for (int i = 1; i <= count; ++i) {
      lua_pushvalue(L, -1);   // copy util_print_r_impl
      lua_pushliteral(L, ""); // prefix first line with this
      lua_pushvalue(L, -1);   // prefix subsequent lines with this
      lua_pushvalue(L, i);    // the actual value to print
      lua_call(L, 3, 1);
   }
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts an iterator to varargs
int util_all(lua_State* L) {
   lua_settop(L, 4);
   // 1 - iterator function
   // 2 - static state
   // 3 - variable state
   // 4 - if not nil, all values are combined into the varargs, not just the variable state.

   int retvals = lua_isnil(L, 4) ? 1 : LUA_MULTRET;
   lua_pop(L, 1); // remove 4

   while (lua_checkstack(L, 3)) {
      int top = lua_gettop(L);
      lua_pushvalue(L, 1);
      lua_pushvalue(L, 2);
      lua_pushvalue(L, 3);
      lua_call(L, 2, retvals);
      int nresults = lua_gettop(L) - top;
      if (nresults == 0 || lua_isnil(L, top + 1)) {
         break;
      }
      lua_copy(L, top + 1, 3);
   }
   return lua_gettop(L) - 3;
}

///////////////////////////////////////////////////////////////////////////////
int util_seed(lua_State* L) {
   lua_settop(L, 0);
   lua_pushinteger(L, (lua_Integer)std::chrono::system_clock::now().time_since_epoch().count());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int util_deflate(lua_State* L) {
   std::size_t len;
   const char* ptr = luaL_checklstring(L, 1, &len);

   I8 level = 8;
   if (lua_gettop(L) > 1) {
      level = (I8)luaL_checkinteger(L, 2);
   }

   Buf<const UC> data = make_buf(ptr, len);
   Buf<const char> compressed(util::deflate_blob(data, false, level));
   lua_pushlstring(L, compressed.get(), compressed.size());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int util_inflate(lua_State* L) {
   std::size_t len;
   const char* ptr = luaL_checklstring(L, 1, &len);
   std::size_t uncompressed_length = luaL_checkinteger(L, 2);
   Buf<const UC> data = make_buf(ptr, len);
   Buf<const char> uncompressed(util::inflate_blob(data, uncompressed_length));
   lua_pushlstring(L, uncompressed.get(), uncompressed.size());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int util_require_load(lua_State* L) {
   std::size_t len;
   const char* ptr = luaL_checklstring(L, 1, &len);
   const char* name = luaL_checkstring(L, 2);

   int result = luaL_loadbufferx(L, ptr, len, name, "bt");
   if (result != LUA_OK) {
      lua_error(L);
   }
   return 1;
}

} // be::belua::()

///////////////////////////////////////////////////////////////////////////////
int open_util(lua_State* L) {
   luaL_Reg fn[] {
      { "print_r", util_print_r },
      { "sprint_r", util_sprint_r },
      { "all", util_all },
      { "seed", util_seed },
      { "deflate", util_deflate },
      { "inflate", util_inflate },
      { "require_load", util_require_load },
      { nullptr, nullptr }
   };

   luaL_newlib(L, fn);

   return 1;
}

} // be::belua
