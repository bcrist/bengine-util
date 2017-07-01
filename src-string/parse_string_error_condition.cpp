#include "pch.hpp"
#include "parse_string_error_condition.hpp"

/*!! include 'parse_string_error_condition' !! 58 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
bool is_valid(ParseStringErrorCondition constant) noexcept {
   switch (constant) {
      case ParseStringErrorCondition::empty_input:
      case ParseStringErrorCondition::syntax_error:
      case ParseStringErrorCondition::out_of_range:
         return true;
      default:
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* parse_string_error_condition_name(ParseStringErrorCondition constant) noexcept {
   switch (constant) {
      case ParseStringErrorCondition::empty_input:  return "empty_input";
      case ParseStringErrorCondition::syntax_error: return "syntax_error";
      case ParseStringErrorCondition::out_of_range: return "out_of_range";
      default:
         return "?";
   }
}

///////////////////////////////////////////////////////////////////////////////
std::array<const ParseStringErrorCondition, 3> parse_string_error_condition_values() noexcept {
   return ::be::EnumTraits<ParseStringErrorCondition>::values<>();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, ParseStringErrorCondition constant) {
   if (is_valid(constant)) {
      os << parse_string_error_condition_name(constant);
   } else {
      os << static_cast<I64>(static_cast<U8>(constant));
   }
   return os;
}

///////////////////////////////////////////////////////////////////////////////
const char* parse_string_error_condition_msg(ParseStringErrorCondition constant) noexcept {
   switch (constant) {
      case ParseStringErrorCondition::empty_input:  return "No value provided";
      case ParseStringErrorCondition::syntax_error: return "Syntax error";
      case ParseStringErrorCondition::out_of_range: return "Value is outside the allowed range";
      default:
         return "An unknown error occurred while parsing the input";
   }
}

} // be::util


/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
S ParseStringErrorCategory::message(int condition) const {
   return parse_string_error_condition_msg(static_cast<ParseStringErrorCondition>(condition));
}

///////////////////////////////////////////////////////////////////////////////
const std::error_category& parse_string_error_category() {
   static ParseStringErrorCategory instance;
   return instance;
}

///////////////////////////////////////////////////////////////////////////////
std::error_code make_error_code(ParseStringErrorCondition e) {
   return std::error_code(static_cast<int>(e), parse_string_error_category());
}

///////////////////////////////////////////////////////////////////////////////
std::error_condition make_error_condition(ParseStringErrorCondition e) {
   return std::error_condition(static_cast<int>(e), parse_string_error_category());
}

} // be::gfx::tex
