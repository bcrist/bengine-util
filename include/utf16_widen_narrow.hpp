#pragma once
#ifndef BE_UTIL_STRING_UTF16_WIDEN_NARROW_HPP_
#define BE_UTIL_STRING_UTF16_WIDEN_NARROW_HPP_

#include "util_string_autolink.hpp"
#include <be/core/be.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  No-op conversion from UTF-16 to UTF-16.
///
/// \details Only exists to simplify converting templated types to UTF-16.
std::u16string widen(std::u16string source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a S containing UTF-8 text to UTF-16.
std::u16string widen(const S& source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a C-string containing UTF-8 to a UTF-16 std::u16string.
///
/// \details Conversion will stop at the first '\0' character.
std::u16string widen(const char* source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a char buffer containing UTF-8 text to UTF-16.
///
/// \details Any '\0' characters encountered will be included in the output
///         and will not cause the conversion to end.
///
/// \param  source The char buffer to convert.
/// \param  length The size of the buffer in bytes. (not codepoints!)
std::u16string widen(const char* source, std::size_t length);

///////////////////////////////////////////////////////////////////////////////
/// \brief  No-op conversion from UTF-8 to UTF-8.
///
/// \details Only exists to simplify converting templated types to UTF-8.
S narrow(S source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a u16string containing UTF-16 to a UTF-8 string.
S narrow(const std::u16string& source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a UTF-16 C-string to UTF-8.
///
/// \details Conversion will stop at the first '\0' character.
S narrow(const char16_t* source);

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a char16_t buffer containing UTF-16 text to UTF-8.
///
/// \details Any '\0' characters encountered will be included in the output
///         and will not cause the conversion to end.
///
/// \param  source The char16_t buffer to convert.
/// \param  length The number of char16_t elements in the buffer. (not
///         necessarily codepoints!)
S narrow(const char16_t* source, std::size_t length);

} // be::util

#endif
