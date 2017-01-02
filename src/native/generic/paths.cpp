#include "pch.hpp"
#include <be/core/native.hpp>
#ifdef BE_NATIVE_GENERIC
#include "paths.hpp"

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
S get_env(const S& name) {
   const char* value = std::getenv(name.c_str());
   if (value) {
      return value;
   }
   return S();
}

} // be::util
} // be

#endif
