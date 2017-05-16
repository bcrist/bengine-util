#pragma once
#ifndef BE_UTIL_PARSE_NUMERIC_STRING_HPP_
#define BE_UTIL_PARSE_NUMERIC_STRING_HPP_

#include "string_span.hpp"
#include "parse_string_error.hpp"
#include <be/core/alg.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, std::pair<T, ParseStringError>>
parse_numeric_string(gsl::cstring_span<> value);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, std::pair<T, ParseStringError>>
parse_numeric_string(gsl::cstring_span<> value, I32 radix);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, std::pair<T, ParseStringError>>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, std::pair<T, ParseStringError>>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix);

} // be::util

#include "parse_numeric_string.inl"

#endif
