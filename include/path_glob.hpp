#pragma once
#ifndef BE_UTIL_FS_PATH_GLOB_HPP_
#define BE_UTIL_FS_PATH_GLOB_HPP_

#include "util_fs_autolink.hpp"
#include "paths.hpp"
#include <be/core/logging.hpp>
#include <regex>

// TODO replace with directory walker visitors

namespace be::util {

enum class PathMatchType : U8 {
   none = 0,
   files = 1,
   directories = 2,
   files_and_dirs = 3,
   misc = 4,
   files_and_misc = 5,
   dirs_and_misc = 6,
   all = 7,
   recursive = 8,
   recursive_files = 9,
   recursive_directories = 10,
   recursive_files_and_dirs = 11,
   recursive_misc = 12,
   recursive_files_and_misc = 13,
   recursive_dirs_and_misc = 14,
   recursive_all = 15
};

namespace detail {

template <typename I>
void greb_helper(std::vector<Path>& paths, const Path& p, I begin, I end, PathMatchType match_type);

template <typename I>
std::vector<Path> recurse_directories(I begin, I end);

} // be::util::detail

template <typename I>
std::vector<Path> glob(const S& pattern, I begin, I end, PathMatchType match_type = PathMatchType::all);
template <typename C>
std::vector<Path> glob(const S& pattern, const C& search_paths, PathMatchType match_type = PathMatchType::all);
std::vector<Path> glob(const S& pattern, const Path& search_path, PathMatchType match_type = PathMatchType::all);
std::vector<Path> glob(const S& pattern, PathMatchType match_type = PathMatchType::all);

template <typename I>
std::vector<Path> greb(const S& pattern, I begin, I end, PathMatchType match_type = PathMatchType::all);
template <typename C>
std::vector<Path> greb(const S& pattern, const C& search_paths, PathMatchType match_type = PathMatchType::all);
std::vector<Path> greb(const S& pattern, const Path& search_path, PathMatchType match_type = PathMatchType::all);
std::vector<Path> greb(const S& pattern, PathMatchType match_type = PathMatchType::all);

} // be::util

#include "path_glob.inl"

#endif
