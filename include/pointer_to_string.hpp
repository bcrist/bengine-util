#pragma once
#ifndef BE_UTIL_STRING_POINTER_TO_STRING_HPP_
#define BE_UTIL_STRING_POINTER_TO_STRING_HPP_

#include "util_string_autolink.hpp"
#include <be/core/be.hpp>

namespace be::util {
namespace detail {

S pointer_address(U16 ptr, SizeTag<U16>);
S pointer_address(U32 ptr, SizeTag<U32>);
S pointer_address(U64 ptr, SizeTag<U64>);

} // be::util::detail

template <typename T>
S get_pointer_address_string(const T* ptr) {
   return detail::pointer_address((Unsigned<sizeof(ptr)>)ptr, SizeTag<const T*>());
}

} // be::util

#endif
