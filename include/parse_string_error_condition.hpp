#pragma once
#ifndef BE_UTIL_STRING_PARSE_STRING_ERROR_CONDITION_HPP_
#define BE_UTIL_STRING_PARSE_STRING_ERROR_CONDITION_HPP_

#include "util_string_autolink.hpp"
#include <be/core/enum_traits.hpp>
#include <be/core/exceptions.hpp>

/*!! include 'parse_string_error_condition' !! 51 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
enum class ParseStringErrorCondition : U8 {
   empty_input = 1,
   syntax_error,
   out_of_range
};

bool is_valid(ParseStringErrorCondition constant) noexcept;
const char* parse_string_error_condition_name(ParseStringErrorCondition constant) noexcept;
std::array<const ParseStringErrorCondition, 3> parse_string_error_condition_values() noexcept;
std::ostream& operator<<(std::ostream& os, ParseStringErrorCondition constant);
const char* parse_string_error_condition_msg(ParseStringErrorCondition constant) noexcept;

} // be::util

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct EnumTraits<::be::util::ParseStringErrorCondition> {
   using type = ::be::util::ParseStringErrorCondition;
   using underlying_type = typename std::underlying_type<type>::type;

   static constexpr std::size_t count = 3;

   static bool is_valid(type value) {
      return ::be::util::is_valid(value);
   }

   static const char* name(type value) {
      return ::be::util::parse_string_error_condition_name(value);
   }

   template <typename C = std::array<const type, count>>
   static C values() {
      return {
         ::be::util::ParseStringErrorCondition::empty_input,
         ::be::util::ParseStringErrorCondition::syntax_error,
         ::be::util::ParseStringErrorCondition::out_of_range,
      };
   }
};

} // be

/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
class ParseStringErrorCategory : public std::error_category {
public:
   virtual const char* name() const noexcept { return "String Parsing Error"; }
   virtual S message(int condition) const;
};

const std::error_category& parse_string_error_category();
std::error_code make_error_code(ParseStringErrorCondition e);
std::error_condition make_error_condition(ParseStringErrorCondition e);

} // be::util

namespace std {

///////////////////////////////////////////////////////////////////////////////
template <>
struct is_error_condition_enum<be::util::ParseStringErrorCondition> : std::true_type { };

} // std

#endif
