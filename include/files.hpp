#pragma once
#ifndef BE_UTIL_FILES_HPP_
#define BE_UTIL_FILES_HPP_

#include "util_autolink.hpp"
#include <be/core/be.hpp>
#include <be/core/filesystem.hpp>
#include <be/core/buf.hpp>

namespace be {
namespace util {

S get_file_contents_string(FILE* fd);
S get_file_contents_string(const Path& path);
Buf<UC> get_file_contents_buf(FILE* fd);
Buf<UC> get_file_contents_buf(const Path& path);

void put_file_contents(const Path& path, const S& contents);
void put_file_contents(const Path& path, const Buf<const UC>& contents);

} // be::util
} // be

#endif
