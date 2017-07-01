#pragma once
#ifndef BE_UTIL_STRING_PARSE_NUMERIC_STRING_HPP_
#define BE_UTIL_STRING_PARSE_NUMERIC_STRING_HPP_

#include "string_span.hpp"
#include <be/core/alg.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, std::error_code& ec) noexcept;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, I32 radix);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, I32 radix, std::error_code& ec) noexcept;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, std::error_code& ec) noexcept;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix);

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix, std::error_code& ec) noexcept;

} // be::util

#include "parse_numeric_string.inl"

#endif
