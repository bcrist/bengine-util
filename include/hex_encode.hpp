#pragma once
#ifndef BE_UTIL_STRING_HEX_ENCODE_HPP_
#define BE_UTIL_STRING_HEX_ENCODE_HPP_

#include <be/core/buf.hpp>

namespace be::util {

S hex_encode(const Buf<const UC>& data, bool lower_case = false);
S hex_encode(SV data, bool lower_case = false);

} // be::util

#endif
