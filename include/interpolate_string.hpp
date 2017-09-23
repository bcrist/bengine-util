#pragma once
#ifndef BE_UTIL_STRING_INTERPOLATE_STRING_HPP_
#define BE_UTIL_STRING_INTERPOLATE_STRING_HPP_

#include <be/core/be.hpp>

namespace be::util {

template <typename F, char Sigil = '$'>
S interpolate_string(SV source, F func = F());

template <typename G, typename F, char Sigil = '$'>
void interpolate_string_ex(SV source, G noninterp_func = G(), F interp_func = F());

} // be::util

#include "interpolate_string.inl"

#endif
