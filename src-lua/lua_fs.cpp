#include "lua_fs.hpp"
#include <be/core/filesystem.hpp>
#include <be/util/paths.hpp>
#include <be/util/path_glob.hpp>
#include <be/util/files.hpp>

namespace be {
namespace belua {

///////////////////////////////////////////////////////////////////////////////
const luaL_Reg fs_module { "be.fs", open_fs };

namespace {

///////////////////////////////////////////////////////////////////////////////
int fs_path_combine(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   I32 last = lua_gettop(L);

   for (I32 i = 2; i <= last; ++i) {
      p /= Path(luaL_checkstring(L, i));
   }

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_parent(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   p = p.parent_path();

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_ancestor_relative(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   Path ancestor(luaL_checkstring(L, 2));

   auto it = p.begin(), end = p.end();
   auto ait = ancestor.begin(), aend = ancestor.end();

   while (it != end && ait != aend && *it == *ait) {
      ++it;
      ++ait;
   }

   Path out;
   if (ait != aend) {
      out = p;
   } else {
      while (it != end) {
         out /= *it;
         ++it;
      }
   }

   lua_pushstring(L, out.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_is_dir(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   lua_pushboolean(L, fs::is_directory(p));
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_create_dirs(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   lua_pushboolean(L, fs::create_directories(p));
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_stem(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   p = p.stem();

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_extension(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   p = p.extension();

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_replace_extension(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   Path ext(luaL_checkstring(L, 2));
   p.replace_extension(ext);

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_filename(lua_State* L) {
   Path p(luaL_checkstring(L, 1));
   p = p.filename();

   lua_pushstring(L, p.string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_file_mtime(lua_State* L) {
   auto time_pt = fs::last_write_time(luaL_checkstring(L, 1));
   time_t mtime = decltype(time_pt)::clock::to_time_t(time_pt);
   lua_pushinteger(L, lua_Integer(mtime));
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_remove(lua_State* L) {
   lua_pushboolean(L, fs::remove(luaL_checkstring(L, 1)) ? 1 : 0);
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_canonical(lua_State* L) {   
   lua_pushstring(L, fs::canonical(luaL_checkstring(L, 1)).string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_absolute(lua_State* L) {   
   lua_pushstring(L, fs::absolute(luaL_checkstring(L, 1)).string().c_str());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_exists(lua_State* L) {
   lua_pushboolean(L, fs::exists(luaL_checkstring(L, 1)) ? 1 : 0);
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_path_equivalent(lua_State* L) {
   std::size_t len;
   const char* ptr = luaL_checklstring(L, 1, &len);
   S a(ptr, len);
   ptr = luaL_checklstring(L, 2, &len);
   S b(ptr, len);

   lua_pushboolean(L, fs::equivalent(a, b) ? 1 : 0);
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_root_path(lua_State* L) {   
   Path p(luaL_checkstring(L, 1));
   p = p.root_path();

   lua_pushstring(L, p.string().c_str());
   return 1;
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
// return names of all non-directory files in the specified directory, or cwd if no dir specified
int fs_get_files(lua_State* L) {
   Path p;
   int count = 0;
   if (lua_gettop(L) == 0) {
      p = fs::current_path();
   } else {
      p = luaL_checkstring(L, 1);
   }

   try {
      if (fs::exists(p) && fs::is_directory(p)) {
         for (fs::directory_iterator i(p), end; i != end; ++i) {
            const Path& ipath = i->path();
            if (fs::is_regular_file(ipath)) {
               lua_checkstack(L, 1);
               lua_pushstring(L, ipath.filename().string().c_str());
               ++count;
            }
         }
      } else {
         return luaL_error(L, "Specified path is not a directory!");
      }
   } catch (fs::filesystem_error& e) {
      return luaL_error(L, e.what());
   }
   return count;
}

///////////////////////////////////////////////////////////////////////////////
// return names of all directories in the specified directory, or cwd if no dir specified
int fs_get_dirs(lua_State* L) {
   Path p;
   int count = 0;
   if (lua_gettop(L) == 0) {
      p = fs::current_path();
   } else {
      p = luaL_checkstring(L, 1);
   }

   try {
      if (fs::exists(p) && fs::is_directory(p)) {
         for (fs::directory_iterator i(p), end; i != end; ++i) {
            const fs::path& ipath = i->path();
            if (fs::is_directory(ipath)) {
               lua_checkstack(L, 1);
               lua_pushstring(L, ipath.filename().string().c_str());
               ++count;
            }
         }
      } else {
         return luaL_error(L, "Specified path is not a directory!");
      }
   } catch (fs::filesystem_error& e) {
      return luaL_error(L, e.what());
   }
   return count;
}

///////////////////////////////////////////////////////////////////////////////
int fs_find_file(lua_State* L) {
   Path filename(luaL_checkstring(L, 1));

   std::vector<Path> search_paths;

   I32 last = lua_gettop(L);
   for (I32 i = 2; i <= last; ++i) {
      util::parse_multi_path(luaL_checkstring(L, i), search_paths);
   }

   if (search_paths.empty()) {
      search_paths.push_back(util::cwd());
   }

   filename = util::find_file(filename, search_paths);
   if (filename.empty()) {
      return 0;
   } else {
      lua_pushstring(L, filename.string().c_str());
      return 1;
   }
}

///////////////////////////////////////////////////////////////////////////////
int fs_glob(lua_State* L) {
   std::size_t len;
   const char* ptr = luaL_checklstring(L, 1, &len);
   S pattern(ptr, len);

   std::vector<Path> search_paths;

   if (!lua_isnil(L, 2)) {
      if (lua_type(L, 2) == LUA_TTABLE) {
         lua_pushnil(L);
         while (lua_next(L, 2) != 0) {
            ptr = luaL_tolstring(L, -1, &len);
            S str(ptr, len);
            lua_pop(L, 2);
            util::parse_multi_path(str, search_paths);
         }
      } else {
         ptr = luaL_tolstring(L, 2, &len);
         S str(ptr, len);
         lua_pop(L, 1);
         util::parse_multi_path(str, search_paths);
      }
   }

   if (search_paths.empty()) {
      search_paths.push_back(util::cwd());
   }

   util::PathMatchType type = util::PathMatchType::all;

   if (!lua_isnil(L, 3)) {
      U8 type_mask = 0;
      ptr = lua_tolstring(L, 3, &len);
      S typestr(ptr, len);
      if (std::find(typestr.begin(), typestr.end(), 'f') != typestr.end()) {
         type_mask |= static_cast<U8>(util::PathMatchType::files);
      }
      if (std::find(typestr.begin(), typestr.end(), 'd') != typestr.end()) {
         type_mask |= static_cast<U8>(util::PathMatchType::directories);
      }
      if (std::find(typestr.begin(), typestr.end(), '?') != typestr.end()) {
         type_mask |= static_cast<U8>(util::PathMatchType::misc);
      }
      if (std::find(typestr.begin(), typestr.end(), 'r') != typestr.end()) {
         type_mask |= static_cast<U8>(util::PathMatchType::recursive);
      }
      type = static_cast<util::PathMatchType>(type_mask);
   }

   std::vector<Path> paths = util::glob(pattern, search_paths, type);

   lua_settop(L, 0);
   if (paths.size() > (std::size_t)std::numeric_limits<int>::max() || !lua_checkstack(L, (int)paths.size())) {
      luaL_error(L, "Too many paths to return on stack!");
      // TODO consider returning a table (sequence) instead to avoid this issue
   }

   for (Path& p : paths) {
      S str = p.string();
      lua_pushlstring(L, str.c_str(), str.length());
   }

   return (int)paths.size();
}

///////////////////////////////////////////////////////////////////////////////
int fs_get_file_contents(lua_State* L) {
   Path filename(luaL_checkstring(L, 1));

   S data = util::get_file_contents_string(filename);

   lua_pushlstring(L, data.c_str(), data.length());
   return 1;
}

///////////////////////////////////////////////////////////////////////////////
int fs_put_file_contents(lua_State* L) {
   Path filename(luaL_checkstring(L, 1));
   std::size_t len;
   const char* input = luaL_tolstring(L, 2, &len);

   util::put_file_contents(filename, S(input, len));
   return 0;
}

} // be::belua::()

///////////////////////////////////////////////////////////////////////////////
int open_fs(lua_State* L) {
   luaL_Reg fn[] {
      { "compose_path", fs_path_combine },
      { "parent_path", fs_path_parent },
      { "ancestor_relative", fs_ancestor_relative },
      { "is_directory", fs_is_dir },
      { "create_dirs", fs_create_dirs },
      { "path_stem", fs_path_stem },
      { "path_extension", fs_path_extension },
      { "replace_extension", fs_replace_extension },
      { "path_filename", fs_path_filename },
      { "remove_path", fs_path_remove },
      { "canonical", fs_path_canonical },
      { "absolute", fs_path_absolute },
      { "exists", fs_path_exists },
      { "equivalent", fs_path_equivalent },
      { "root_path", fs_root_path },
      { "get_files", fs_get_files },
      { "get_dirs", fs_get_dirs },
      { "file_mtime", fs_file_mtime },
      { "find_file", fs_find_file },
      { "glob", fs_glob },
      { "get_file_contents", fs_get_file_contents },
      { "put_file_contents", fs_put_file_contents },
      { nullptr, nullptr }
   };

   luaL_newlib(L, fn);
   return 1;
}

} // be::belua
} // be
