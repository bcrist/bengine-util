#pragma once
#ifndef BE_UTIL_FS_PUT_FILE_CONTENTS_HPP_
#define BE_UTIL_FS_PUT_FILE_CONTENTS_HPP_

#include <be/core/filesystem.hpp>
#include <be/core/buf.hpp>

namespace be::util {

void put_file_contents(const Path& path, const S& contents);
void put_file_contents(const Path& path, const S& contents, std::error_code& ec) noexcept;
void put_file_contents(const Path& path, const Buf<const UC>& contents);
void put_file_contents(const Path& path, const Buf<const UC>& contents, std::error_code& ec) noexcept;

} // be::util

#endif
