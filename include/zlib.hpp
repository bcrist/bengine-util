#pragma once
#ifndef BE_UTIL_COMPRESSION_ZLIB_HPP_
#define BE_UTIL_COMPRESSION_ZLIB_HPP_

#include "util_compression_autolink.hpp"
#include <be/core/buf.hpp>

namespace be::util {

Buf<UC> deflate_text(const S& text, bool encode_length = true, I8 level = 9);
Buf<UC> deflate_blob(const Buf<const UC>& data, bool encode_length = true, I8 level = 9);

S inflate_text(const Buf<const UC>& compressed);
S inflate_text(const Buf<const UC>& compressed, std::size_t uncomressed_length);
Buf<UC> inflate_blob(const Buf<const UC>& compressed);
Buf<UC> inflate_blob(const Buf<const UC>& compressed, std::size_t uncomressed_length);

} // be::util

#endif
