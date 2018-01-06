#include <be/core/native.hpp>
#ifdef BE_NATIVE_VC_WIN

#include "utf16_widen_narrow.hpp"

#include <be/core/native/vc_win/vc_win_win32.hpp>
#include <algorithm>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(std::u16string_view source) {
   return std::u16string(source);
}

///////////////////////////////////////////////////////////////////////////////
std::u16string widen(SV source) {
   assert(source.length() <= std::numeric_limits<int>::max());
   int size = ::MultiByteToWideChar(CP_UTF8, 0, source.data(), (int)source.length(), NULL, 0);
   std::u16string dest(size, 0);
   ::MultiByteToWideChar(CP_UTF8, 0, source.data(), (int)source.length(), reinterpret_cast<wchar_t*>(dest.data()), size);
   return dest;
}

///////////////////////////////////////////////////////////////////////////////
S narrow(SV source) {
   return S(source);
}

///////////////////////////////////////////////////////////////////////////////
S narrow(std::u16string_view source) {
   assert(source.length() <= std::numeric_limits<int>::max());
   int size = ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(source.data()), (int)source.length(), NULL, 0, NULL, NULL);
   S dest(size, 0);
   ::WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(source.data()), (int)source.length(), dest.data(), size, NULL, NULL);
   return dest;
}

} // be::util

#endif
