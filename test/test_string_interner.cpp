#ifdef BE_TEST

#include "string_interner.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:string]"

using namespace be;

TEST_CASE("util::StringInterner basic usage", BE_CATCH_TAGS) {

   util::StringInterner interner;

   SV interned;

   {
      S local_string = "test";
      interned = interner(local_string);
      REQUIRE(interned == "test");
      REQUIRE(interned.data() != local_string.data());
      REQUIRE(interned.data() == interner("test").data());
   }

   REQUIRE(interned == "test");
   REQUIRE(interned.data() == interner("test").data());
}

TEST_CASE("util::StringInterner advanced usage", BE_CATCH_TAGS) {

   util::StringInterner interner;

   SECTION("Too small segments are ignored") {
      interner.reserve(1);
      interner.reserve(2);
      interner.reserve(3);

      REQUIRE_NOTHROW(interner("asdf"));
   }

   SECTION("too small provisioning policy is enlarged") {
      interner.provisioning_policy([](std::size_t s) { return 1; });
      REQUIRE_NOTHROW(interner("asdf"));
   }

   SECTION("existing segment is used if large enough free") {
      interner.provisioning_policy([](std::size_t s) { return 100; });
      interner("asdf");
      REQUIRE(interner("asdf").data() < interner("ffff").data());
      REQUIRE(interner("asdf").data() + 100 > interner("ffff").data());
   }
}

#endif
