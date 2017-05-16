#include "pch.hpp"
#include "parse_string_error.hpp"

/*!! include 'parse_string_error' !! 49 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
bool is_valid(ParseStringError constant) noexcept {
   switch (constant) {
      case ParseStringError::none:
      case ParseStringError::empty_input:
      case ParseStringError::syntax_error:
      case ParseStringError::out_of_range:
         return true;
      default:
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* parse_string_error_name(ParseStringError constant) noexcept {
   switch (constant) {
      case ParseStringError::none:         return "none";
      case ParseStringError::empty_input:  return "empty_input";
      case ParseStringError::syntax_error: return "syntax_error";
      case ParseStringError::out_of_range: return "out_of_range";
      default:
         return "?";
   }
}

///////////////////////////////////////////////////////////////////////////////
std::array<const ParseStringError, 4> parse_string_error_values() noexcept {
   return ::be::EnumTraits<ParseStringError>::values<>();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, ParseStringError constant) {
   if (is_valid(constant)) {
      os << parse_string_error_name(constant);
   } else {
      os << static_cast<I64>(static_cast<U8>(constant));
   }
   return os;
}

} // be::util


/* ######################### END OF GENERATED CODE ######################### */
