#ifdef BE_TEST

#include "binary_units.hpp"

#include <be/testing/arbitrary_builtins.hpp>
#include <iostream>

#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util]"

using namespace be;

TEST_CASE("util::binary_unit_string", BE_CATCH_TAGS) {

   REQUIRE(util::binary_unit_string(0) == "0 ");
   REQUIRE(util::binary_unit_string(1) == "1 ");
   REQUIRE(util::binary_unit_string(10) == "10 ");
   REQUIRE(util::binary_unit_string(100) == "100 ");
   REQUIRE(util::binary_unit_string(1000) == "1000 ");

   REQUIRE(util::binary_unit_string(1024ll) == "1 Ki");
   REQUIRE(util::binary_unit_string(1024ll * 1024ll) == "1 Mi");
   REQUIRE(util::binary_unit_string(1024ll * 1024ll * 1024ll) == "1 Gi");
   REQUIRE(util::binary_unit_string(1024ll * 1024ll * 1024ll * 1024ll) == "1 Ti");
   REQUIRE(util::binary_unit_string(1024ll * 1024ll * 1024ll * 1024ll * 1024ll) == "1 Pi");
   REQUIRE(util::binary_unit_string(1024ll * 1024ll * 1024ll * 1024ll * 1024ll * 1024ll) == "1 Ei");

   REQUIRE(util::binary_unit_string(2000) == "1.95 Ki");
}

#endif
