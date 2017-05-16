#pragma once
#ifndef BE_UTIL_PARSE_STRING_ERROR_HPP_
#define BE_UTIL_PARSE_STRING_ERROR_HPP_

#include "util_autolink.hpp"
#include <be/core/enum_traits.hpp>
#include <be/core/exceptions.hpp>

/*!! include 'parse_string_error' !! 52 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
enum class ParseStringError : U8 {
   none = 0,
   empty_input,
   syntax_error,
   out_of_range
};

bool is_valid(ParseStringError constant) noexcept;
const char* parse_string_error_name(ParseStringError constant) noexcept;
std::array<const ParseStringError, 4> parse_string_error_values() noexcept;
std::ostream& operator<<(std::ostream& os, ParseStringError constant);

} // be::util

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct EnumTraits<::be::util::ParseStringError> {
   using type = ::be::util::ParseStringError;
   using underlying_type = typename std::underlying_type<type>::type;

   static constexpr std::size_t count = 4;

   static bool is_valid(type value) {
      return ::be::util::is_valid(value);
   }

   static const char* name(type value) {
      return ::be::util::parse_string_error_name(value);
   }

   template <typename C = std::array<const type, count>>
   static C values() {
      return {
         ::be::util::ParseStringError::none,
         ::be::util::ParseStringError::empty_input,
         ::be::util::ParseStringError::syntax_error,
         ::be::util::ParseStringError::out_of_range,
      };
   }
};

} // be

/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T ignore_error(std::pair<T, ParseStringError> result) {
   return result.first;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T default_on_error(std::pair<T, ParseStringError> result, T default_value) {
   if (result.second == ParseStringError::none) {
      return result.first;
   } else {
      return default_value;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T throw_on_error(std::pair<T, ParseStringError> result) {
   switch (result.second) {
      case ParseStringError::none:
         return result.first;

      case ParseStringError::empty_input:
         throw Recoverable<>("No value provided!");

      case ParseStringError::syntax_error:
         throw Recoverable<>("Syntax error; expected number!");

      case ParseStringError::out_of_range:
         throw Recoverable<>("Value is outside the allowed range!");

      default:
         throw Recoverable<>("Unexpected parsing error!");
   }
}

} // be::util

#endif
