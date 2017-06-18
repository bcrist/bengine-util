#pragma once
#ifndef BE_UTIL_STRING_BASE64_ENCODE_HPP_
#define BE_UTIL_STRING_BASE64_ENCODE_HPP_

#include <be/core/buf.hpp>

namespace be::util {

template <char S62 = '+', char S63 = '/', char P = '='>
S base64_encode(const Buf<const UC>& data);

template <char S62 = '+', char S63 = '/', char P = '='>
S base64_encode(const S& data);

} // be::util

#include "base64_encode.inl"

#endif
