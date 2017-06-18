#ifdef BE_TEST

#include "xorshift_1024_star.hpp"
#include "prng_test_util.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:prng]"

using namespace be;
using be::util::Xorshift1024Star;

namespace {
namespace vigna {

/* This is a fast, high-quality generator. If 1024 bits of state are too
   much, try a xoroshiro128+ generator.

   Note that the three lowest bits of this generator are LSFRs, and thus
   they are slightly less random than the other bits. We suggest to use a
   sign test to extract a random Boolean value.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

uint64_t s[16]; 
int p;

uint64_t next(void) {
   const uint64_t s0 = s[p];
   uint64_t s1 = s[p = (p + 1) & 15];
   s1 ^= s1 << 31; // a
   s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30); // b,c
   return s[p] * UINT64_C(1181783497276652981);
}


/* This is the jump function for the generator. It is equivalent
   to 2^512 calls to next(); it can be used to generate 2^512
   non-overlapping subsequences for parallel computations. */

void jump(void) {
   static const uint64_t JUMP[] = { 0x84242f96eca9c41d,
      0xa3c65b8776f96855, 0x5b34a39f070b5837, 0x4489affce4f31a1e,
      0x2ffeeb0a48316f40, 0xdc2d9891fe68c022, 0x3659132bb12fea70,
      0xaac17d8efa43cab8, 0xc4cb815590989b13, 0x5ee975283d71c93b,
      0x691548c86c1bd540, 0x7910c41d10a1e6a5, 0x0b5fc64563b3e2a8,
      0x047f7684e9fc949d, 0xb99181f2d8f685ca, 0x284600e3f30e38c3
   };

   uint64_t t[16] = { 0 };
   for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
      for(int b = 0; b < 64; b++) {
         if (JUMP[i] & 1ULL << b)
            for(int j = 0; j < 16; j++)
               t[j] ^= s[(j + p) & 15];
         next();
      }

   for(int j = 0; j < 16; j++)
      s[(j + p) & 15] = t[j];
}

} // ()::vigna
} // ()

TEST_CASE("util::Xorshift1024Star parameters & basic usage", BE_CATCH_TAGS) {
   using ResultTypeCheck = std::is_same<Xorshift1024Star<>::result_type, U64>;
   REQUIRE(ResultTypeCheck::value);
   REQUIRE(Xorshift1024Star<>::multiplier == 1181783497276652981ULL);

   REQUIRE(Xorshift1024Star<>::min() == std::numeric_limits<U64>::min());
   REQUIRE(Xorshift1024Star<>::max() == std::numeric_limits<U64>::max());

   util::xs1024s prng;
   util::xs1024s::state_type state {{
         16490336266968443936ull, 16834447057089888969ull,
         4048727598324417001ull, 7862637804313477842ull,
         13015481187462834606ull, 15212506146343009075ull,
         17388166129998380965ull, 4638043754431676516ull,
         14194966728679492740ull, 224706085343030812ull,
         266333147328794389ull, 14876895156350639527ull,
         128728123335686875ull, 15965508135439572106ull,
         3840741419012094145ull, 12461074500743476456ull
      }, 0 };

   REQUIRE(prng.state() == state);
   REQUIRE(prng() == 4291630594070596585ULL);
   REQUIRE(prng() == 5888908670475247019ULL);
   REQUIRE(prng() == 18221217103853320316ULL);
   REQUIRE(prng() == 3480080632864392856ULL);
}

TEST_CASE("util::Xorshift1024Star explicit seed values", BE_CATCH_TAGS) {
   Xorshift1024Star<5> prng(64);
   REQUIRE(prng() == 4541693288832150068ull);
   prng.seed();
   REQUIRE(prng() == 7000923110509221998ull);
   prng.seed(1343);
   REQUIRE(prng() == 10370313130562525558ull);
}

