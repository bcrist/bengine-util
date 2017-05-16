#pragma once
#ifndef BE_UTIL_CHECK_FILE_SIGNATURE_HPP_
#define BE_UTIL_CHECK_FILE_SIGNATURE_HPP_

#include <be/core/buf.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename U, std::size_t N>
bool file_signature_matches(const Buf<T>& buf, const U (& signature)[N]) {
   if (buf.size() < N) {
      return false;
   }

   for (int i = 0; i < N; ++i) {
      if (buf[i] != signature[i]) {
         return false;
      }
   }

   return true;
}

} // be::util

#endif
