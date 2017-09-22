#ifndef BE_UTIL_STRING_BINARY_UNITS_HPP_
#define BE_UTIL_STRING_BINARY_UNITS_HPP_

#include <be/core/be.hpp>

namespace be::util {

struct BinaryUnitStringFormat {
   const char* suffixes = "KMGTPE";
   const char* int_separator = nullptr;
   const char* frac_separator = nullptr;
   int min_sig_digits = 3;
   U32 cutoff = 1023;
   bool use_jedec_units = false;
};

namespace detail {

inline const BinaryUnitStringFormat& default_binary_unit_string_format() {
   static BinaryUnitStringFormat format;
   return format;
}

} // be::util::detail

S binary_unit_string(I64 value, const BinaryUnitStringFormat& format = detail::default_binary_unit_string_format());

} // be::util

#endif
