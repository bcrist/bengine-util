#pragma once
#ifndef BE_UTIL_STRING_SPAN_HPP_
#define BE_UTIL_STRING_SPAN_HPP_

#include "util_autolink.hpp"
#include <be/core/alg.hpp>
#include <gsl/string_span>

namespace gsl {

///////////////////////////////////////////////////////////////////////////////
template <std::ptrdiff_t Extent = dynamic_range>
std::ostream& operator<<(std::ostream& os, basic_string_span<const char, Extent> ss) {
   if (ss.size() > 0 && ss[ss.size() - 1] == '\0') {
      os.write(ss.data(), ss.size() - 1);
   } else {
      os.write(ss.data(), ss.size());
   }
   return os;
}

///////////////////////////////////////////////////////////////////////////////
template <std::ptrdiff_t Extent = dynamic_range>
std::ostream& operator<<(std::ostream& os, basic_string_span<char, Extent> ss) {
   if (ss.size() > 0 && ss[ss.size() - 1] == '\0') {
      os.write(ss.data(), ss.size() - 1);
   } else {
      os.write(ss.data(), ss.size());
   }
   return os;
}

} // gsl

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_range, typename Pred = IsWhitespace>
gsl::basic_string_span<CharT> trim_left(gsl::basic_string_span<CharT, Extent> input, Pred pred = Pred()) {
   for (auto it = input.cbegin(), end = input.cend(); it != end; ++it) {
      if (!pred(*it)) {
         return input.subspan(it - input.cbegin());
      }
   }
   return input.subspan(input.size(), 0);
}

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_range, typename Pred = IsWhitespace>
gsl::basic_string_span<CharT> trim_right(gsl::basic_string_span<CharT, Extent> input, Pred pred = Pred()) {
   for (auto it = input.crbegin(), end = input.crend(); it != end; ++it) {
      if (!pred(*it)) {
         return input.subspan(0, input.size() - (it - input.crbegin()));
      }
   }
   return input.subspan(0, 0);
}

///////////////////////////////////////////////////////////////////////////////
template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_range, typename Pred = IsWhitespace>
gsl::basic_string_span<CharT> trim(gsl::basic_string_span<CharT, Extent> input, Pred pred = Pred()) {
   return trim_left(trim_right(input, pred), pred);
}

} // be

#endif
