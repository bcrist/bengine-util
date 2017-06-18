#pragma once
#ifndef BE_UTIL_FS_FILES_HPP_
#define BE_UTIL_FS_FILES_HPP_

#include "util_fs_autolink.hpp"
#include "file_read_error.hpp"
#include "file_write_error.hpp"
#include <be/core/be.hpp>
#include <be/core/filesystem.hpp>
#include <be/core/buf.hpp>

namespace be::util {

std::pair<S, FileReadError> get_file_contents_string(FILE* fd);
std::pair<S, FileReadError> get_file_contents_string(const Path& path);
std::pair<Buf<UC>, FileReadError> get_file_contents_buf(FILE* fd);
std::pair<Buf<UC>, FileReadError> get_file_contents_buf(const Path& path);

FileWriteError put_file_contents(const Path& path, const S& contents);
FileWriteError put_file_contents(const Path& path, const Buf<const UC>& contents);

} // be::util

#endif
