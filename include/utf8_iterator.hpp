#pragma once
#ifndef BE_UTIL_STRING_UTF8_ITERATOR_HPP_
#define BE_UTIL_STRING_UTF8_ITERATOR_HPP_

#include "util_string_autolink.hpp"
#include <be/core/be.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  S::const_iterator adapter for iterating over the UCS
///         codepoints in a UTF-8 encoded S.
///
/// \details Due to the variable character length in UTF-8, Utf8Iterator does
///         not provide the means to modify the string backing it.
///         S::iterator objects can be implicitly converted to
///         Utf8Iterator objects, but a Utf8Iterator always uses a
///         S::const_iterator internally.
///
///         Utf8Iterator will cope with any UTF-8 encoding errors in a way
///         consistent with the guidelines set forth in the Unicode standard.
///         Upon encountering an error, an error flag is set, which can be
///         queried using the getError() function.  If it is not reset using
///         resetError() before the next error is detected, the second error
///         will overwrite the first one.
///
///         Codepoint representations with missing continuation bytes,
///         Continuation bytes with no corresponding initial byte, invalid
///         codepoints (above U+10FFFF) and bytes which are never used in UTF-8
///         will all be replaced by one or more U+FFFD codepoints (the
///         replacement character) when dereferenced or traversed over. 
///         Codepoints encoded using more bytes than necessary will not be
///         replaced, but an error condition will still be set.  Similarly,
///         codepoints in the range [U+D800, U+DFFF] which are reserved for
///         UTF-16 surrogate pairs, and shouldn't normally be found in UTF-8
///         text, will be seen as an error but not replaced.  These rules
///         allow some interoperability with variants like modified UTF-8
///         (U+0000 represented using 2-byte encoding to avoid '\0' appearing
///         in the string) or CESU-8 (UTF-16 code units treated as codepoints,
///         resulting in the chance of UTF-8 encoded surrogate code units).
///
///         Note that care must be taken when using operator++, operator--, and
///         operator* with the possibility of malformed UTF-8.
///         S::c_str() should be used before creating the iterator
///         to ensure the backing string is null terminated.  Otherwise a
///         malformed final codepoint could cause dereferencing of unallocated
///         memory.  When using operator--, the beginning of the string should
///         also be checked to ensure it does not begin with a continuation
///         byte.  Alternatively, the string may be prefixed with any ASCII
///         character (but this will usually be slower).  Furthermore,
///         operator++ must never be called on an iterator "iter" referencing
///         a string "str" when iter == str.end(), and operator-- must never be
///         called when iter == str.begin().
///        
///         If a Utf8Iterator is created from an arbitrary offset into a UTF-8
///         string, it is possible that it will point to one of a multi-byte
///         codepoint's continuation bytes.  In this case, operator* will
///         return U+FFFD for the first codepoint.  To resolve this, the
///         normalize() function can be used.
///
/// \note   Inspired by the article "A code point iterator adapter for C++
///         strings in UTF-8" available at: http://www.nubaria.com/en/blog/?p=371
class Utf8Iterator : public std::iterator<std::bidirectional_iterator_tag, C32, S::difference_type, const C32*, const C32&> {
public:
   ////////////////////////////////////////////////////////////////////////////
   /// \brief  Enumerates the possible errors which a Utf8Iterator can detect.
   enum class error_type {
      no_error = 0,                 ///< Indicates that no errors have occured since resetError() was last called.
      unexpected_continuation_byte, ///< Indicates that a spurious continuation byte was encountered where a non-continuation byte was expected.  The byte is interpretted as U+FFFD instead.
      missing_continuation_byte,    ///< Indicates that one or more continuation bytes were expected, but not found, so the character they would have helped define was interpretted as U+FFFD instead.
      invalid_byte,                 ///< Indicates that a byte of the form b'11111xxx' was found and replaced with U+FFFD.
      overlong_encoding,            ///< Indicates that a codepoint was found in an inefficient encoding.  Eg. U+0020 (SP) encoded as 0xC0 0xA0 instead of just 0x20.  Not considered a structural error.
      surrogate_codepoint,          ///< Indicates a codepoint in the range [U+D800, U+DFFF] was found.  These are reserved for UTF-16 surrogate pairs and shouldn't appear in UTF-8.  Not considered a structural error.
      invalid_codepoint             ///< Indicates a codepoint in the range [U+110000, U+1FFFFF] was found and replaced with U+FFFD
   };

   Utf8Iterator() noexcept;
   Utf8Iterator(S::const_iterator other) noexcept;
   Utf8Iterator(S::iterator other) noexcept;

   explicit operator S::const_iterator() const noexcept;

   error_type error() const noexcept;
   error_type structural_error() const noexcept;
   error_type reset_error() noexcept;

   Utf8Iterator& operator++() noexcept;
   Utf8Iterator operator++(int) noexcept;
   Utf8Iterator& operator--() noexcept;
   Utf8Iterator operator--(int) noexcept;

   Utf8Iterator& normalize() noexcept;

   C32 operator*() const noexcept;
 
private:
   std::pair<C32, error_type> try_parse_(S::const_iterator& it) const noexcept;

   S::const_iterator it_;
   mutable C32 current_codepoint_;
   mutable error_type err_;
   mutable error_type structural_;
};

bool operator==(const Utf8Iterator& a, const Utf8Iterator& b) noexcept;
bool operator!=(const Utf8Iterator& a, const Utf8Iterator& b) noexcept;

bool operator==(const S::const_iterator& a, const Utf8Iterator& b) noexcept;
bool operator!=(const S::const_iterator& a, const Utf8Iterator& b) noexcept;

bool operator==(const Utf8Iterator& a, const S::const_iterator& b) noexcept;
bool operator!=(const Utf8Iterator& a, const S::const_iterator& b) noexcept;

bool operator==(const S::iterator& a, const Utf8Iterator& b) noexcept;
bool operator!=(const S::iterator& a, const Utf8Iterator& b) noexcept;

bool operator==(const Utf8Iterator& a, const S::iterator& b) noexcept;
bool operator!=(const Utf8Iterator& a, const S::iterator& b) noexcept;

const char* get_name(Utf8Iterator::error_type error) noexcept;

} // be::util

#endif
