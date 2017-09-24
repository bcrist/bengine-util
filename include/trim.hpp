#pragma once
#ifndef BE_UTIL_STRING_TRIM_HPP_
#define BE_UTIL_STRING_TRIM_HPP_

#include <be/core/alg.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Pred = IsWhitespace>
std::basic_string_view<CharT> trim_left(std::basic_string_view<CharT> input, Pred pred = Pred()) {
   for (auto it = input.cbegin(), end = input.cend(); it != end; ++it) {
      if (!pred(*it)) {
         input.remove_prefix(it - input.cbegin());
         return input;
      }
   }
   input.remove_prefix(input.size());
   return input;
}

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Pred = IsWhitespace>
std::basic_string_view<CharT> trim_right(std::basic_string_view<CharT> input, Pred pred = Pred()) {
   for (auto it = input.crbegin(), end = input.crend(); it != end; ++it) {
      if (!pred(*it)) {
         input.remove_suffix(it - input.crbegin());
         return input;
      }
   }
   input.remove_suffix(input.size());
   return input;
}

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, typename Pred = IsWhitespace>
std::basic_string_view<CharT> trim(std::basic_string_view<CharT> input, Pred pred = Pred()) {
   return trim_left(trim_right(input, pred), pred);
}

} // be

#endif
