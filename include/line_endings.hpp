#pragma once
#ifndef BE_UTIL_STRING_LINE_ENDINGS_HPP_
#define BE_UTIL_STRING_LINE_ENDINGS_HPP_

#include <be/core/be.hpp>

namespace be::util {

void normalize_newlines(S& string);
S normalize_newlines_copy(SV string);

// TODO?
//void platform_preferred_newlines(S& string);
//S platform_preferred_newlines_copy(SV string);

} // be::util

#endif
