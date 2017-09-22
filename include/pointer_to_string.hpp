#pragma once
#ifndef BE_UTIL_STRING_POINTER_TO_STRING_HPP_
#define BE_UTIL_STRING_POINTER_TO_STRING_HPP_

#include <be/core/be.hpp>
#include <be/core/t_size.hpp>

namespace be::util {
namespace detail {

S pointer_address(U16 ptr, be::t::SizeTag<U16>);
S pointer_address(U32 ptr, be::t::SizeTag<U32>);
S pointer_address(U64 ptr, be::t::SizeTag<U64>);

} // be::util::detail

template <typename T>
S get_pointer_address_string(const T* ptr) {
   return detail::pointer_address((be::t::Unsigned<sizeof(ptr)>)ptr, be::t::SizeTag<const T*>());
}

} // be::util

#endif
