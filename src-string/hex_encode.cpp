#include "pch.hpp"
#include "hex_encode.hpp"

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
S hex_encode(const Buf<const UC>& data, bool lower_case) {
   S result;
   result.reserve(data.size() * 2);

   const char* symbols = lower_case ? "0123456789abcdef" : "0123456789ABCDEF";

   for (auto p = data.get(), end = p + data.size(); p != end; ++p) {
      char high = symbols[(*p >> 4) & 0xF];
      char low = symbols[*p & 0xF];

      result.append(1, high);
      result.append(1, low);
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////
S hex_encode(SV data, bool lower_case) {
   S result;
   result.reserve(data.length() * 2);

   const char* symbols = lower_case ? "0123456789abcdef" : "0123456789ABCDEF";

   for (char c : data) {
      char high = symbols[(c >> 4) & 0xF];
      char low = symbols[c & 0xF];

      result.append(1, high);
      result.append(1, low);
   }

   return result;
}



} // be::util
