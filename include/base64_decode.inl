#if !defined(BE_UTIL_STRING_BASE64_DECODE_HPP_) && !defined(DOXYGEN)
#include "base64_decode.hpp"
#elif !defined(BE_UTIL_STRING_BASE64_DECODE_INL_)
#define BE_UTIL_STRING_BASE64_DECODE_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
UC base64_index(char symbol) {
   if (symbol >= 'A' && symbol <= 'Z') {
      return UC(symbol - 'A');
   } else if (symbol >= 'a' && symbol <= 'z') {
      return UC(26 + symbol - 'a');
   } else if (symbol >= '0' && symbol <= '9') {
      return UC(52 + symbol - '0');
   } else if (symbol == S62) {
      return 62u;
   } else if (symbol == S63) {
      return 63u;
   } else if (symbol == P) {
      return UC(-2);
   } else {
      return UC(-1);
   }
}

///////////////////////////////////////////////////////////////////////////////
inline void base64_decode_3_bytes(UC a, UC b, UC c, UC d, UC* out) {
   out[0] = (a << 2) | (b >> 4);
   out[1] = (b << 4) | (c >> 2);
   out[2] = (c << 6) | d;
}

///////////////////////////////////////////////////////////////////////////////
inline void base64_decode_2_bytes(UC a, UC b, UC c, UC* out) {
   out[0] = (a << 2) | (b >> 4);
   out[1] = (b << 4) | (c >> 2);
}

///////////////////////////////////////////////////////////////////////////////
inline void base64_decode_1_byte(UC a, UC b, UC* out) {
   out[0] = (a << 2) | (b >> 4);
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
std::size_t base64_decode(SV encoded_data, UC* out) {
   std::size_t remaining_bytes = encoded_data.size();
   const char* ptr = &(*encoded_data.begin());
   UC* begin = out;

   UC indices[4];
   UC n_indices = 0;
   while (remaining_bytes > 0) {
      UC index = base64_index<S62, S63, P>(*ptr);
      indices[n_indices] = index;
      ++ptr;

      if (index <= 63u) {
         ++n_indices;

         if (n_indices == 4) {
            base64_decode_3_bytes(indices[0], indices[1], indices[2], indices[3], out);
            out += 3;
            n_indices = 0;
         }
      } else if (index == UC(-2)) {
         break;   // if we find a pad character, ignore the rest of the input
      }
      --remaining_bytes;
   }

   if (n_indices == 3) {
      base64_decode_2_bytes(indices[0], indices[1], indices[2], out);
      out += 2;
   } else if (n_indices == 2) {
      base64_decode_1_byte(indices[0], indices[1], out);
      ++out;
   }

   return std::size_t(out - begin);
}

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
S base64_decode_string(SV encoded_data) {
   S decoded;

   if (encoded_data.empty()) {
      return decoded;
   }

   decoded.resize((encoded_data.size() / 4) * 3 + 3);
   std::size_t size = detail::base64_decode<S62, S63, P>(encoded_data, reinterpret_cast<UC*>(&(decoded[0])));
   decoded.resize(size);

   return decoded;
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
Buf<UC> base64_decode_buf(SV encoded_data) {
   if (encoded_data.empty()) {
      return Buf<UC>();
   }

   Buf<UC> buf = make_buf<UC>((encoded_data.size() / 4) * 3 + 3);
   std::size_t size = detail::base64_decode<S62, S63, P>(encoded_data, buf.get());

   buf.release();
   return Buf<UC>(buf.get(), size, be::detail::delete_array);
}

} // be::util

#endif
