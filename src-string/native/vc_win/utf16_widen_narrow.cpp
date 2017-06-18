#include <be/core/native.hpp>
#ifdef BE_NATIVE_VC_WIN

#include "utf16_widen_narrow.hpp"

#include <be/core/native/vc_win/vc_win_win32.hpp>
#include <algorithm>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(std::u16string source) {
   return source;
}

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(const S& source) {
   return widen(source.c_str(), source.length());
}

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(const char* source) {
   return widen(source, strlen(source));
}

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(const char* source, std::size_t length) {
   int intLength = (int)std::min((std::size_t)std::numeric_limits<int>::max(), length);
   std::size_t size = ::MultiByteToWideChar(CP_UTF8, 0, source, intLength, NULL, 0);
   std::u16string dest(size, 0);
   ::MultiByteToWideChar(CP_UTF8, 0, source, intLength, reinterpret_cast<wchar_t*>(&dest[0]), (int)size);
   return dest;
}

///////////////////////////////////////////////////////////////////////////////
S narrow(S source) {
   return source;
}

///////////////////////////////////////////////////////////////////////////////
S narrow(const std::u16string& source) {
   return narrow(source.c_str(), source.length());
}

///////////////////////////////////////////////////////////////////////////////
S narrow(const char16_t* source) {
   const char16_t* end = source;
   for (; *end; ++end);
   return narrow(source, end - source);
}

///////////////////////////////////////////////////////////////////////////////
S narrow(const char16_t* source, std::size_t length) {
   int intLength = (int)std::min((std::size_t)std::numeric_limits<int>::max(), length);
   std::size_t size = ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(source), intLength, NULL, 0, NULL, NULL);
   S dest(size, 0);
   ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(source), intLength, &dest[0], (int)size, NULL, NULL);
   return dest;
}

} // be::util

#endif
