#include "pch.hpp"
#include "path_glob.hpp"
#include <be/core/filesystem.hpp>
#include <regex>

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
std::vector<Path> glob(const S& pattern, const Path& search_path, PathMatchType match_type) {
   return glob(pattern, &search_path, &search_path + 1, match_type);
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Path> glob(const S& pattern, PathMatchType match_type) {
   return glob(pattern, fs::current_path(), match_type);
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Path> greb(const S& pattern, const Path& search_path, PathMatchType match_type) {
   return greb(pattern, &search_path, &search_path + 1, match_type);
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Path> greb(const S& pattern, PathMatchType match_type) {
   return greb(pattern, fs::current_path(), match_type);
}

} // be::util
} // be
