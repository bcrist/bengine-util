#include "pch.hpp"
#include "binary_units.hpp"
#include <sstream>

namespace be::util {
namespace {

int append_int(std::ostringstream& oss, U64 value, const char* separator, int digits_before_separator = 3) {
   int digits = 1;
   if (value > 9) {
      digits += append_int(oss, value / 10, separator, (digits_before_separator + 4) % 3);
      U64 v = value % 10;
      oss << (char)('0' + v);
   } else {
      oss << (char)('0' + value);
   }

   if (digits_before_separator == 0 && separator) {
      oss << separator;
   }

   return digits;
}

void append_fp(std::ostringstream& oss, F64 value, bool started, int digits_remaining, const char* separator, int digits_before_separator) {
   if (digits_remaining <= 0) {
      return;
   }

   if (value < std::numeric_limits<F64>::epsilon() * 6) {
      return;
   }

   value *= 10;
   U8 c = (U8)value;
   value -= c;

   if (digits_before_separator == 0 && separator) {
      oss << separator;
   }
   oss << (char)('0' + c);
   if (started || c != 0) {
      --digits_remaining;
      started = true;
   }

   append_fp(oss, value, started, digits_remaining, separator, (digits_before_separator + 4) % 3);
}

} // be::util::()

///////////////////////////////////////////////////////////////////////////////
S binary_unit_string(I64 value, const BinaryUnitStringFormat& format) {
   const char* suffixes = format.suffixes;
   if (!suffixes) {
      suffixes = "KMGTPE";
   }

   int n_suffixes = (int)strlen(suffixes);

   std::ostringstream oss;

   if (value < 0) {
      value = -value;
      oss << '-';
   }

   U64 val = static_cast<U64>(value);

   int i = -1;
   U64 one = 1;
   while (val >= format.cutoff && i < n_suffixes - 1) {
      val >>= 10;
      one <<= 10;
      ++i;
   }

   int digits = append_int(oss, val, format.int_separator);

   int min_sig_digits = format.min_sig_digits;
   if (digits < min_sig_digits && value != 0) {
      bool started = false;
      if (val != 0) {
         min_sig_digits -= digits;
         started = true;
      }

      U64 remainder = static_cast<U64>(value) & (one - 1);
      if (remainder != 0) {
         F64 frac = (F64)remainder / (F64)one;
         oss << '.';
         append_fp(oss, frac, started, min_sig_digits, format.frac_separator, 3);
      }
   }

   oss << ' ';

   if (i >= 0) {
      oss << suffixes[i];

      if (!format.use_jedec_units) {
         oss << 'i';
      }
   }

   return oss.str();
}

} // be::util
