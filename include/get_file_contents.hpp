#pragma once
#ifndef BE_UTIL_FS_GET_FILE_CONTENTS_HPP_
#define BE_UTIL_FS_GET_FILE_CONTENTS_HPP_

#include "util_fs_autolink.hpp"
#include <be/core/filesystem.hpp>
#include <be/core/buf.hpp>

namespace be::util {

S get_file_contents_string(FILE* fd);
S get_file_contents_string(FILE* fd, std::error_code& ec) noexcept;
Buf<UC> get_file_contents_buf(FILE* fd);
Buf<UC> get_file_contents_buf(FILE* fd, std::error_code& ec) noexcept;

S get_file_contents_string(const Path& path);
S get_file_contents_string(const Path& path, std::error_code& ec) noexcept;
Buf<UC> get_file_contents_buf(const Path& path);
Buf<UC> get_file_contents_buf(const Path& path, std::error_code& ec) noexcept;

} // be::util

#endif
