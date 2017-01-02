#pragma once
#ifndef BE_UTIL_XORSHIFT_1024_STAR_HPP_
#define BE_UTIL_XORSHIFT_1024_STAR_HPP_

#include "split_mix_64.hpp"
#include <cassert>

namespace be {
namespace util {
namespace detail {

class Xorshift1024StarState {
public:
   static constexpr std::size_t state_length = 0x10;
   static constexpr std::size_t state_length_bitmask = 0xF;

   U64 data[state_length];
   int index;
};

template <typename E, typename T, typename S>
void read_xorshift1024star_state(std::basic_istream<E, T>& is, S& s) {
   using state_type = detail::Xorshift1024StarState;
   state_type temp;
   temp.index = state_type::state_length_bitmask;
   for (std::size_t i = 0; i < state_type::state_length && (bool)is; ++i) {
      is >> temp.data[i];
   }

   if (is) {
      s = temp;
   }
}

inline bool operator==(const Xorshift1024StarState& a, const Xorshift1024StarState& b) {
   using state_type = detail::Xorshift1024StarState;
   for (std::size_t i = 0; i < state_type::state_length; ++i) {
      std::size_t j = (i + a.index) & state_type::state_length_bitmask;
      std::size_t k = (i + b.index) & state_type::state_length_bitmask;
      if (a.data[j] != b.data[k]) {
         return false;
      }
   }

   return true;
}

inline bool operator!=(const Xorshift1024StarState& a, const Xorshift1024StarState& b) {
   return !(a == b);
}

template <typename E, typename T>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, const Xorshift1024StarState& state) {
   using state_type = detail::Xorshift1024StarState;
   for (std::size_t i = 0; i < state_type::state_length; ++i) {
      if (i != 0) {
         os << ' ';
      }

      os << state.data[(i + state.index + 1) & state_type::state_length_bitmask];
   }
   
   return os;
}

template <typename E, typename T>
std::basic_istream<E, T>& operator>>(std::basic_istream<E, T>& is, Xorshift1024StarState& state) {
   read_xorshift1024star_state(is, state);
   return is;
}

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
/// \brief  An implementation of the Xorshift1024* PRNG which satisfies the
///         requirements set forth in the C++11 standard 26.5.1.3-4 based on
///         the reference implementation found here:
///
///         http://xorshift.di.unimi.it/xorshift1024star.c
///
/// \author Benjamin Crist
template <U64 DefaultSeed = 0xFFFFFFFFFFFFFFFFULL>
class Xorshift1024Star : private detail::Xorshift1024StarState {
public:
   static constexpr U64 default_seed = DefaultSeed;
   static constexpr U64 multiplier = 1181783497276652981ULL;

   using result_type = U64;
   using type = Xorshift1024Star<DefaultSeed>;
   using state_type = detail::Xorshift1024StarState;

   explicit Xorshift1024Star(U64 s = default_seed) {
      seed(s);
   }

   template <U64 S>
   Xorshift1024Star(const Xorshift1024Star<S>& other) {
      assign_(other.state());
   }

   Xorshift1024Star(const state_type& other) {
      assign_(other);
   }

   template <typename SeedSeq, typename = typename std::enable_if<t::IsSeedSeq<SeedSeq, type>::value>::type>
   explicit Xorshift1024Star(SeedSeq& sseq) {
      seed(sseq);
   }

   template <U64 S>
   type& operator=(const Xorshift1024Star<S>& other) {
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
      const U64 s0 = data[index];
      index = (index + 1) & state_length_bitmask;
      U64 s1 = data[index];

      s1 ^= s1 << 31; // a
      data[index] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c

      return data[index] * multiplier;
   }

   void discard(unsigned long long num = 1) {
      while (num) {
         (*this)();
         --num;
      }
   }

   // a call to jump(n) is equivalent to 2^512 calls to discard(n)
   void jump(unsigned long long num) {
      while (num) {
         jump();
         --num;
      }
   }

   // equivalent to 2^512 calls to discard(1);
   void jump() {
      static const U64 jump_data[state_length] = {
         0x84242f96eca9c41dULL, 0xa3c65b8776f96855ULL, 0x5b34a39f070b5837ULL, 0x4489affce4f31a1eULL,
         0x2ffeeb0a48316f40ULL, 0xdc2d9891fe68c022ULL, 0x3659132bb12fea70ULL, 0xaac17d8efa43cab8ULL,
         0xc4cb815590989b13ULL, 0x5ee975283d71c93bULL, 0x691548c86c1bd540ULL, 0x7910c41d10a1e6a5ULL,
         0x0b5fc64563b3e2a8ULL, 0x047f7684e9fc949dULL, 0xb99181f2d8f685caULL, 0x284600e3f30e38c3ULL
      };

      U64 temp[state_length] = { 0 };
      for (std::size_t i = 0; i < state_length; ++i) {
         for (std::size_t b = 0; b < 8 * sizeof(U64); ++b) {
            if (jump_data[i] & (1ULL << b)) {
               for (std::size_t j = index; j < state_length; ++j) {
                  temp[j - index] ^= data[j];
               }
               const std::size_t base = state_length - index;
               for (std::size_t j = 0; j < index; ++j) {
                  temp[j + base] ^= data[j];
               }
            }
            (*this)();
         }
      }

      memcpy(data + index, temp, sizeof(U64) * (state_length - index));
      if (index > 0) {
         memcpy(data, temp + state_length - index, sizeof(U64) * index);
      }
   }

   const state_type& state() const {
      return *this;
   }

private:
   void assign_(const state_type& other) {
      assert(int(other.index & state_length_bitmask) == other.index);
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
      index = 0;
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
      index = 0;
   }

   template <class Generator>
   void seed_with_gen_(Generator& generator, False) {
      using T = typename Generator::result_type;
      constexpr std::size_t n = (sizeof(data) + sizeof(T) - 1) / sizeof(T);
      T temp[n];
      static_assert(sizeof(data) <= sizeof(temp), "temp was not sized properly!");
      for (std::size_t i = 0; i < n; ++i) {
         temp[i] = generator();
      }

      memcpy(data, temp, sizeof(data));
      index = 0;
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
bool operator==(const Xorshift1024Star<S>& a, const Xorshift1024Star<T>& b) {
   return a.state() == b.state();
}

template <U64 S, U64 T>
bool operator!=(const Xorshift1024Star<S>& a, const Xorshift1024Star<T>& b) {
   return !(a.state() == b.state());
}

template <typename E, typename T, U64 S>
std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, const Xorshift1024Star<S>& xs) {
   return  os << xs.state();
}

template <typename E, typename T, U64 S>
std::basic_istream<E, T>& operator>>(std::basic_istream<E, T>& is, Xorshift1024Star<S>& xs) {
   detail::read_xorshift1024star_state(is, xs);
   return is;
}

} // be::util

namespace rnd {

using xs1024s = util::Xorshift1024Star<>;

} // be::rnd
} // be

#endif
