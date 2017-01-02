#pragma once
#ifndef BE_UTIL_PRNG_TEST_UTIL_HPP_
#define BE_UTIL_PRNG_TEST_UTIL_HPP_

#include <be/core/be.hpp>
#include <functional>
#include <random>

namespace be {

struct DummySSeq {
   using result_type = U32;

   DummySSeq(const std::function<U32()>& func) : func_(func) { }

   template <typename I>
   void generate(I begin, I end) {
      while (begin != end) {
         *begin = func_();
         ++begin;
      }
   }

private:
   std::function<U32()> func_;
};

template <typename T>
struct DummyGen {
   using result_type = T;

   DummyGen(const std::function<T()>& func) : func_(func) { }

   T operator()() {
      return func_();
   }

   static constexpr T min() {
      return std::numeric_limits<T>::min();
   }

   static constexpr T max() {
      return std::numeric_limits<T>::max();
   }

private:
   std::function<T()> func_;
};

} // be

#endif
