#pragma once
#ifndef BE_UTIL_SPLIT_MIX_64_HPP_
#define BE_UTIL_SPLIT_MIX_64_HPP_

#include "util_autolink.hpp"
#include <be/core/t_is_seed_seq.hpp>
#include <be/core/t_is_same_size.hpp>
#include <limits>
#include <iostream>

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  An implementation of the SplitMix64 PRNG which satisfies the
///         requirements set forth in the C++11 standard 26.5.1.3-4 based on
///         the reference implementation found here:
///
///         http://xorshift.di.unimi.it/splitmix64.c
///
/// \author Benjamin Crist
template <U64 DefaultSeed = 0xFFFFFFFFFFFFFFFFULL>
class SplitMix64 {
public:
   static constexpr U64 default_seed = DefaultSeed;
   static constexpr U64 increment = 0x9E3779B97F4A7C15ULL;
   static constexpr U64 multiplier_a = 0xBF58476D1CE4E5B9ULL;
   static constexpr U64 multiplier_b = 0x94D049BB133111EBULL;

   using result_type = U64;
   using type = SplitMix64<DefaultSeed>;

   explicit SplitMix64(U64 s = default_seed) {
      state_ = s;
   }

   template <U64 S>
   SplitMix64(const SplitMix64<S>& other) {
      state_ = other.state();
   }

   template <typename SeedSeq, typename = typename std::enable_if<t::IsSeedSeq<SeedSeq, type>::value>::type>
   explicit SplitMix64(SeedSeq& sseq) {
      seed(sseq);
   }

   template <U64 S>
   type& operator=(const SplitMix64<S>& other) {
      state_ = other.state();
      return *this;
   }

   void seed(U64 s = default_seed) {
      state_ = s;
   }

   template<class Generator>
   void seed(Generator& generator_or_seed_seq) {
      seed_(generator_or_seed_seq, typename t::IsSeedSeq<Generator, type>::type());
   }

   static constexpr U64 (min)() {
      return 0;
   }

   static constexpr U64 (max)() {
      return (std::numeric_limits<U64>::max)();
   }

   U64 operator()() {
      state_ += increment;
      U64 z = state_;
      z = (z ^ (z >> 30)) * multiplier_a;
      z = (z ^ (z >> 27)) * multiplier_b;
      return z ^ (z >> 31);
   }

   void discard(unsigned long long num = 1) {
      while (num) {
         (*this)();
         --num;
      }
   }

   U64 state() const {
      return state_;
   }

private:
   template<class SeedSeq>
   void seed_(SeedSeq& sseq, True) {
      constexpr std::size_t n = 2;
      U32 temp[n];
      sseq.generate(static_cast<U32*>(temp), temp + n);
      memcpy(&state_, temp, sizeof(state_));
   }

   template<class Generator>
   void seed_(Generator& generator, False) {
      seed_with_gen_(generator, typename t::IsSameSize<typename Generator::result_type, U64>::type());
   }

   template <class Generator>
   void seed_with_gen_(Generator& generator, True) {
      state_ = generator();
   }

   template <class Generator>
   void seed_with_gen_(Generator& generator, False) {
      using T = typename Generator::result_type;
      constexpr std::size_t n = (sizeof(state_) + sizeof(T) - 1) / sizeof(T);
      T temp[n];
      static_assert(sizeof(state_) <= sizeof(temp), "temp was not sized properly!");
      for (std::size_t i = 0; i < n; ++i) {
         temp[i] = generator();
      }

      memcpy(&state_, temp, sizeof(state_));
   }

   U64 state_;
};

template <U64 S, U64 T>
bool operator==(const SplitMix64<S>& a, const SplitMix64<T>& b) {
   return a.state() == b.state();
}

template <U64 S, U64 T>
bool operator!=(const SplitMix64<S>& a, const SplitMix64<T>& b) {
   return !(a.state() == b.state());
}

template <typename E, typename T, U64 S>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, const SplitMix64<S>& xs) {
   return  os << xs.state();
}

template <typename E, typename T, U64 S>
std::basic_istream<E, T>& operator>>(std::basic_istream<E, T>& is, SplitMix64<S>& xs) {
   U64 s;
   is >> s;
   if (is) {
      xs.seed(s);
   }

   return is;
}

} // be::util

namespace rnd {

using sm64 = util::SplitMix64<>;

} // be::rnd
} // be

#endif
