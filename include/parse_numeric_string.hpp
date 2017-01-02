#pragma once
#ifndef BE_UTIL_PARSE_NUMERIC_STRING_HPP_
#define BE_UTIL_PARSE_NUMERIC_STRING_HPP_

#include "string_span.hpp"
#include <be/core/exceptions.hpp>
#include <boost/algorithm/string.hpp>

namespace be {
namespace util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename T,
   bool Float = std::is_floating_point<T>::value,
   bool Unsigned = std::is_unsigned<T>::value>
struct ParseNumericString;

// Floating point
template <typename T>
struct ParseNumericString<T, true, false> {
   T operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
      auto str = trim(value);
      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      F64 val = strtod(begin, &iter);

      if (iter == begin) {
         throw Recoverable<>("Input is not a floating-point value!");
      }
      
      while (iter < end && boost::is_space()(*iter)) {
         ++iter;
      }

      if (iter < end && *iter == '%') {
         val /= 100.f;
         ++iter;
      }

      if (iter != end) {
         throw Recoverable<>("Input is not a floating-point value!");
      }

      if (val > (F64)max || val < (F64)min) {
         throw Recoverable<>("Floating-point value out of range!");
      }

      return static_cast<T>(val);
   }
};

// Signed integers
template <typename T>
struct ParseNumericString<T, false, false> {
   T operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      I64 val = strtoll(begin, &iter, radix);

      if (iter != end) {
         throw Recoverable<>("Input is not an integer value!");
      }

      if (val > (I64)max || val < (I64)min) {
         throw Recoverable<>("Integer value out of range!");
      }

      return static_cast<T>(val);
   }
};

// Unsigned integers
template <typename T>
struct ParseNumericString<T, false, true> {
   T operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;

      if ((radix == 16 || radix == 0) && begin != end && *begin == '#') {
         radix = 16;
         ++begin;
      }

      U64 val = strtoull(begin, &iter, radix);

      if (iter != end) {
         throw Recoverable<>("Input is not an unsigned integer value!");
      }

      if (val > (U64)max || val < (U64)min) {
         throw Recoverable<>("Unsigned integer value out of range!");
      }

      return static_cast<T>(val);
   }
};

} // be::util::detail

template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value) {
   detail::ParseNumericString<T> func;
   return func(value);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::type, T>
parse_numeric_string(gsl::cstring_span<> value, I32 radix) {
   detail::ParseNumericString<T> func;
   return func(value, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), radix);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::type || std::is_floating_point<T>::type, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max) {
   detail::ParseNumericString<T> func;
   return func(value, min, max);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::type, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix) {
   detail::ParseNumericString<T> func;
   return func(value, min, max, radix);
}

} // be::util
} // be

#endif
