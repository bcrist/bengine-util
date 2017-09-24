#pragma once
#ifndef BE_UTIL_STRING_BASE64_DECODE_HPP_
#define BE_UTIL_STRING_BASE64_DECODE_HPP_

#include <be/core/buf.hpp>

namespace be::util {

template <char S62 = '+', char S63 = '/', char P = '='>
S base64_decode_string(SV encoded_data);

template <char S62 = '+', char S63 = '/', char P = '='>
Buf<UC> base64_decode_buf(SV encoded_data);

} // be::util

#include "base64_decode.inl"

#endif
