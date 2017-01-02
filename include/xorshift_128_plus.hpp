#pragma once
#ifndef BE_UTIL_XORSHIFT_128_PLUS_HPP_
#define BE_UTIL_XORSHIFT_128_PLUS_HPP_

#include "split_mix_64.hpp"
#include <limits>
#include <cassert>

namespace be {
namespace util {
namespace detail {

class Xorshift128PlusState {
public:
   static constexpr std::size_t state_length = 2;
   U64 data[state_length];
};

#pragma warning(push)
#pragma warning(disable: 4701) // potentially uninitialized variable (not really)
template <typename E, typename T, typename S>
void read_xorshift128plus_state(std::basic_istream<E, T>& is, S& s) {
   using state_type = detail::Xorshift128PlusState;
   state_type temp;
   for (std::size_t i = 0; i < state_type::state_length && (bool)is; ++i) {
      is >> temp.data[i];
   }

   if (is) {
      s = temp;
   }
}
#pragma warning(pop)

inline bool operator==(const Xorshift128PlusState& a, const Xorshift128PlusState& b) {
   using state_type = detail::Xorshift128PlusState;
   for (std::size_t i = 0; i < state_type::state_length; ++i) {
      if (a.data[i] != b.data[i]) {
         return false;
      }
   }

   return true;
}

inline bool operator!=(const Xorshift128PlusState& a, const Xorshift128PlusState& b) {
   return !(a == b);
}

template <typename E, typename T>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, const Xorshift128PlusState& state) {
   using state_type = detail::Xorshift128PlusState;
   for (std::size_t i = 0; i < state_type::state_length; ++i) {
      if (i != 0) {
         os << ' ';
      }

      os << state.data[i];
   }

   return os;
}

template <typename E, typename T>
std::basic_istream<E, T>& operator>>(std::basic_istream<E, T>& is, Xorshift128PlusState& state) {
   read_xorshift128plus_state(is, state);
   return is;
}

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
/// \brief  An implementation of the Xorshift128+ PRNG which satisfies the
///         requirements set forth in the C++11 standard 26.5.1.3-4 based on
///         the reference implementation found here:
///
///         http://xorshift.di.unimi.it/xorshift128plus.c
///
/// \author Benjamin Crist
template <U64 DefaultSeed = 0xFFFFFFFFFFFFFFFFULL>
class Xorshift128Plus : private detail::Xorshift128PlusState {
public:
   static constexpr U64 default_seed = DefaultSeed;

   using result_type = U64;
   using type = Xorshift128Plus<DefaultSeed>;
   using state_type = detail::Xorshift128PlusState;

   explicit Xorshift128Plus(U64 s = default_seed) {
      seed(s);
   }

   template <U64 S>
   Xorshift128Plus(const Xorshift128Plus<S>& other) {
      assign_(other.state());
   }

   Xorshift128Plus(const state_type& other) {
      assign_(other);
   }

   template <typename SeedSeq, typename = typename std::enable_if<t::IsSeedSeq<SeedSeq, type>::value>::type>
   explicit Xorshift128Plus(SeedSeq& sseq) {
      seed(sseq);
   }

   template <U64 S>
   type& operator=(const Xorshift128Plus<S>& other) {
      assign_(other.state());
      return *this;
   }

   type& operator=(const state_type& other) {
      assign_(other);
      return *this;
   }

   void seed(U64 s = default_seed) {
      SplitMix64<> seed_gen(s);
      seed(seed_gen);
   }

   template<class Generator>
   void seed(Generator& generatorOrSeedSeq) {
      seed_(generatorOrSeedSeq, typename t::IsSeedSeq<Generator, type>::type());
   }

   static constexpr U64 (min)() {
      return 0;
   }

   static constexpr U64 (max)() {
      return (std::numeric_limits<U64>::max)();
   }

   U64 operator()() {
      U64 s1 = data[0];
      const U64 s0 = data[1];
      data[0] = s0;
      s1 ^= s1 << 23; // a
      data[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
      return data[1] + s0; 
   }

   void discard(unsigned long long num = 1) {
      while (num) {
         (*this)();
         --num;
      }
   }

   // a call to jump(n) is equivalent to 2^64 calls to discard(n)
   void jump(unsigned long long num) {
      while (num) {
         jump();
         --num;
      }
   }

   // equivalent to 2^64 calls to discard(1);
   void jump() {
      static const U64 jump_data[state_length] = { 0x8a5cd789635d2dffULL, 0x121fd2155c472f96ULL };

      U64 s0 = 0;
      U64 s1 = 0;
      for (std::size_t i = 0; i < state_length; ++i) {
         for (std::size_t b = 0; b < 8 * sizeof(U64); ++b) {
            if (jump_data[i] & (1ULL << b)) {
               s0 ^= data[0];
               s1 ^= data[1];
            }
            (*this)();
         }
      }

      data[0] = s0;
      data[1] = s1;
   }

   const state_type& state() const {
      return *this;
   }

private:
   void assign_(const state_type& other) {
      state_type* my_state = this;
      if (&other != my_state) {
         *my_state = other;
         validate_state_();
      }
   }

   template<class SeedSeq>
   void seed_(SeedSeq& sseq, True) {
      constexpr std::size_t n = state_length * 2;
      U32 temp[n];
      sseq.generate(static_cast<U32*>(temp), temp + n);
      memcpy(data, temp, sizeof(data));
      validate_state_();
   }

   template<class Generator>
   void seed_(Generator& generator, False) {
      seed_with_gen_(generator, typename t::IsSameSize<typename Generator::result_type, U64>::type());
      validate_state_();
   }

   template <class Generator>
   void seed_with_gen_(Generator& generator, True) {
      for (std::size_t i = 0; i < state_length; ++i) {
         data[i] = generator();
      }
   }

   template <class Generator>
   void seed_with_gen_(Generator& generator, std::false_type) {
      using T = typename Generator::result_type;
      constexpr std::size_t n = (sizeof(data) + sizeof(T) - 1) / sizeof(T);
      T temp[n];
      static_assert(sizeof(data) <= sizeof(temp), "temp was not sized properly!");
      for (std::size_t i = 0; i < n; ++i) {
         temp[i] = generator();
      }

      memcpy(data, temp, sizeof(data));
   }

   void validate_state_() {
      for (std::size_t i = 0; i < state_length; ++i) {
         if (data[i] != 0) {
            return;
         }
      }

      seed(); // seed with default seed; guaranteed not to produce all zeros
   }
};

template <U64 S, U64 T>
bool operator==(const Xorshift128Plus<S>& a, const Xorshift128Plus<T>& b) {
   return a.state() == b.state();
}

template <U64 S, U64 T>
bool operator!=(const Xorshift128Plus<S>& a, const Xorshift128Plus<T>& b) {
   return !(a.state() == b.state());
}

template <typename E, typename T, U64 S>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, const Xorshift128Plus<S>& xs) {
   return  os << xs.state();
}

template <typename E, typename T, U64 S>
std::basic_istream<E, T>& operator>>(std::basic_istream<E, T>& is, Xorshift128Plus<S>& xs) {
   detail::read_xorshift128plus_state(is, xs);
   return is;
}

} // be::util

namespace rnd {

using xs128p = util::Xorshift128Plus<>;

} // be::rnd
} // be

#endif
