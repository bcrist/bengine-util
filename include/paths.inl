#if !defined(BE_UTIL_FS_PATHS_HPP_) && !defined(DOXYGEN)
#include "paths.hpp"
#elif !defined(BE_UTIL_FS_PATHS_INL_)
#define BE_UTIL_FS_PATHS_INL_

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename I>
Path find_file(const Path& filename, I begin, I end) {
   if (filename.is_absolute() && fs::exists(filename) && !fs::is_directory(filename)) {
      return fs::canonical(filename);
   }

   Path search = filename.relative_path();

   if (!search.has_filename()) {
      return Path();
   }

   while (begin != end) {
      Path p = fs::absolute(*begin);
      p /= search;

      if (fs::exists(p) && !fs::is_directory(p)) {
         return fs::canonical(p);
      }
      ++begin;
   }

   return Path();
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
Path find_file(const Path& filename, const C& search_paths) {
   return find_file(filename, std::begin(search_paths), std::end(search_paths));
}

} // be::util

#endif
