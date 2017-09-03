#pragma once
#ifndef BE_UTIL_STRING_UTF8_CODEPOINT_HPP_
#define BE_UTIL_STRING_UTF8_CODEPOINT_HPP_

#include "util_string_autolink.hpp"
#include <be/core/be.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  Allows integral UCS codepoints to be converted to their UTF-8
///         representation.
class Utf8Codepoint {
public:
   using iterator = const char*;

   constexpr Utf8Codepoint() noexcept;
   Utf8Codepoint(C32 codepoint);

   constexpr operator C32() const noexcept;
   constexpr const char* c_str() const noexcept;

   constexpr char operator[](std::size_t offset) const noexcept;
   constexpr std::size_t length() const noexcept;
   constexpr std::size_t size() const noexcept;

   constexpr iterator begin() const noexcept { return code_units_; };
   constexpr iterator end() const noexcept { return code_units_ + length_; };

private:
   C32 codepoint_;
   U8 length_;
   char code_units_[5];
};

} // be::util

#endif
