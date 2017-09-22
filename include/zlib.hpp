#pragma once
#ifndef BE_UTIL_COMPRESSION_ZLIB_HPP_
#define BE_UTIL_COMPRESSION_ZLIB_HPP_

#include <be/core/buf.hpp>

namespace be::util {

Buf<UC> deflate_string(const S& text, bool encode_length = true, I8 level = 7);
Buf<UC> deflate_string(const S& text, std::error_code& ec, bool encode_length = true, I8 level = 7) noexcept;
Buf<UC> deflate_buf(const Buf<const UC>& data, bool encode_length = true, I8 level = 7);
Buf<UC> deflate_buf(const Buf<const UC>& data, std::error_code& ec, bool encode_length = true, I8 level = 7) noexcept;

S inflate_string(const Buf<const UC>& compressed);
S inflate_string(const Buf<const UC>& compressed, std::error_code& ec) noexcept;
S inflate_string(const Buf<const UC>& compressed, std::size_t uncomressed_length);
S inflate_string(const Buf<const UC>& compressed, std::size_t uncomressed_length, std::error_code& ec) noexcept;
Buf<UC> inflate_buf(const Buf<const UC>& compressed);
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::error_code& ec) noexcept;
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::size_t uncomressed_length);
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::size_t uncomressed_length, std::error_code& ec) noexcept;

} // be::util

#endif
