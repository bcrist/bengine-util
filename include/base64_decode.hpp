#pragma once
#ifndef BE_UTIL_BASE64_DECODE_HPP_
#define BE_UTIL_BASE64_DECODE_HPP_

#include <be/core/buf.hpp>
#include <gsl/string_span>

namespace be::util {

template <char S62 = '+', char S63 = '/', char P = '='>
S base64_decode_string(gsl::cstring_span<> encoded_data);

template <char S62 = '+', char S63 = '/', char P = '='>
Buf<UC> base64_decode_buf(gsl::cstring_span<> encoded_data);

} // be::util

#include "base64_decode.inl"

#endif
