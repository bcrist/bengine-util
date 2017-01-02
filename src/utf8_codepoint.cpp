#include "pch.hpp"
#include "utf8_codepoint.hpp"

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  Constructs a Utf8Codepoint representing U+0000.
///
/// \details Conversion of U+0000 to c-string results in a modified-UTF-8
///         representation - "\xC0\x80\0" but character access using
///         operator[] results in 0x00 for all indices.
constexpr Utf8Codepoint::Utf8Codepoint() noexcept
   : codepoint_(0),
     length_(1),
     code_units_{ static_cast<char>(0xC0u), static_cast<char>(0x80u) }
{ }

///////////////////////////////////////////////////////////////////////////////
/// \brief  Constructs a Utf8Codepoint representing the provided codepoint.
///
/// \details Conversion of U+0000 to c-string results in a modified-UTF-8
///         representation - "\xC0\x80\0" but character access using
///         operator[] results in 0x00 for all indices.
///
///         If a surrogate codepoint is presented, no exception will be thrown,
///         and the codepoint will be encoded as in CESU-8.
///
/// \param  codepoint The codepoint this Utf8Codepoint should represent.
/// \throws std::invalid_argument If the codepoint is outside the range of
///         valid UCS codepoints, namely U+0000 to U+10FFFF
Utf8Codepoint::Utf8Codepoint(C32 codepoint)
   : codepoint_(codepoint)
{
   if (static_cast<U32>(codepoint_) > 0x10FFFF)
      throw std::invalid_argument("Invalid codepoint!");

   if (codepoint_ == 0) {
      length_ = 1;
      code_units_[0] = 0xC0U;
      code_units_[1] = 0x80U;
      code_units_[2] = 0;
      code_units_[3] = 0;
      code_units_[4] = 0;
   } else if (codepoint_ < 0x80) {
      length_ = 1;
      code_units_[0] = static_cast<char>(codepoint_);
      code_units_[1] = 0;
      code_units_[2] = 0;
      code_units_[3] = 0;
      code_units_[4] = 0;
   } else if (codepoint_ < 0x800) {
      length_ = 2;
      code_units_[0] = static_cast<char>(0xC0U | (codepoint_ >> 6));
      code_units_[1] = static_cast<char>(0x80U | (codepoint_ & 0x3FU));
      code_units_[2] = 0;
      code_units_[3] = 0;
      code_units_[4] = 0;
   } else if (codepoint_ < 0x10000) {
      length_ = 3;
      code_units_[0] = static_cast<char>(0xE0U | (codepoint_ >> 12));
      code_units_[1] = static_cast<char>(0x80U | ((codepoint_ >> 6) & 0x3FU));
      code_units_[2] = static_cast<char>(0x80U | (codepoint_ & 0x3FU));
      code_units_[3] = 0;
      code_units_[4] = 0;
   } else {
      length_ = 4;
      code_units_[0] = static_cast<char>(0xF0U | (codepoint_ >> 18));
      code_units_[1] = static_cast<char>(0x80U | ((codepoint_ >> 12) & 0x3FU));
      code_units_[2] = static_cast<char>(0x80U | ((codepoint_ >> 6) & 0x3FU));
      code_units_[3] = static_cast<char>(0x80U | (codepoint_ & 0x3FU));
      code_units_[4] = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts this codepoint to a 32-bit character equivalent to the
///         one used to construct the Utf8Codepoint.
///
/// \return The integral representation of this codepoint.
constexpr Utf8Codepoint::operator C32() const noexcept {
   return codepoint_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts this codepoint to a pointer the UTF-8 representation of
///         the codepoint.
///
/// \details For U+0000, operator const C8*() will return a modified-UTF-8
///         representation of the codepoint, namely "\xC0\x80\0", but character
///         access using operator[] will return the standard UTF representation
///         "\0".
///
/// \note   The returned pointer shall be valid until the Utf8Codepoint is
///         destroyed or assigned to.
///
/// \return A c-string containing the modified-UTF-8 representation of the
///         codepoint.
constexpr const char* Utf8Codepoint::c_str() const noexcept {
   return code_units_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves a character at the requested offset from the UTF-8
///         representation of the codepoint.
///
/// \details For U+0000, operator const C8*() will return a modified-UTF-8
///         representation of the codepoint, namely "\xC0\x80\0", but character
///         access using operator[] will return the standard UTF representation
///         "\0".
///
/// \param  offset The offset from the first byte of the UTF-8 representation.
///         If offset >= length(), 0 will be returned.
/// \return The byte at the requested offset used to represent the codepoint.
constexpr char Utf8Codepoint::operator[](size_t offset) const noexcept {
   return codepoint_ == 0 || offset > length_ ? 0 : code_units_[offset];
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves the number of bytes required to encode the codepoint
///         using UTF-8.
///
/// \details For U+0000, length() returns 1, and operator[](0) == 0, even
///         though operator const C8*() returns "\xC0\x80".
constexpr std::size_t Utf8Codepoint::length() const noexcept {
   return length_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Alias for length()
constexpr std::size_t Utf8Codepoint::size() const noexcept {
   return length_;
}

} // be::util
} // be
