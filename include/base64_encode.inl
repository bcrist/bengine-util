#if !defined(BE_UTIL_STRING_BASE64_ENCODE_HPP_) && !defined(DOXYGEN)
#include "base64_encode.hpp"
#elif !defined(BE_UTIL_STRING_BASE64_ENCODE_INL_)
#define BE_UTIL_STRING_BASE64_ENCODE_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63>
char base64_symbol(char index) {
   if (index <= 61) {
      return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[index];
   } else if (index == 62) {
      return S62;
   } else {
      return S63;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63>
void base64_encode_3_bytes(UC a, UC b, UC c, S& str) {
   char encoded[4];

   encoded[0] = base64_symbol<S62, S63>(a >> 2);
   encoded[1] = base64_symbol<S62, S63>(0x3F & (a << 4 | b >> 4));
   encoded[2] = base64_symbol<S62, S63>(0x3F & (b << 2 | c >> 6));
   encoded[3] = base64_symbol<S62, S63>(0x3F & c);

   str.append(encoded, size_t(4));
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63>
void base64_encode_2_bytes(UC a, UC b, S& str) {
   char encoded[3];
   encoded[0] = base64_symbol<S62, S63>(a >> 2);
   encoded[1] = base64_symbol<S62, S63>(0x3F & (a << 4 | b >> 4));
   encoded[2] = base64_symbol<S62, S63>(0x3F & (b << 2));

   str.append(encoded, size_t(3));
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63>
void base64_encode_1_byte(UC data, S& str) {
   char encoded[2];
   encoded[0] = base64_symbol<S62, S63>(data >> 2);
   encoded[1] = base64_symbol<S62, S63>(0x3F & (data << 4));

   str.append(encoded, size_t(2));
}

///////////////////////////////////////////////////////////////////////////////
template <char P>
struct Base64Padding {
   static void encode(std::size_t count, S& str) {
      str.append(count, P);
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct Base64Padding<'\0'> {
   static void encode(std::size_t count, S& str) { }
};

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
S base64_encode(const Buf<const UC>& data) {
   S str;
   const UC* ptr = data.get();

   std::size_t n = data.size() / 3;
   for (std::size_t i = n; i > 0; --i) {
      detail::base64_encode_3_bytes<S62, S63>(ptr[0], ptr[1], ptr[2], str);
      ptr += 3;
   }

   std::size_t written_bytes = n * 3;
   if (written_bytes < data.size()) {
      if (data.size() - written_bytes > 1) {
         detail::base64_encode_2_bytes<S62, S63>(ptr[0], ptr[1], str);
         detail::Base64Padding<P>::encode(1, str);
      } else {
         detail::base64_encode_1_byte<S62, S63>(ptr[0], str);
         detail::Base64Padding<P>::encode(2, str);
      }
   }

   return str;
}

///////////////////////////////////////////////////////////////////////////////
template <char S62, char S63, char P>
S base64_encode(const S& data) {
   return base64_encode<S62, S63, P>((Buf<const UC>)tmp_buf(data));
}

} // be::util

#endif
