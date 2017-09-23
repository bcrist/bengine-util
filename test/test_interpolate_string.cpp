#ifdef BE_TEST

#include "interpolate_string.hpp"
#include <catch/catch.hpp>
#include <type_traits>

#define BE_CATCH_TAGS "[util][util:string]"

using namespace be;
using namespace be::util;

namespace {

auto functor = [](SV str) { return str; };

struct Stateful {
   S operator()(SV) {
      return std::to_string(++count);
   }
private:
   int count = 0;
};

bool test_interpolate_string_ex(SV input, int interp, int noninterp) {
   int i = 0, n = 0;
   interpolate_string_ex(input, [&](SV) { ++n; }, [&](SV) { ++i; });
   return i == interp && n == noninterp;
}

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

TEST_CASE("interpolate_string_ex", BE_CATCH_TAGS) {
   REQUIRE(test_interpolate_string_ex("$(asdf)", 1, 2));
   REQUIRE(test_interpolate_string_ex("aaa$(bbb)", 1, 2));
   
   REQUIRE(test_interpolate_string_ex("$(asdf)$(iiii)", 2, 3));
   REQUIRE(test_interpolate_string_ex("aaa$(bbb)aaa$(bbb) $(bbb)", 3, 4));
   REQUIRE(test_interpolate_string_ex("a$() $() $(asdf)", 3, 4));

   REQUIRE(test_interpolate_string_ex("$(asdf$(iiii))", 1, 2));
   REQUIRE(test_interpolate_string_ex("$($()$())", 2, 3));

   REQUIRE(test_interpolate_string_ex("$$ $$ $$", 0, 7));
   REQUIRE(test_interpolate_string_ex("$($$)", 1, 2));
   REQUIRE(test_interpolate_string_ex("$$$()", 1, 4));

   REQUIRE(test_interpolate_string_ex("Hello $(World", 0, 2));
   REQUIRE(test_interpolate_string_ex("aaa$(bbb) $(", 1, 3));
}

#endif
