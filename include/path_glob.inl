#if !defined(BE_UTIL_FS_PATH_GLOB_HPP_) && !defined(DOXYGEN)
#include "path_glob.hpp"
#elif !defined(BE_FS_UTIL_PATH_GLOB_INL_)
#define BE_FS_UTIL_PATH_GLOB_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename I>
void greb_helper(std::vector<Path>& paths, const Path& p, I begin, I end, PathMatchType match_type) {
   try {
      if (begin == end) {
         if (!fs::exists(p)) {
            return;
         }

         switch (fs::status(p).type()) {
            case fs::file_type::directory:
               if (((U8)match_type & (U8)PathMatchType::directories)) {
                  paths.push_back(p);
               }
               break;

            case fs::file_type::regular:
               if (((U8)match_type & (U8)PathMatchType::files)) {
                  paths.push_back(p);
               }
               break;

            default:
               if (((U8)match_type & (U8)PathMatchType::misc)) {
                  paths.push_back(p);
               }
               break;
         }
         return;
      }

      S& src = begin->first;
      std::regex& regex = begin->second;
      I next = begin;
      ++next;

      if (src == "\\.") {
         greb_helper(paths, p, next, end, match_type);
      } else if (src == "\\.\\.") {
         greb_helper(paths, p.parent_path(), next, end, match_type);
      } else if (fs::is_directory(p)) {
         typedef fs::directory_iterator DIter;
         for (DIter it(p), ite; it != ite; ++it) {
            Path newp = it->path();

            if (!newp.empty()) {
               S last_elem = (--newp.end())->string();
               if (std::regex_match(last_elem, regex)) {
                  greb_helper(paths, newp, next, end, match_type);
               }
            }
         }
      }
   } catch (const fs::filesystem_error& e) {
      be_notice() << "Filesystem Error"
         & attr("Error Code") << e.code()
         & attr("Path 1") << e.path1()
         & attr("Path 2") << e.path2()
         | default_log();
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename I>
std::vector<Path> recurse_directories(I begin, I end) {
   typedef fs::directory_iterator DIter;

   std::vector<Path> dirs(begin, end);
   std::vector<Path> newdirs;

   for (I it = begin; it != end; ++it) {
      try {
         const Path& p = *it;
         if (fs::exists(p) && fs::is_directory(p)) {
            for (DIter i(p), ie; i != ie; ++i) {
               const Path& newp = i->path();

               if (is_directory(newp)) {
                  newdirs.push_back(newp);
               }
            }
         }
      } catch (const fs::filesystem_error& e) {
         be_notice() << "Filesystem Error"
            & attr("Error Code") << e.code()
            & attr("Path 1") << e.path1()
            & attr("Path 2") << e.path2()
            | default_log();
      }
   }
   
   if (!newdirs.empty()) {
      newdirs = recurse_directories(newdirs.begin(), newdirs.end());
      dirs.reserve(dirs.size() + newdirs.size());
      for (Path& p : newdirs) {
         dirs.push_back(std::move(p));
      }
   }

   return dirs;
}

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <typename I>
std::vector<Path> glob(const S& pattern, I begin, I end, PathMatchType match_type) {
   S expanded_pattern = expand_path(pattern);
   S greb_pattern;
   greb_pattern.reserve(expanded_pattern.size() * 2);

   bool escaped = false;
   bool cclass = false;

   for (char c : expanded_pattern) {
      switch (c) {
         case '%':
            escaped = !escaped;
            if (!escaped) {
               greb_pattern.append("%");
            }
            continue;

         case '\\':
            if (escaped) {
               greb_pattern.append("\\\\");
            } else if (cclass) {
               greb_pattern.append(1, '\\');
            } else {
               greb_pattern.append(1, '/');
            }
            break;

         case '^':
         case '$':
         case '+':
         case '.':
         case '(':
         case ')':
         case '|':
         case '{':
         case '}':
            if (!cclass || escaped) {
               greb_pattern.append(1, '\\');
            }
            greb_pattern.append(1, c);
            break;

         case '[':
            if (escaped || cclass) {
               greb_pattern.append("\\[");
               break;
            }
            cclass = true;
            greb_pattern.append(1, '[');
            break;

         case ']':
            if (escaped || !cclass) {
               greb_pattern.append("\\]");
               break;
            }
            cclass = false;
            greb_pattern.append(1, ']');
            break;

         case '!':
            if (escaped || !cclass) {
               greb_pattern.append(1, '!');
            } else {
               greb_pattern.append(1, '^');
            }
            break;

         case '?':
            if (escaped) {
               greb_pattern.append("\\?");
            } else if (cclass) {
               greb_pattern.append(1, '?');
            } else {
               greb_pattern.append(1, '.');
            }
            break;

         case '*':
            if (escaped) {
               greb_pattern.append("\\*");
            } else if (cclass) {
               greb_pattern.append(1, '*');
            } else {
               greb_pattern.append(".*");
            }
            break;

         default:
            greb_pattern.append(1, c);
            break;
      }

      escaped = false;
   }

   return greb(greb_pattern, begin, end, match_type);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
std::vector<Path> glob(const S& pattern, const C& search_paths, PathMatchType match_type) {
   return glob(pattern, std::begin(search_paths), std::end(search_paths), match_type);
}

///////////////////////////////////////////////////////////////////////////////
template <typename I>
std::vector<Path> greb(const S& pattern, I begin, I end, PathMatchType match_type) {
   std::vector<Path> paths;

   if ((U8)match_type & (U8)PathMatchType::recursive) {
      paths = greb(pattern, detail::recurse_directories(begin, end), (PathMatchType)((U8)match_type & (U8)PathMatchType::all));
   } else {
      std::vector<std::pair<S, std::regex>> regexen;

      for (auto it1(pattern.begin()), it2(it1), ite(pattern.end()); it1 != ite; it1 = it2) {
         it2 = std::find(it1, ite, '/');
         S src(it1, it2);

         if (src.empty()) {
            src = "\\.";
         }

         regexen.push_back(std::make_pair(src, std::regex(src)));

         if (it2 != ite) {
            ++it2; // skip the delimiter
         }
      }

      while (begin != end) {
         detail::greb_helper(paths, fs::absolute(*begin), regexen.begin(), regexen.end(), match_type);
         ++begin;
      }
   }

   return paths;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
std::vector<Path> greb(const S& pattern, const C& search_paths, PathMatchType match_type) {
   return greb(pattern, std::begin(search_paths), std::end(search_paths), match_type);
}

} // be::util

#endif
