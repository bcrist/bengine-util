#if !defined(BE_UTIL_PARSE_NUMERIC_STRING_HPP_) && !defined(DOXYGEN)
#include "parse_numeric_string.hpp"
#elif !defined(BE_UTIL_PARSE_NUMERIC_STRING_INL_)
#define BE_UTIL_PARSE_NUMERIC_STRING_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename T,
   bool Float = std::is_floating_point<T>::value,
   bool Unsigned = std::is_unsigned<T>::value>
   struct ParseNumericString;

///////////////////////////////////////////////////////////////////////////////
// Floating point
template <typename T>
struct ParseNumericString<T, true, false> {
   std::pair<T, ParseStringError> operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
      auto str = trim(value);
      if (str.empty()) {
         return std::make_pair(T(), ParseStringError::empty_input);
      }

      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      F64 val = strtod(begin, &iter);

      if (iter == begin) {
         return std::make_pair(T(), ParseStringError::syntax_error);
      }

      while (iter < end && is_whitespace(*iter)) {
         ++iter;
      }

      if (iter < end) {
         if (*iter == '%') {
            val /= 100.f;
            ++iter;
         } else if (*iter == '/') {
            ++iter;
            while (iter < end && is_whitespace(*iter)) {
               ++iter;
            }

            char* iter2;
            F64 denom = strtod(iter, &iter2);

            if (iter2 == iter) {
               return std::make_pair(T(), ParseStringError::syntax_error);
            }

            val /= denom;
            iter = iter2;

            while (iter < end && is_whitespace(*iter)) {
               ++iter;
            }
         }
      }

      if (iter != end) {
         return std::make_pair(T(), ParseStringError::syntax_error);
      }

      if (val > (F64)max || val < (F64)min) {
         return std::make_pair(T(), ParseStringError::out_of_range);
      }

      return std::make_pair(static_cast<T>(val), ParseStringError::none);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Signed integers
template <typename T>
struct ParseNumericString<T, false, false> {
   std::pair<T, ParseStringError> operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      if (str.empty()) {
         return std::make_pair(T(), ParseStringError::empty_input);
      }

      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      I64 val = strtoll(begin, &iter, radix);

      if (iter != end) {
         return std::make_pair(T(), ParseStringError::syntax_error);
      }

      if (val > (I64)max || val < (I64)min) {
         return std::make_pair(T(), ParseStringError::out_of_range);
      }

      return std::make_pair(static_cast<T>(val), ParseStringError::none);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Unsigned integers
template <typename T>
struct ParseNumericString<T, false, true> {
   std::pair<T, ParseStringError> operator()(gsl::cstring_span<> value, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      if (str.empty()) {
         return std::make_pair(T(), ParseStringError::empty_input);
      }

      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;

      if ((radix == 16 || radix == 0) && begin != end && *begin == '#') {
         radix = 16;
         ++begin;
      }

      U64 val = strtoull(begin, &iter, radix);

      if (iter != end) {
         return std::make_pair(T(), ParseStringError::syntax_error);
      }

      if (val > (U64)max || val < (U64)min) {
         return std::make_pair(T(), ParseStringError::out_of_range);
      }

      return std::make_pair(static_cast<T>(val), ParseStringError::none);
   }
};

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, std::pair<T, ParseStringError>>
parse_numeric_string(gsl::cstring_span<> value) {
   detail::ParseNumericString<T> func;
   return func(value);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::value, std::pair<T, ParseStringError>>
parse_numeric_string(gsl::cstring_span<> value, I32 radix) {
   detail::ParseNumericString<T> func;
   return func(value, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), radix);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, std::pair<T, ParseStringError>>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max) {
   detail::ParseNumericString<T> func;
   return func(value, min, max);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::value, std::pair<T, ParseStringError>>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix) {
   detail::ParseNumericString<T> func;
   return func(value, min, max, radix);
}

} // be::util

#endif
