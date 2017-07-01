#if !defined(BE_UTIL_STRING_PARSE_NUMERIC_STRING_HPP_) && !defined(DOXYGEN)
#include "parse_numeric_string.hpp"
#elif !defined(BE_UTIL_STRING_PARSE_NUMERIC_STRING_INL_)
#define BE_UTIL_STRING_PARSE_NUMERIC_STRING_INL_

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
   T operator()(gsl::cstring_span<> value, std::error_code& ec, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max()) {
      auto str = trim(value);
      if (str.empty()) {
         ec = make_error_code(ParseStringErrorCondition::empty_input);
         return T();
      }

      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      F64 val = strtod(begin, &iter);

      if (iter == begin) {
         ec = make_error_code(ParseStringErrorCondition::syntax_error);
         return T(val);
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
               ec = make_error_code(ParseStringErrorCondition::syntax_error);
               return T(val);
            }

            val /= denom;
            iter = iter2;

            while (iter < end && is_whitespace(*iter)) {
               ++iter;
            }
         }
      }

      if (iter != end) {
         ec = make_error_code(ParseStringErrorCondition::syntax_error);
         return T(val);
      }

      if (val > (F64)max || val < (F64)min) {
         ec = make_error_code(ParseStringErrorCondition::out_of_range);
      }

      return T(val);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Signed integers
template <typename T>
struct ParseNumericString<T, false, false> {
   T operator()(gsl::cstring_span<> value, std::error_code& ec, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      if (str.empty()) {
         ec = make_error_code(ParseStringErrorCondition::empty_input);
         return T();
      }

      const char* begin = str.data();
      const char* end = begin + str.size();
      char* iter;
      I64 val = strtoll(begin, &iter, radix);

      if (iter != end) {
         ec = make_error_code(ParseStringErrorCondition::syntax_error);
         return T(val);
      }

      if (val > (I64)max || val < (I64)min) {
         ec = make_error_code(ParseStringErrorCondition::out_of_range);
      }

      return T(val);
   }
};

///////////////////////////////////////////////////////////////////////////////
// Unsigned integers
template <typename T>
struct ParseNumericString<T, false, true> {
   T operator()(gsl::cstring_span<> value, std::error_code& ec, T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max(), I32 radix = 0) {
      auto str = trim(value);
      if (str.empty()) {
         ec = make_error_code(ParseStringErrorCondition::empty_input);
         return T();
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
         ec = make_error_code(ParseStringErrorCondition::syntax_error);
         return T(val);
      }

      if (val > (U64)max || val < (U64)min) {
         ec = make_error_code(ParseStringErrorCondition::out_of_range);
      }

      return T(val);
   }
};

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value) {
   T result;
   detail::ParseNumericString<T> func;
   std::error_code ec;
   result = func(value, ec);
   if (ec) {
      throw RecoverableTrace(ec);
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, std::error_code& ec) noexcept {
   detail::ParseNumericString<T> func;
   return func(value, ec);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, I32 radix) {
   T result;
   detail::ParseNumericString<T> func;
   std::error_code ec;
   result = func(value, ec, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), radix);
   if (ec) {
      throw RecoverableTrace(ec);
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_numeric_string(gsl::cstring_span<> value, I32 radix, std::error_code& ec) noexcept {
   detail::ParseNumericString<T> func;
   return func(value, ec, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), radix);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max) {
   T result;
   detail::ParseNumericString<T> func;
   std::error_code ec;
   result = func(value, ec, min, max);
   if (ec) {
      if (ec == ParseStringErrorCondition::out_of_range) {
         using std::to_string;
         throw RecoverableTrace(ec, "Value must be in the range [ " + to_string(min) + ", " + to_string(max) + " ]");
      } else {
         throw RecoverableTrace(ec);
      }
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, std::error_code& ec) noexcept {
   detail::ParseNumericString<T> func;
   return func(value, ec, min, max);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix) {
   T result;
   detail::ParseNumericString<T> func;
   std::error_code ec;
   result = func(value, ec, min, max, radix);
   if (ec) {
      if (ec == ParseStringErrorCondition::out_of_range) {
         using std::to_string;
         throw RecoverableTrace(ec, "Value must be in the range [ " + to_string(min) + ", " + to_string(max) + " ]");
      } else {
         throw RecoverableTrace(ec);
      }
   }
   return result;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::enable_if_t<std::is_integral<T>::value, T>
parse_bounded_numeric_string(gsl::cstring_span<> value, T min, T max, I32 radix, std::error_code& ec) noexcept {
   detail::ParseNumericString<T> func;
   return func(value, ec, min, max, radix);
}

} // be::util

#endif
