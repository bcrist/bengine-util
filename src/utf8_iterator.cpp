#include "pch.hpp"
#include "utf8_iterator.hpp"

namespace be {
namespace util {
namespace {

const char first_bit = 0x80U; // 10000000
const char second_bit = 0x40U; // 01000000
const char third_bit = 0x20U; // 00100000
const char fourth_bit = 0x10U; // 00010000
const char fifth_bit = 0x08U; // 00001000

// Checks if a codepoint is in the surrogate reserved range or the
// invalid range above U+10FFFF
bool check_codepoint(C32& codepoint, Utf8Iterator::error_type& err) {
   if (codepoint < 0xD800)
      return true;

   if (codepoint <= 0xDFFF) {
      err = Utf8Iterator::error_type::surrogate_codepoint;
      return false;
   }

   if (codepoint <= 0x10FFFF)
      return true;

   err = Utf8Iterator::error_type::invalid_codepoint;
   codepoint = 0xFFFD;
   return false;
}

// Checks if a codepoint encoded in 2 bytes is overlong or in the surrogate or invalid ranges.
bool check_2byte_codepoint(C32& codepoint, Utf8Iterator::error_type& err) {
   if (codepoint > 0x007F)
      return check_codepoint(codepoint, err);

   err = Utf8Iterator::error_type::overlong_encoding;
   return false;
}

// Checks if a codepoint encoded in 3 bytes is overlong or in the surrogate or invalid ranges.
bool check_3byte_codepoint(C32& codepoint, Utf8Iterator::error_type& err) {
   if (codepoint > 0x07FF)
      return check_codepoint(codepoint, err);

   err = Utf8Iterator::error_type::overlong_encoding;
   return false;
}

// Checks if a codepoint encoded in 4 bytes is overlong or in the surrogate or invalid ranges.
bool check_4byte_codepoint(C32& codepoint, Utf8Iterator::error_type& err) {
   if (codepoint > 0xFFFF)
      return check_codepoint(codepoint, err);

   err = Utf8Iterator::error_type::overlong_encoding;
   return false;
}

} // be::util::()

///////////////////////////////////////////////////////////////////////////////
/// \brief  Constructs a new empty Utf8Iterator().
///
/// \details The object cannot be used until it is assigned a a valid state
///         using operator=.
Utf8Iterator::Utf8Iterator() noexcept
   : current_codepoint_(C32(-1)),
     err_(error_type::no_error),
     structural_(error_type::no_error)
{ }

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a S::const_iterator to a Utf8Iterator.
///
/// \param  other The S iterator to wrap this iterator's operations
///         around.
Utf8Iterator::Utf8Iterator(S::const_iterator other) noexcept
   : it_(other),
     current_codepoint_(C32(-1)),
     err_(error_type::no_error),
     structural_(error_type::no_error)
{ }

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts a S::iterator to a Utf8Iterator.
///
/// \note   The iterator is treated as a const_iterator.
///
/// \param  other The S iterator to wrap this iterator's operations
///         around.
Utf8Iterator::Utf8Iterator(S::iterator other) noexcept
   : it_(other),
     current_codepoint_(C32(-1)),
     err_(error_type::no_error),
     structural_(error_type::no_error)
{ }

///////////////////////////////////////////////////////////////////////////////
/// \brief  Converts this Utf8Iterator back into a string iterator.
Utf8Iterator::operator S::const_iterator() const noexcept{
   return it_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves the last error detected since reseterror() was last
///         called.
///
/// \return a non-zero enumeration value representing the last error detected,
///         or an enumeration having value 0 if no error has been detected.
Utf8Iterator::error_type Utf8Iterator::error() const noexcept {
   return err_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves the last structural error detected since reseterror() was
///         last called.
///
/// \details Any errors which are replaced by U+FFFD in operator* are
///         considered structural errors.  This includes any errors other than
///         error::OverlongEncoding and error::SurrogateCodepoint.
///
/// \return a non-zero enumeration value representing the last error detected,
///         or an enumeration having value 0 if no error has been detected.
Utf8Iterator::error_type Utf8Iterator::structural_error() const noexcept {
   return structural_;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves the last error detected since the previous call to
///         reseterror().
///
/// \details After calling reseterror(), geterror() and getStructuralerror()
///         will return error::Noerror until the next error is detected.
///
/// \return a non-zero enumeration value representing the last error detected,
///         or an enumeration having value 0 if no error has been detected.
Utf8Iterator::error_type Utf8Iterator::reset_error() noexcept{
   error_type temp(err_);
   err_ = error_type::no_error;
   structural_ = error_type::no_error;
   return temp;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Adjusts this iterator so that it points to the first byte after
///         the end of this codepoint's byte sequence.
///
/// \details If the current codepoint has not been parsed, or if an error was
///         detected when it was parsed, it will be parsed again and checked
///         for errors.  The first byte of the current codepoint is accessed
///         through the S::const_iterator::operator*, but further
///         bytes are accessed through a pointer offset, to avoid the iterator
///         complaining when the string's null terminator is accessed (which
///         is not required to be accessible through an iterator).
///
/// \warning UTF-8 code unit sequences which do not end in a valid codepoint
///         may cause operator++ to dereference past-the-end elements.  To
///         avoid crashes, if malformed UTF-8 inputs are possible, ensure that
///         the input string is null-terminated before using operator++().
///         This can be ensured by calling S::c_str() on the string
///         before retrieving iterators to it.  Even with this protection,
///         operator++() must never be called when the iterator compares
///         equal to the backing string's end() iterator.
///
/// \return *this
Utf8Iterator& Utf8Iterator::operator++() noexcept {
   auto result = try_parse_(it_);

   current_codepoint_ = C32(-1);

   if (result.second != error_type::no_error) {
      err_ = result.second;

      if (!(result.second == error_type::overlong_encoding || result.second == error_type::surrogate_codepoint))
         structural_ = result.second;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Adjusts this iterator so that it points to the first byte after
///         the end of this codepoint's byte sequence.
///
/// \details Identical to operator++() but returns a copy of the iterator's
///         state before incrementing occurs.
///
/// \sa     operator++()
///
/// \return *this
Utf8Iterator Utf8Iterator::operator++(int) noexcept {
   Utf8Iterator temp = *this;
   ++(*this);
   return temp;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Adjusts this iterator so that it points to the first byte of the
///         closest codepoint which begins before the current position.
///
/// \details Up to four bytes prior to the current byte will be accessed, until
///         a non-continuation byte is found.  If four continuation bytes are
///         found, the last one must be an unexpected one, so the error flags
///         are updated accordingly and the position is decremented by 1 byte.
///         Otherwise, the previous codepoint is parsed to ensure there are no
///         extra continuation bytes between it and the current position.
///
/// \note   If a Utf8Iterator points to a continuation byte in the middle of a
///         valid UTF-8 string, using operator--() will not cause an error
///         to be detected, whereas operator++() never looks backwards, so
///         it would detect an UnexpectedContinuationByte in this situation.
///
/// \warning UTF-8 code unit sequences which do not begin with a valid
///         codepoint may cause operator-- to dereference past-the-beginning
///         elements.  To avoid crashes, if malformed UTF-8 inputs are 
///         possible, ensure that the following expression evaluates to true:
///         (str[0] & 0xC0) != 0x80;
///         Otherwise, the string may be prepended with any ASCII character
///         to satisfy this condition.  Even with this protection, operator--()
///         must never be called when the iterator compares equal to the
///         backing string's begin() iterator.
///
/// \return *this
Utf8Iterator& Utf8Iterator::operator--() noexcept {
   if (!(it_[-1] & first_bit)) {
      // if the previous character is ASCII, we're done!
      // handling this as a special case is a good optimization since most
      // UTF-8 strings consist of mostly ASCII.
      --it_;
      current_codepoint_ = *it_;
   } else {
      // the previous character is either a continuation byte or a starting byte

      for (int i = -1; i >= -4; --i) {
         S::const_iterator it(it_ + i);

         if ((*it & (first_bit | second_bit)) != first_bit) {
            // it_[i] is not a continuation byte.

            auto result = try_parse_(it);

            if (it >= it_) {
               // there are no extra continuation bytes between the previous codepoint and this one.
               // this is the expected outcome!
               it_ += i;

               if (result.second == error_type::no_error) {
                  current_codepoint_ = result.first;
               } else {
                  err_ = result.second;

                  if (!(result.second == error_type::overlong_encoding || result.second == error_type::surrogate_codepoint))
                     structural_ = result.second;

                  current_codepoint_ = C32(-1);
               }

               break;
            } else {
               // the previous codepoint requires less continuation bytes than are actually
               // in the string, so there is at least one unexpected continuation byte at the end.
               --it_;
               err_ = structural_ = error_type::unexpected_continuation_byte;
               current_codepoint_ = C32(-1);
               break;
            }
         } else if (i == -4) {
            // There are never more than 3 consecutive continuation bytes in
            // valid UTF-8 so if we find 4, the last one is definitely an unexpected one.
            --it_;
            err_ = structural_ = error_type::unexpected_continuation_byte;
            current_codepoint_ = C32(-1);
            break;
         }
      }
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Adjusts this iterator so that it points to the first byte of the
///         closest codepoint which begins before the current position.
///
/// \details Identical to operator--() but returns a copy of the iterator's
///         state before decrementing occurs.
///
/// \sa     operator--()
///
/// \return *this
Utf8Iterator Utf8Iterator::operator--(int) noexcept {
   Utf8Iterator temp = *this;
   --(*this);
   return temp;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  If the iterator points to the middle of a codepoint sequence,
///         moves the iterator to the start of that codepoint.
///         
/// \details The error state of the iterator will <i>not</i> be changed by
///         calling normalize().  If the iterator is in the middle of a
///         codepoint where operator* would result in an error, it will be
///         moved to the first byte that causes that error, but the error
///         flags will remain unaffected.
///
/// \return *this
Utf8Iterator& Utf8Iterator::normalize() noexcept {
   if (static_cast<be::I32>(current_codepoint_) < 0) {
      U8 firstByte = *it_;

      if (!(firstByte & first_bit)) {
         current_codepoint_ = *it_;
      } else if (!(firstByte & second_bit)) {
         // pointing to continuation byte; normalization required

         for (int i = -1; i >= -4; --i) {
            S::const_iterator it(it_ + i);

            if ((*it & (first_bit | second_bit)) != first_bit) {
               // it_[i] is not a continuation byte.

               
               auto result = try_parse_(it);

               if (it > it_) {
                  // the current continuation byte is part of the previous codepoint

                  it_ += i;

                  if (result.second == error_type::no_error) {
                     current_codepoint_ = result.first;
                  }
               }
               break;
            }
         }
      }
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves the codepoint starting at the iterator's current
///         position.
///
/// \details If the current codepoint has not been parsed, or if an error was
///         detected when it was parsed, it will be parsed again and checked
///         for errors.  The first byte of the current codepoint is accessed
///         through S::const_iterator::operator*, but further bytes
///         are accessed through a pointer offset, to avoid the iterator
///         complaining when the string's null terminator is accessed (which
///         is not required to be accessible through an iterator).
///
///         If a codepoint is successfully parsed with no errors, its value
///         will be cached until the iterator is incremented, decremented,
///         or assigned to, so that further calls to operator* do not need
///         to reparse the codepoint.  If an error::OverlongEncoding or
///         error::SurrogateCodepoint error occurs while parsing the codepoint,
///         the error flag will be set accordingly, and the value will not be
///         cached, but the detected codepoint will be returned as-is.  If any
///         other error occurs, the replacement character, U+FFFD will be
///         returned.
///
/// \warning UTF-8 code unit sequences which do not end in a valid codepoint
///         may cause operator* to dereference past-the-end elements.  To avoid
///         crashes, if malformed UTF-8 inputs are possible, ensure that the
///         input string is null-terminated before using operator*().  This
///         can be ensured by calling S::c_str() on the string
///         before retrieving iterators to it.  Even with this protection,
///         operator*() must never be called when the iterator compares
///         equal to the backing string's end() iterator.
///
/// \return The UCS codepoint starting at the iterator's current position,
///         or 0xFFFD if a valid codepoint could not be parsed.
C32 Utf8Iterator::operator*() const noexcept {
   S::const_iterator it(it_);
   auto result = try_parse_(it);

   if (result.second != error_type::no_error) {
      err_ = result.second;

      if (!(result.second == error_type::overlong_encoding || result.second == error_type::surrogate_codepoint))
         structural_ = result.second;
   }

   return result.first;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Helper function to attempt to parse a codepoint.
///
/// \param  it A modifiable iterator where the parsing should begin.  The
///         parsing process will consume characters from this iterator.
/// \param  codepoint A reference to a variable to store the parsed codepoint
///         in.
/// \return A error enumeration value indicating whether an error occurred
///         during parsing if if so, its type.
std::pair<C32, Utf8Iterator::error_type> Utf8Iterator::try_parse_(S::const_iterator& it) const noexcept {
   error_type err = error_type::no_error;
   C32 codepoint = 0;

   if (static_cast<be::I32>(current_codepoint_) >= 0 && it == it_) {
      // If we've cached the current codepoint, theres no need to reparse it.
      codepoint = current_codepoint_;

      if (current_codepoint_ <= 0x007F)      // single byte codepoint
         ++it;
      else if (current_codepoint_ <= 0x07FF) // 2 byte codepoint
         it += 2;
      else if (current_codepoint_ <= 0xFFFF) // 3 byte codepoint
         it += 3;
      else                                   // 4 byte codepoint
         it += 4;
   } else {
      // we haven't yet parsed the codepoint (or there was an error when we did, so the result wasn't cached)

      if (!(*it & first_bit)) {
         // if this is a single codepoint (ascii) then we can just increment once and be done.
         codepoint = *it;
         ++it;
      } else {
         // otherwise this byte is part of a multi-byte codepoint

         const char* ptr = &(*it);  // convert string iterator to U8* so we don't dereference a
                                    // past-the-end iterator to the null-terminator - resulting in
                                    // an iterator assertion failure.

         if (*it & second_bit) {
            // the first byte of a multibyte codepoint should have at least 2 MSBs set.
            if (!(*it & third_bit)) {
               // 2-byte codepoint
               codepoint = (*it & 0x1F) << 6;
               ++it;

               if ((ptr[1] & (first_bit | second_bit)) == first_bit) {
                  codepoint |= ptr[1] & 0x3F;
                  ++it;

                  check_2byte_codepoint(codepoint, err);
               } else {
                  err = error_type::missing_continuation_byte;
                  codepoint = 0xFFFD;
               }
            } else if (!(*it & fourth_bit)) {
               // 3-byte codepoint
               codepoint = (*it & 0x0F) << 12;
               ++it;

               for (int i = 1; i < 3; ++i) {
                  if ((ptr[i] & (first_bit | second_bit)) == first_bit) {
                     codepoint |= (ptr[i] & 0x3F) << (6 * (2 - i));
                     ++it;

                     if (i == 2)
                        check_3byte_codepoint(codepoint, err);
                  } else {
                     err = error_type::missing_continuation_byte;
                     codepoint = 0xFFFD;
                     break;
                  }
               }
            } else if (!(*it & fifth_bit)) {
               // 4-byte codepoint
               codepoint = (*it & 0x07) << 18;
               ++it;

               for (int i = 1; i < 4; ++i) {
                  if ((ptr[i] & (first_bit | second_bit)) == first_bit) {
                     codepoint |= (ptr[i] & 0x3F) << (6 * (3 - i));
                     ++it;

                     if (i == 3)
                        check_4byte_codepoint(codepoint, err);
                  } else {
                     err = error_type::missing_continuation_byte;
                     codepoint = 0xFFFD;
                     break;
                  }
               }
            } else {
               // invalid byte - each byte must start with up to four '1' bits followed by a '0'.
               err = error_type::invalid_byte;
               codepoint = 0xFFFD;
               ++it;
            }
         } else {
            // this is a continuation byte, but we expected a starting byte!
            err = error_type::unexpected_continuation_byte;
            codepoint = 0xFFFD;
            ++it;
         }
      }

      if (it == it_ && err == error_type::no_error)
         current_codepoint_ = codepoint;
   }

   return std::make_pair(codepoint, err);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const Utf8Iterator& a, const Utf8Iterator& b) noexcept {
   return (S::const_iterator)a == (S::const_iterator)b;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const Utf8Iterator& a, const Utf8Iterator& b) noexcept {
   return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const S::const_iterator& a, const Utf8Iterator& b) noexcept {
   return a == (S::const_iterator)b;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const S::const_iterator& a, const Utf8Iterator& b) noexcept {
   return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const Utf8Iterator& a, const S::const_iterator& b) noexcept {
   return (S::const_iterator)a == b;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const Utf8Iterator& a, const S::const_iterator& b) noexcept {
   return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const S::iterator& a, const Utf8Iterator& b) noexcept {
   return (S::const_iterator)a == (S::const_iterator)b;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const S::iterator& a, const Utf8Iterator& b) noexcept {
   return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
bool operator==(const Utf8Iterator& a, const S::iterator& b) noexcept {
   return (S::const_iterator)a == (S::const_iterator)b;
}

///////////////////////////////////////////////////////////////////////////////
bool operator!=(const Utf8Iterator& a, const S::iterator& b) noexcept {
   return !(a == b);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Retrieves a string representation of a Utf8Iterator error
///         enumeration for debugging and logging.
///
/// \param  error The Utf8Iterator::error in question.
///
/// \return A c-string representation of the error's name.
const char* get_name(Utf8Iterator::error_type error) noexcept {
   switch (error) {
      case Utf8Iterator::error_type::no_error:                      return "no_error";
      case Utf8Iterator::error_type::unexpected_continuation_byte:  return "unexpected_continuation_byte";
      case Utf8Iterator::error_type::missing_continuation_byte:     return "missing_continuation_byte";
      case Utf8Iterator::error_type::invalid_byte:                  return "invalid_byte";
      case Utf8Iterator::error_type::overlong_encoding:             return "overlong_encoding";
      case Utf8Iterator::error_type::surrogate_codepoint:           return "surrogate_codepoint";
      case Utf8Iterator::error_type::invalid_codepoint:             return "invalid_codepoint";
      default:                                                      return "Unknown";
   }
}

} // be::util
} // be
