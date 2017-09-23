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
      log_exception(e);
   }
}

///////////////////////////////////////////////////////////////////////////////
inline void recurse_directories(const Path& parent, std::vector<Path>& out) {
   typedef fs::directory_iterator DIter;
   try {
      auto stat = fs::status(parent);
      if (fs::exists(stat) && fs::is_directory(stat)) {
         out.push_back(parent);

         for (DIter i(parent), end; i != end; ++i) {
            const Path& child = i->path();
            recurse_directories(child, out);
         }
      }
   } catch (const fs::filesystem_error& e) {
      log_exception(e);
   }
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
      std::vector<Path> parents;
      while (begin != end) {
         detail::recurse_directories(*begin, parents);
         ++begin;
      }

      paths = greb(pattern, parents, (PathMatchType)((U8)match_type & (U8)PathMatchType::all));
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
