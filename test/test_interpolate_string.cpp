#ifdef BE_TEST

#include "interpolate_string.hpp"
#include <catch/catch.hpp>
#include <type_traits>

#define BE_CATCH_TAGS "[util][util:string]"

using namespace be;
using namespace be::util;

namespace {

auto functor = [](const S& str) { return str; };

struct Stateful {
   S operator()(const S&) {
      return std::to_string(++count);
   }
private:
   int count = 0;
};

} // ()

TEST_CASE("interpolate_string - Single replacement", BE_CATCH_TAGS) {
   REQUIRE(interpolate_string("$(asdf)", functor) == "asdf");
   REQUIRE(interpolate_string("aaa$(bbb)", functor) == "aaabbb");
   REQUIRE(interpolate_string<Stateful>("aaa$(bbb)") == "aaa1");
}

TEST_CASE("interpolate_string - Multiple replacement", BE_CATCH_TAGS) {
   REQUIRE(interpolate_string("$(asdf)$(iiii)", functor) == "asdfiiii");
   REQUIRE(interpolate_string("aaa$(bbb)aaa$(bbb) $(bbb)", functor) == "aaabbbaaabbb bbb");
   REQUIRE(interpolate_string<Stateful>("a$() $() $(asdf)") == "a1 2 3");
}

TEST_CASE("interpolate_string - Non-nesting", BE_CATCH_TAGS) {
   REQUIRE(interpolate_string("$(asdf$(iiii))", functor) == "asdf$(iiii)");
   REQUIRE(interpolate_string("$($()$())", functor) == "$()");
   REQUIRE(interpolate_string<Stateful>("$($()$())") == "12)");
}

TEST_CASE("interpolate_string - Escaping", BE_CATCH_TAGS) {
   REQUIRE(interpolate_string("$$ $$ $$", functor) == "$ $ $");
   REQUIRE(interpolate_string("$($$)", functor) == "$$");
   REQUIRE(interpolate_string<Stateful>("$$$()") == "$1");
}

TEST_CASE("interpolate_string - Incomplete var tag", BE_CATCH_TAGS) {
   REQUIRE(interpolate_string("Hello $(World", functor) == "Hello $(World");
   REQUIRE(interpolate_string("aaa$(bbb) $(", functor) == "aaabbb $(");
}

#endif
