#ifdef BE_TEST

#include "xoroshiro_128_plus.hpp"
#include "prng_test_util.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:prng]"

using namespace be;
using be::util::Xoroshiro128Plus;

namespace {
namespace vigna {

/* This is the successor to xorshift128+. It is the fastest full-period
   generator passing BigCrush without systematic failures, but due to the
   relatively short period it is acceptable only for applications with a
   mild amount of parallelism; otherwise, use a xorshift1024* generator.

   Beside passing BigCrush, this generator passes the PractRand test suite
   up to (and included) 16TB, with the exception of binary rank tests,
   which fail due to the lowest bit being an LFSR; all other bits pass all
   tests. We suggest to use a sign test to extract a random Boolean value.

   Note that the generator uses a simulated rotate operation, which most C
   compilers will turn into a single instruction. In Java, you can use
   Long.rotateLeft(). In languages that do not make low-level rotation
   instructions accessible xorshift128+ could be faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

uint64_t s[2];

static inline uint64_t rotl(const uint64_t x, int k) {
   return (x << k) | (x >> (64 - k));
}

uint64_t next(void) {
   const uint64_t s0 = s[0];
   uint64_t s1 = s[1];
   const uint64_t result = s0 + s1;

   s1 ^= s0;
   s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
   s[1] = rotl(s1, 36); // c

   return result;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void jump(void) {
   static const uint64_t JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };

   uint64_t s0 = 0;
   uint64_t s1 = 0;
   for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
      for(int b = 0; b < 64; b++) {
         if (JUMP[i] & 1ULL << b) {
            s0 ^= s[0];
            s1 ^= s[1];
         }
         next();
      }

   s[0] = s0;
   s[1] = s1;
}

} // ()::vigna
} // ()

TEST_CASE("util::Xoroshiro128Plus parameters & basic usage", BE_CATCH_TAGS) {
   using ResultTypeCheck = std::is_same<Xoroshiro128Plus<>::result_type, U64>;
   REQUIRE(ResultTypeCheck::value);

   REQUIRE(Xoroshiro128Plus<>::min() == std::numeric_limits<U64>::min());
   REQUIRE(Xoroshiro128Plus<>::max() == std::numeric_limits<U64>::max());

   util::xo128p prng;
   util::xo128p::state_type state { 16490336266968443936ull, 16834447057089888969ull };

   REQUIRE(prng.state() == state);
   REQUIRE(prng() == 14878039250348781289ull);
   REQUIRE(prng() == 14459866659131551115ull);
   REQUIRE(prng() == 6061679626612904225ull);
   REQUIRE(prng() == 14700104316428589731ull);
}

TEST_CASE("util::Xoroshiro128Plus explicit seed values", BE_CATCH_TAGS) {
   Xoroshiro128Plus<5> prng(64);
   REQUIRE(prng() == 18322729244133645280ull);
   prng.seed();
   REQUIRE(prng() == 2565482072468683346ull);
   prng.seed(1343);
   REQUIRE(prng() == 11686077494212941693ull);
}

TEST_CASE("util::Xoroshiro128Plus seed sequences", BE_CATCH_TAGS) {
   SECTION("std::seed_seq") {
      std::seed_seq sseq { 1,2,3,4,5 };
      Xoroshiro128Plus<> prng(sseq);
      Xoroshiro128Plus<4> prng2;
      std::seed_seq sseq2 { 1,2,3,4,5 };
      prng2.seed(sseq2);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("DummySSeq") {
      DummySSeq dsseq([]() { return (U32)0x12345678U; });
      util::xo128p::state_type state { 0x1234567812345678ULL, 0x1234567812345678ULL };
      REQUIRE(Xoroshiro128Plus<>(dsseq).state() == state);
   }
}

TEST_CASE("util::Xoroshiro128Plus seed from generator", BE_CATCH_TAGS) {
   SECTION("U32 generator") {
      DummyGen<U32> dgen([]() { return (U32)0x8b8bff00; });
      Xoroshiro128Plus<> prng;
      util::xo128p::state_type state { 0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }

   SECTION("U64 generator") {
      DummyGen<U64> dgen([]() { return (U64)0x8b8bff0012344321ULL; });
      Xoroshiro128Plus<> prng;
      util::xo128p::state_type state { 0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }
}

TEST_CASE("util::Xoroshiro128Plus copy construction & assignment", BE_CATCH_TAGS) {
   Xoroshiro128Plus<> prng(64);

   SECTION("Basic copy construction") {
      Xoroshiro128Plus<> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
      prng2();
      REQUIRE(prng.state() != prng2.state());
      prng();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction across different template default seeds") {
      Xoroshiro128Plus<1337> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction using state object") {
      Xoroshiro128Plus<2> prng2(prng.state());
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Basic assignment") {
      Xoroshiro128Plus<> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment across different template default seeds") {
      Xoroshiro128Plus<13387> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment using state object") {
      Xoroshiro128Plus<2> prng2;
      prng2 = prng.state();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment of zero state") {
      util::xo128p::state_type state { 0 };
      prng = state;
      REQUIRE(prng.state() != state);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
   }
}

TEST_CASE("util::Xoroshiro128Plus discard", BE_CATCH_TAGS) {
   Xoroshiro128Plus<> prng(18181818181ULL);
   Xoroshiro128Plus<> prng2(prng);

   REQUIRE(prng.state() == prng2.state());

   prng();
   prng2.discard();

   REQUIRE(prng.state() == prng2.state());

   prng.discard(10);
   prng2.discard(5);
   prng2();
   prng2.discard(4);

   REQUIRE(prng.state() == prng2.state());
}

TEST_CASE("util::Xoroshiro128Plus jump()", BE_CATCH_TAGS) {
   DummyGen<U8> dgen([]() { return (U8)0xFF; });

   Xoroshiro128Plus<> prng;
   prng.seed(dgen);
   Xoroshiro128Plus<> prng2(prng);

   util::xo128p::state_type s1, s2;

   memset(vigna::s, 0xFF, sizeof(vigna::s));
   vigna::next();
   vigna::jump();
   memcpy(s1.data, vigna::s, sizeof(vigna::s));

   memset(vigna::s, 0xFF, sizeof(vigna::s));
   vigna::jump();
   vigna::next();
   memcpy(s2.data, vigna::s, sizeof(vigna::s));

   REQUIRE(s1 == s2);

   REQUIRE(prng == prng2);
   prng2.jump();
   REQUIRE(prng != prng2);
   prng();
   REQUIRE(prng != prng2);
   prng.jump();
   prng2();

   REQUIRE(prng.state() == s1);
   REQUIRE(prng2.state() == s2);

   REQUIRE(prng == prng2);
}

TEST_CASE("util::Xoroshiro128Plus comparison operators", BE_CATCH_TAGS) {
   Xoroshiro128Plus<> prng(18181818181ULL);
   Xoroshiro128Plus<> prng2(prng);

   REQUIRE(prng == prng2);
   REQUIRE(prng.state() == prng2.state());
   prng();
   REQUIRE(prng != prng2);
   REQUIRE(prng.state() != prng2.state());
}


TEST_CASE("util::Xoroshiro128Plus stream insertion/extraction", BE_CATCH_TAGS) {
   Xoroshiro128Plus<> prng(18181818181ULL);
   prng();
   std::stringstream ss;

   SECTION("Generator Insertion") {
      ss << prng;
      REQUIRE(ss.str() == "3817640107780238840 10106765657052273121");

      SECTION("Generator Extraction") {
         Xoroshiro128Plus<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xoroshiro128Plus<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }

   SECTION("State Insertion") {
      ss << prng.state();
      REQUIRE(ss.str() == "3817640107780238840 10106765657052273121");

      SECTION("Generator Extraction") {
         Xoroshiro128Plus<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xoroshiro128Plus<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }
}

#endif
