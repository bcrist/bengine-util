#ifdef BE_TEST

#include "line_endings.hpp"
#include <catch/catch.hpp>
#include <type_traits>

#define BE_CATCH_TAGS "[util][util:string]"

using namespace be;
using namespace be::util;
using namespace std::literals::string_view_literals;

bool test_normalize_newlines(SV input, SV expected) {
   S string(input);
   normalize_newlines(string);
   return string == expected;
}

TEST_CASE("normalize_newlines", BE_CATCH_TAGS) {
   REQUIRE(test_normalize_newlines("abcdef"sv, "abcdef"sv));
   REQUIRE(test_normalize_newlines("abc\0def"sv, "abc\0def"sv));

   REQUIRE(test_normalize_newlines("abc\ndef"sv, "abc\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\ndef\n"sv, "abc\ndef\n"sv));
   REQUIRE(test_normalize_newlines("abc\n\n\ndef"sv, "abc\n\n\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\n\nd\nef\n"sv, "abc\n\nd\nef\n"sv));
   REQUIRE(test_normalize_newlines("\n\n\n"sv, "\n\n\n"sv));

   REQUIRE(test_normalize_newlines("abc\rdef"sv, "abc\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\rdef\r"sv, "abc\ndef\n"sv));
   REQUIRE(test_normalize_newlines("abc\r\r\rdef"sv, "abc\n\n\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\r\rd\ref\r"sv, "abc\n\nd\nef\n"sv));
   REQUIRE(test_normalize_newlines("\r\r\r"sv, "\n\n\n"sv));

   REQUIRE(test_normalize_newlines("abc\r\ndef"sv, "abc\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\r\ndef\r\n"sv, "abc\ndef\n"sv));
   REQUIRE(test_normalize_newlines("abc\r\n\r\n\r\ndef"sv, "abc\n\n\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\r\n\r\nd\r\nef\r\n"sv, "abc\n\nd\nef\n"sv));
   REQUIRE(test_normalize_newlines("\r\n\r\n\r\n"sv, "\n\n\n"sv));

   REQUIRE(test_normalize_newlines("\rabc\ndef"sv, "\nabc\ndef"sv));
   REQUIRE(test_normalize_newlines("\r\nabc\rd\n"sv, "\nabc\nd\n"sv));
   REQUIRE(test_normalize_newlines("abc\r\r\ndef"sv, "abc\n\ndef"sv));
   REQUIRE(test_normalize_newlines("abc\n\r\nd\r\nef\n"sv, "abc\n\nd\nef\n"sv));
}

TEST_CASE("normalize_newlines_copy", BE_CATCH_TAGS) {
   REQUIRE(normalize_newlines_copy("abcdef"sv) == "abcdef"sv);
   REQUIRE(normalize_newlines_copy("abc\0def"sv) == "abc\0def"sv);

   REQUIRE(normalize_newlines_copy("abc\ndef"sv) == "abc\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\ndef\n"sv) == "abc\ndef\n"sv);
   REQUIRE(normalize_newlines_copy("abc\n\n\ndef"sv) == "abc\n\n\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\n\nd\nef\n"sv) == "abc\n\nd\nef\n"sv);
   REQUIRE(normalize_newlines_copy("\n\n\n"sv) == "\n\n\n"sv);

   REQUIRE(normalize_newlines_copy("abc\rdef"sv) == "abc\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\rdef\r"sv) == "abc\ndef\n"sv);
   REQUIRE(normalize_newlines_copy("abc\r\r\rdef"sv) == "abc\n\n\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\r\rd\ref\r"sv) == "abc\n\nd\nef\n"sv);
   REQUIRE(normalize_newlines_copy("\r\r\r"sv) == "\n\n\n"sv);

   REQUIRE(normalize_newlines_copy("abc\r\ndef"sv) == "abc\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\r\ndef\r\n"sv) == "abc\ndef\n"sv);
   REQUIRE(normalize_newlines_copy("abc\r\n\r\n\r\ndef"sv) == "abc\n\n\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\r\n\r\nd\r\nef\r\n"sv) == "abc\n\nd\nef\n"sv);
   REQUIRE(normalize_newlines_copy("\r\n\r\n\r\n"sv) == "\n\n\n"sv);

   REQUIRE(normalize_newlines_copy("\rabc\ndef"sv) == "\nabc\ndef"sv);
   REQUIRE(normalize_newlines_copy("\r\nabc\rd\n"sv) == "\nabc\nd\n"sv);
   REQUIRE(normalize_newlines_copy("abc\r\r\ndef"sv) == "abc\n\ndef"sv);
   REQUIRE(normalize_newlines_copy("abc\n\r\nd\r\nef\n"sv) == "abc\n\nd\nef\n"sv);
}

#endif
