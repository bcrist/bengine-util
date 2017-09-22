#pragma once
#ifndef BE_UTIL_FNV_HPP_
#define BE_UTIL_FNV_HPP_

#include <be/core/be.hpp>
#include <gsl/string_span>

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t S = sizeof(T)>
struct FnvMult { };

template <typename T>
struct FnvMult<T, 4> {
   static const T value = T(0x1000193u);
};

template <typename T>
struct FnvMult<T, 8> {
   static const T value = T(0x100000001B3ull);
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, T M = FnvMult<T>::value>
struct ConstexprFnv {
   constexpr T operator()(const char* ptr) const {
      return operator()(T(0), ptr);
   }

   constexpr T operator()(T v, const char* ptr) const {
      return *ptr ? ConstexprFnv<T>()((v * M) ^ U8(*ptr), ptr + 1) : v;
   }
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, T B = T(0), T M = FnvMult<T>::value>
struct Fnv {
   template <typename I>
   T operator()(I begin, I end) const {
      T v(B);
      for (; begin != end; ++begin) {
         v = (v * M) ^ U8(*begin);
      }
      return v;
   }

   template <typename I>
   T operator()(T v, I begin, I end) const {
      for (; begin != end; ++begin) {
         v = (v * M) ^ U8(*begin);
      }
      return v;
   }
};

///////////////////////////////////////////////////////////////////////////////
template <typename T, T B = T(0), T M = FnvMult<T>::value>
struct Fnva {
   template <typename I>
   T operator()(I begin, I end) const {
      T v(B);
      for (; begin != end; ++begin) {
         v = (v ^ U8(*begin)) * M;
      }
      return v;
   }

   template <typename I>
   T operator()(T v, I begin, I end) const {
      for (; begin != end; ++begin) {
         v = (v ^ U8(*begin)) * M;
      }
      return v;
   }
};

#pragma warning(push)
#pragma warning(disable:4307)
///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct FnvBasis {
   static const T value = T(ConstexprFnv<T>()("chongo <Landon Curt Noll> /\\../\\"));
};
#pragma warning(pop)

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <typename T = U64, typename I = void>
T fnv1(I begin, I end) {
   using U = std::make_unsigned_t<T>;
   return T(detail::Fnv<U, detail::FnvBasis<U>::value>()(begin, end));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T = U64, typename I = void>
T fnv1a(I begin, I end) {
   using U = std::make_unsigned_t<T>;
   return T(detail::Fnva<U, detail::FnvBasis<U>::value>()(begin, end));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T = U64, typename I = void>
T fnv1(T basis, I begin, I end) {
   using U = std::make_unsigned_t<T>;
   return T(detail::Fnv<U>()(U(basis), begin, end));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T = U64, typename I = void>
T fnv1a(T basis, I begin, I end) {
   using U = std::make_unsigned_t<T>;
   return T(detail::Fnva<U>()(U(basis), begin, end));
}

S fnv256_0(gsl::cstring_span<> input);
S fnv256_1(gsl::cstring_span<> input);
S fnv256_1a(gsl::cstring_span<> input);

} // be::util

#endif

