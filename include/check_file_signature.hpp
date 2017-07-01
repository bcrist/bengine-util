#pragma once
#ifndef BE_UTIL_FS_CHECK_FILE_SIGNATURE_HPP_
#define BE_UTIL_FS_CHECK_FILE_SIGNATURE_HPP_

#include <be/core/buf.hpp>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <std::size_t O = 0, typename T = UC, typename U = T, std::size_t N = 0>
bool file_signature_matches(const Buf<T>& buf, const U (& signature)[N]) noexcept {
   if (buf.size() < O + N) {
      return false;
   }

   auto ptr = buf.get() + O;

   for (int i = 0; i < N; ++i) {
      if (ptr[i] != signature[i]) {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T = UC, typename U = T, std::size_t N = 0>
bool file_signature_matches(const Buf<T>& buf, std::size_t offset, const U(&signature)[N]) noexcept {
   if (buf.size() < offset + N) {
      return false;
   }

   auto ptr = buf.get() + offset;

   for (int i = 0; i < N; ++i) {
      if (ptr[i] != signature[i]) {
         return false;
      }
   }

   return true;
}

} // be::util

#endif
