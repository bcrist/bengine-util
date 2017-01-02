#pragma once
#ifndef BE_UTIL_PATHS_HPP_
#define BE_UTIL_PATHS_HPP_

#include "util_autolink.hpp"
#include <be/core/be.hpp>
#include <be/core/filesystem.hpp>

namespace be {
namespace util {

enum class SpecialPath : U8 {
   temp,
   install,
   process_image,
   default_install,
   local_profile_data,
   roaming_profile_data,
   device_local_data,
   file_export,
   screenshots,
   recordings,
   saved_games,
   documents,
   downloads,
   pictures,
   videos,
   music,
   desktop
};

Path cwd();
void cwd(const Path& path, bool create_path = false);

Path default_special_path(SpecialPath type);
Path special_path(SpecialPath type);

Path random_path(const Path& model = ".%%%%%%%%%%%%%%%%", const Path& symbols = "0123456789abcdefghijklmnopqrstuvwxyz");

S get_env(const S& name);

S expand_path(const S& path, bool use_config = true);
Path parse_path(const S& path);
std::vector<Path> parse_multi_path(const S& multi_path);
void parse_multi_path(const S& multi_path, std::vector<Path>& out);

template <typename I>
Path find_file(const Path& filename, I begin, I end);
template <typename C>
Path find_file(const Path& filename, const C& search_paths);

} // namespace be::util
} // namespace be

#include BE_NATIVE(util, paths.hpp)

#include "paths.inl"

#endif