TEST_CASE("util::Xorshift1024Star seed sequences", BE_CATCH_TAGS) {
   SECTION("std::seed_seq") {
      std::seed_seq sseq { 1,2,3,4,5 };
      Xorshift1024Star<> prng(sseq);
      Xorshift1024Star<4> prng2;
      std::seed_seq sseq2 { 1,2,3,4,5 };
      prng2.seed(sseq2);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("DummySSeq") {
      DummySSeq dsseq([]() { return (U32)0x12345678U; });
      util::xs1024s::state_type state { {
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
            0x1234567812345678ULL, 0x1234567812345678ULL,
         }, 0 };
      REQUIRE(Xorshift1024Star<>(dsseq).state() == state);
   }
}

TEST_CASE("util::Xorshift1024Star seed from generator", BE_CATCH_TAGS) {
   SECTION("U32 generator") {
      DummyGen<U32> dgen([]() { return (U32)0x8b8bff00; });
      Xorshift1024Star<> prng;
      util::xs1024s::state_type state { {
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL,
            0x8b8bff008b8bff00ULL, 0x8b8bff008b8bff00ULL
         }, 0 };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }

   SECTION("U64 generator") {
      DummyGen<U64> dgen([]() { return (U64)0x8b8bff0012344321ULL; });
      Xorshift1024Star<> prng;
      prng(); // verify that index gets reset.
      util::xs1024s::state_type state { {
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
            0x8b8bff0012344321ULL, 0x8b8bff0012344321ULL,
         }, 0 };
      prng.seed(dgen);
      REQUIRE(prng.state() == state);
   }
}

TEST_CASE("util::Xorshift1024Star copy construction & assignment", BE_CATCH_TAGS) {
   Xorshift1024Star<> prng(64);

   SECTION("Basic copy construction") {
      Xorshift1024Star<> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
      prng2();
      REQUIRE(prng.state() != prng2.state());
      prng();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction across different template default seeds") {
      Xorshift1024Star<1337> prng2(prng);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Copy construction using state object") {
      Xorshift1024Star<2> prng2(prng.state());
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Basic assignment") {
      Xorshift1024Star<> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment across different template default seeds") {
      Xorshift1024Star<13387> prng2;
      prng2 = prng;
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment using state object") {
      Xorshift1024Star<2> prng2;
      prng2 = prng.state();
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("Assignment of zero state") {
      util::xs1024s::state_type state { { 0 }, 0 };
      prng = state;
      REQUIRE(prng.state() != state);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
      REQUIRE(prng() != 0);
   }
}

TEST_CASE("util::Xorshift1024Star discard", BE_CATCH_TAGS) {
   Xorshift1024Star<> prng(18181818181ULL);
   Xorshift1024Star<> prng2(prng);

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

TEST_CASE("util::Xorshift1024Star jump()", BE_CATCH_TAGS) {
   DummyGen<U8> dgen([]() { return (U8)0xFF; });

   Xorshift1024Star<> prng;
   prng.seed(dgen);
   Xorshift1024Star<> prng2(prng);

   util::xs1024s::state_type s1, s2;
   
   memset(vigna::s, 0xFF, sizeof(vigna::s));
   vigna::p = 0;
   vigna::next();
   vigna::jump();
   memcpy(s1.data, vigna::s, sizeof(vigna::s));
   s1.index = vigna::p;

   memset(vigna::s, 0xFF, sizeof(vigna::s));
   vigna::p = 0;
   vigna::jump();
   vigna::next();
   memcpy(s2.data, vigna::s, sizeof(vigna::s));
   s2.index = vigna::p;

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

TEST_CASE("util::Xorshift1024Star comparison operators", BE_CATCH_TAGS) {
   Xorshift1024Star<> prng(18181818181ULL);
   Xorshift1024Star<> prng2(prng);

   REQUIRE(prng == prng2);
   REQUIRE(prng.state() == prng2.state());
   prng();
   REQUIRE(prng != prng2);
   REQUIRE(prng.state() != prng2.state());

   SECTION("Equality of states regardless of circular-buffer rotation") {
      util::xs1024s::state_type s1 {{ 1, 2, 3 }, 0 };
      util::xs1024s::state_type s2 {{ 0, 1, 2, 3 }, 1 };

      REQUIRE(s1 == s2);
   }
}


TEST_CASE("util::Xorshift1024Star stream insertion/extraction", BE_CATCH_TAGS) {
   Xorshift1024Star<> prng(18181818181ULL);
   prng();
   std::stringstream ss;

   SECTION("Generator Insertion") {
      ss << prng;
      REQUIRE(ss.str() ==
              "12202461725578204765 11882970973652205881 "
              "5923212207835580905 16326117212382333386 "
              "7748261383765600620 5396197518968460635 "
              "15550032103755770495 4397805399254442938 "
              "6446078105217541439 8025986963995757479 "
              "3525629286681585947 10172642172234523649 "
              "12834264678305333192 11909659428955521828 "
              "3171019241509211121 11129880870853319148");

      SECTION("Generator Extraction") {
         Xorshift1024Star<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xorshift1024Star<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }

   SECTION("State Insertion") {
      ss << prng.state();
      REQUIRE(ss.str() ==
              "12202461725578204765 11882970973652205881 "
              "5923212207835580905 16326117212382333386 "
              "7748261383765600620 5396197518968460635 "
              "15550032103755770495 4397805399254442938 "
              "6446078105217541439 8025986963995757479 "
              "3525629286681585947 10172642172234523649 "
              "12834264678305333192 11909659428955521828 "
              "3171019241509211121 11129880870853319148");

      SECTION("Generator Extraction") {
         Xorshift1024Star<> prng2;
         ss >> prng2;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == prng2.state());
      }

      SECTION("State Extraction") {
         Xorshift1024Star<>::state_type state;
         ss >> state;
         REQUIRE((bool)ss == true);
         REQUIRE(prng.state() == state);
      }
   }
}

#endif
