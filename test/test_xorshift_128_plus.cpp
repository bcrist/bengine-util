#ifdef BE_TEST

#include "xorshift_128_plus.hpp"
#include "prng_test_util.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:prng]"

using namespace be;
using be::util::Xorshift128Plus;

namespace {
namespace vigna {

/* This generator has been replaced by xoroshiro128plus, which is
   significantly faster and has better statistical properties.

   It might be nonetheless useful for languages in which low-level rotate
   instructions are not available. Due to the relatively short period it
   is acceptable only for applications with a mild amount of parallelism;
   otherwise, use a xorshift1024* generator.

   Note that the lowest bit of this generator is an LSFR, and thus it is
   slightly less random than the other bits. We suggest to use a sign test
   to extract a random Boolean value.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

uint64_t s[2];

uint64_t next(void) {
   uint64_t s1 = s[0];
   const uint64_t s0 = s[1];
   s[0] = s0;
   s1 ^= s1 << 23; // a
   s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
   return s[1] + s0;
}


/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */

void jump(void) {
   static const uint64_t JUMP[] = { 0x8a5cd789635d2dff, 0x121fd2155c472f96 };

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

TEST_CASE("util::Xorshift128Plus parameters & basic usage", BE_CATCH_TAGS) {
   using ResultTypeCheck = std::is_same<Xorshift128Plus<>::result_type, U64>;
   REQUIRE(ResultTypeCheck::value);

   REQUIRE(Xorshift128Plus<>::min() == std::numeric_limits<U64>::min());
   REQUIRE(Xorshift128Plus<>::max() == std::numeric_limits<U64>::max());

   util::xs128p prng;
   util::xs128p::state_type state { 16490336266968443936ull, 16834447057089888969ull };

   REQUIRE(prng.state() == state);
   REQUIRE(prng() == 11180128869114632943ULL);
   REQUIRE(prng() == 2306313906319208473ULL);
   REQUIRE(prng() == 9465539935170273786ULL);
   REQUIRE(prng() == 3887240813958492738ULL);
}

TEST_CASE("util::Xorshift128Plus explicit seed values", BE_CATCH_TAGS) {
   Xorshift128Plus<5> prng(64);
   REQUIRE(prng() == 18165202866211910236ull);
   prng.seed();
   REQUIRE(prng() == 16454041752245913263ull);
   prng.seed(1343);
   REQUIRE(prng() == 4442436556527442854ull);
}

TEST_CASE("util::Xorshift128Plus seed sequences", BE_CATCH_TAGS) {
   SECTION("std::seed_seq") {
      std::seed_seq sseq { 1,2,3,4,5 };
      Xorshift128Plus<> prng(sseq);
      Xorshift128Plus<4> prng2;
      std::seed_seq sseq2 { 1,2,3,4,5 };
      prng2.seed(sseq2);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("DummySSeq") {
      DummySSeq dsseq([]() { return (U32)0x12345678U; });
      util::xs128p::state_type state { 0x1234567812345678ULL, 0x1234567812345678ULL };
      REQUIRE(Xorshift128Plus<>(dsseq).state() == state);
   }
}

TEST_CASE("util::Xorshift128Plus seed from generator", BE_CATCH_TAGS) {
   SECTION("U32 generator") {
      DummyGen<U32> dgen([]() { return (U32)0x8b8bff00; });
      Xorshift128Plus<> prng;
      util::xs128p::state_type state { 0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }

   SECTION("U64 generator") {
      DummyGen<U64> dgen([]() { return (U64)0x8b8bff0012344321ULL; });
      Xorshift128Plus<> prng;
      util::xs128p::state_type state { 0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }
}

TEST_CASE("util::Xorshift128Plus copy construction & assignment", BE_CATCH_TAGS) {
   Xorshift128Plus<> prng(64);

   SECTION("Basic copy construction") {
      Xorshift128Plus<> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
      prng2();
      REQUIRE(prng.state() != prng2.state());
      prng();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction across different template default seeds") {
      Xorshift128Plus<1337> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction using state object") {
      Xorshift128Plus<2> prng2(prng.state());
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Basic assignment") {
      Xorshift128Plus<> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment across different template default seeds") {
      Xorshift128Plus<13387> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment using state object") {
      Xorshift128Plus<2> prng2;
      prng2 = prng.state();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment of zero state") {
      util::xs128p::state_type state { 0 };
      prng = state;
      REQUIRE(prng.state() != state);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
   }
}

TEST_CASE("util::Xorshift128Plus discard", BE_CATCH_TAGS) {
   Xorshift128Plus<> prng(18181818181ULL);
   Xorshift128Plus<> prng2(prng);

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

TEST_CASE("util::Xorshift128Plus jump()", BE_CATCH_TAGS) {
   DummyGen<U8> dgen([]() { return (U8)0xFF; });

   Xorshift128Plus<> prng;
   prng.seed(dgen);
   Xorshift128Plus<> prng2(prng);

   util::xs128p::state_type s1, s2;

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

TEST_CASE("util::Xorshift128Plus comparison operators", BE_CATCH_TAGS) {
   Xorshift128Plus<> prng(18181818181ULL);
   Xorshift128Plus<> prng2(prng);

   REQUIRE(prng == prng2);
   REQUIRE(prng.state() == prng2.state());
   prng();
   REQUIRE(prng != prng2);
   REQUIRE(prng.state() != prng2.state());
}


TEST_CASE("util::Xorshift128Plus stream insertion/extraction", BE_CATCH_TAGS) {
   Xorshift128Plus<> prng(18181818181ULL);
   prng();
   std::stringstream ss;

   SECTION("Generator Insertion") {
      ss << prng;
      REQUIRE(ss.str() == "4542659421780700396 10049233086000338143");

      SECTION("Generator Extraction") {
         Xorshift128Plus<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xorshift128Plus<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }

   SECTION("State Insertion") {
      ss << prng.state();
      REQUIRE(ss.str() == "4542659421780700396 10049233086000338143");

      SECTION("Generator Extraction") {
         Xorshift128Plus<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xorshift128Plus<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }
}

#endif
