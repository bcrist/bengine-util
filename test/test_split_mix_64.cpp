#ifdef BE_TEST

#include "split_mix_64.hpp"
#include "prng_test_util.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:prng]"

using namespace be;
using be::util::SplitMix64;

TEST_CASE("util::SplitMix64 parameters & basic usage", BE_CATCH_TAGS) {
   using ResultTypeCheck = std::is_same<SplitMix64<>::result_type, U64>;
   REQUIRE(ResultTypeCheck::value);
   REQUIRE(SplitMix64<>::increment == 0x9E3779B97F4A7C15ULL);
   REQUIRE(SplitMix64<>::multiplier_a == 0xBF58476D1CE4E5B9ULL);
   REQUIRE(SplitMix64<>::multiplier_b == 0x94D049BB133111EBULL);

   REQUIRE(SplitMix64<>::min() == std::numeric_limits<U64>::min());
   REQUIRE(SplitMix64<>::max() == std::numeric_limits<U64>::max());

   util::sm64 prng;
   REQUIRE(prng.state() == 0xFFFFFFFFFFFFFFFFULL);
   REQUIRE(prng() == 0xe4d971771b652c20ULL);
   REQUIRE(prng() == 0xe99ff867dbf682c9ULL);
   REQUIRE(prng() == 0x382ff84cb27281e9ULL);
   REQUIRE(prng() == 0x6d1db36ccba982d2ULL);
}

TEST_CASE("util::SplitMix64 explicit seed values", BE_CATCH_TAGS) {
   SplitMix64<5> prng(64);
   REQUIRE(prng.state() == 64);
   prng.seed();
   REQUIRE(prng.state() == 5);
   prng.seed(1343);
   REQUIRE(prng.state() == 1343);
}

TEST_CASE("util::SplitMix64 seed sequences", BE_CATCH_TAGS) {
   SECTION("std::seed_seq") {
      std::seed_seq sseq { 1,2,3,4,5 };
      SplitMix64<> prng(sseq);
      SplitMix64<4> prng2;
      std::seed_seq sseq2 { 1,2,3,4,5 };
      prng2.seed(sseq2);
      REQUIRE(prng.state() == prng2.state());
   }

   SECTION("DummySSeq") {
      DummySSeq dsseq([]() { return (U32)0x12345678U; });
      REQUIRE(SplitMix64<>(dsseq).state() == 0x1234567812345678ULL);
   }
}

TEST_CASE("util::SplitMix64 seed from generator", BE_CATCH_TAGS) {
   SECTION("U32 generator") {
      DummyGen<U32> dgen([]() { return (U32)0x8b8bff00; });
      SplitMix64<> prng;
      prng.seed(dgen);
      REQUIRE(prng.state() == 0x8b8bff008b8bff00ULL);
   }
   
   SECTION("U64 generator") {
      DummyGen<U64> dgen([]() { return (U64)0x8b8bff0012344321ULL; });
      SplitMix64<> prng;
      prng.seed(dgen);
      REQUIRE(prng.state() == 0x8b8bff0012344321ULL);
   }
}

TEST_CASE("util::SplitMix64 copy construction & assignment", BE_CATCH_TAGS) {
   SplitMix64<> prng(64);

   SECTION("Basic copy construction") {
      SplitMix64<> prng2(prng);
      REQUIRE(prng2.state() == 64);
      prng2();
      REQUIRE(prng.state() != prng2.state());
      prng();
      REQUIRE(prng.state() == prng2.state());
   }
   
   SECTION("Copy construction across different template default seeds") {
      SplitMix64<1337> prng2(prng);
      REQUIRE(prng2.state() == 64);
   }

   SECTION("Basic assignment") {
      SplitMix64<> prng2;
      prng2 = prng;
      REQUIRE(prng2.state() == 64);
   }

   SECTION("Assignment across different template default seeds") {
      SplitMix64<13387> prng2;
      prng2 = prng;
      REQUIRE(prng2.state() == 64);
   }
}

TEST_CASE("util::SplitMix64 discard", BE_CATCH_TAGS) {
   SplitMix64<> prng(18181818181ULL);
   SplitMix64<> prng2(prng);
   
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


TEST_CASE("util::SplitMix64 comparison operators", BE_CATCH_TAGS) {
   SplitMix64<> prng(18181818181ULL);
   SplitMix64<> prng2(prng);

   REQUIRE(prng == prng2);
   prng();
   REQUIRE(prng != prng2);
}


TEST_CASE("util::SplitMix64 stream insertion/extraction", BE_CATCH_TAGS) {
   SplitMix64<> prng(18181818181ULL);
   std::stringstream ss;
   ss << prng;

   SECTION("Insertion") {
      REQUIRE(ss.str() == "18181818181");
   }
   
   SECTION("Extraction") {
      SplitMix64<> prng2;
      ss >> prng2;
      REQUIRE((bool)ss == true);
      REQUIRE(prng.state() == prng2.state());
   }
}

#endif
