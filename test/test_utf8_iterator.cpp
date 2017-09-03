#ifdef BE_TEST

#include "utf8_iterator.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:string]"

using namespace be;

TEST_CASE("util::Utf8Iterator - basic functionality", BE_CATCH_TAGS) {
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::no_error) == 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::unexpected_continuation_byte) != 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::missing_continuation_byte) != 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::invalid_byte) != 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::overlong_encoding) != 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::surrogate_codepoint) != 0);
   REQUIRE(static_cast<int>(util::Utf8Iterator::error_type::invalid_codepoint) != 0);

   REQUIRE((std::is_same<std::iterator_traits<util::Utf8Iterator>::iterator_category, std::bidirectional_iterator_tag>::value ||
            std::is_same<std::iterator_traits<util::Utf8Iterator>::iterator_category, std::random_access_iterator_tag>::value) == true);

   REQUIRE((std::is_same<std::iterator_traits<util::Utf8Iterator>::value_type, char32_t>::value) == true);

   S s(" ");
   S s2("Test");
   util::Utf8Iterator it(s.begin());

   REQUIRE(it == s.begin());
   REQUIRE(it == s.cbegin());

   REQUIRE(it != s.end());
   REQUIRE(it != s.cend());

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   util::Utf8Iterator it2(it);
   REQUIRE(it2 == s.begin());

   it = s2.begin() + 1;
   REQUIRE(it == s2.begin() + 1);

   REQUIRE(it2 == s.begin());
   it2 = it;
   REQUIRE(it2 == s2.begin() + 1);
}

TEST_CASE("util::Utf8Iterator - valid UTF-8 input", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   s = "\x4D"; // U+004D
   it = s.begin();

   REQUIRE(*it == 0x4D);
   REQUIRE(*it == 0x4D);   // test again to ensure caching is working correctly.
   REQUIRE(it == s.begin());

   REQUIRE(it++ == s.begin());   // test postfix ++
   REQUIRE(it == s.end());

   REQUIRE(it-- == s.end());     // test postfix --
   REQUIRE(it == s.begin());
   REQUIRE(*it == 0x4D);

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   s = "\xD0\xB0";   // U+0430
   it = util::Utf8Iterator(s.begin());

   REQUIRE(*it == 0x430);
   REQUIRE(*it == 0x430);   // test again to ensure caching is working correctly.
   REQUIRE(it == s.begin());

   REQUIRE(++it == s.end());     // test prefix ++
   REQUIRE(it == s.end());

   REQUIRE(--it == s.begin());   // test prefix --
   REQUIRE(it == s.begin());
   REQUIRE(*it == 0x430);

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);


   s = "\xE4\xBA\x8C";  // U+4E8C
   it = s.begin();

   REQUIRE(*it == 0x4E8C);
   REQUIRE(*it == 0x4E8C);   // test again to ensure caching is working correctly.
   REQUIRE(it == s.begin());

   REQUIRE(++it == s.end());     // test prefix ++
   REQUIRE(it == s.end());

   REQUIRE(--it == s.begin());   // test prefix --
   REQUIRE(it == s.begin());
   REQUIRE(*it == 0x4E8C);

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);


   s = "\xF0\x90\x8C\x82"; // U+10302
   it = s.begin();

   REQUIRE(*it == 0x10302);
   REQUIRE(*it == 0x10302);   // test again to ensure caching is working correctly.
   REQUIRE(it == s.begin());

   REQUIRE(++it == s.end());     // test prefix ++
   REQUIRE(it == s.end());

   REQUIRE(--it == s.begin());   // test prefix --
   REQUIRE(it == s.begin());
   REQUIRE(*it == 0x10302);

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   s = "Testing 123...";
   std::size_t index = 0;
   for (util::Utf8Iterator i(s.begin()), end(s.end()); i != end; ++i) {
      REQUIRE(*i == (C32)s[index++]);
      REQUIRE(i.error() == util::Utf8Iterator::error_type::no_error);
   }

   for (util::Utf8Iterator i(s.end()), begin(s.begin()); i != begin; ) {
      REQUIRE(*(--i) == (C32)s[--index]);
      REQUIRE(i.error() == util::Utf8Iterator::error_type::no_error);
   }

   // U+00A3 ": 33" U+2030 U+266B "_" U+10ABCD
   s = "\xC2\xA3: 33\xE2\x80\xB0\xE2\x99\xAB_\xF4\x8A\xAF\x8D";
   it = s.begin();

   REQUIRE(*(it++) == 0x00A3);
   REQUIRE(*(it++) == ':');
   REQUIRE(*(it++) == ' ');
   REQUIRE(*(it++) == '3');
   REQUIRE(*(it++) == '3');
   REQUIRE(*(it++) == 0x2030);
   REQUIRE(*(it++) == 0x266B);
   REQUIRE(*(it++) == '_');
   REQUIRE(*(it++) == 0x10ABCD);

   REQUIRE(it == s.end());
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0x10ABCD);
   REQUIRE(*(--it) == '_');
   REQUIRE(*(--it) == 0x266B);
   REQUIRE(*(--it) == 0x2030);
   REQUIRE(*(--it) == '3');
   REQUIRE(*(--it) == '3');
   REQUIRE(*(--it) == ' ');
   REQUIRE(*(--it) == ':');
   REQUIRE(*(--it) == 0x00A3);

   REQUIRE(it == s.begin());
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
}

TEST_CASE("util::Utf8Iterator - extra continuation bytes", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   // ' ' at beginning prevents buffer underrun
   //       0x80   0x80   _   U+2030         0x80   U+266B         U+266B         0x80   0x81   0x82   0x80   0x8A
   s = " " "\x80" "\x80" "_" "\xE2\x80\xB0" "\x80" "\xE2\x99\xAB" "\xE2\x99\xAB" "\x80" "\x81" "\x82" "\x80" "\x8A";
   s.c_str(); // ensure string is null terminated.
   it = s.begin() + 1;  // skip initial space padding character

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(*(it++) == '_');
   REQUIRE(*(it++) == 0x2030);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(*(it++) == 0x266B);
   REQUIRE(*(it++) == 0x266B);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(it == s.end());

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(*(--it) == 0x266B);
   REQUIRE(*(--it) == 0x266B);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(*(--it) == 0x2030);
   REQUIRE(*(--it) == '_');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);

   REQUIRE(it == s.begin() + 1);
}

TEST_CASE("util::Utf8Iterator - missing continuation bytes", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   // ' ' at beginning prevents buffer underrun
   //       missing 1   A  missing 1   missing 2  _   missing 1      missing 2  missing 3
   s = " " "\xC2"      "A" "\xE2\xB0" "\xE2"     "_" "\xF4\x8A\xAF" "\xF4\x8A" "\xF4";
   s.c_str(); // ensure string is null terminated.
   it = s.begin() + 1;  // skip initial space padding character

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*(it++) == 'A');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*(it++) == '_');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(it == s.end());
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*(--it) == '_');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*(--it) == 'A');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(it == s.begin() + 1);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
}

TEST_CASE("util::Utf8Iterator - invalid bytes", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   // ' ' at beginning prevents buffer underrun
   //       0xF8   _   U+2030         0xF9   U+266B         0xFA   0xFB   0xFC   0xFD   0xFE   0xFF
   s = " " "\xF8" "_" "\xE2\x80\xB0" "\xF9" "\xE2\x99\xAB" "\xFA" "\xFB" "\xFC" "\xFD" "\xFE" "\xFF";
   s.c_str(); // ensure string is null terminated.
   it = s.begin() + 1;  // skip initial space padding character

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);

   REQUIRE(*(it++) == '_');
   REQUIRE(*(it++) == 0x2030);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);

   REQUIRE(*(it++) == 0x266B);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);

   REQUIRE(it == s.end());

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);


   REQUIRE(*(--it) == 0x266B);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);

   REQUIRE(*(--it) == 0x2030);
   REQUIRE(*(--it) == '_');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);

   REQUIRE(*(--it) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);

   REQUIRE(it == s.begin() + 1);
}

TEST_CASE("util::Utf8Iterator - overlong encodings", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   //   U+007F U+007F     U+007F         U+007F               U+07FF     U+07FF         U+07FF             U+FFFF         U+FFFF
   s = "\x7F" "\xC1\xBF" "\xE0\x81\xBF" "\xF0\x80\x81\xBF"   "\xDF\xBF" "\xE0\x9F\xBF" "\xF0\x80\x9F\xBF" "\xEF\xBF\xBF" "\xF0\x8F\xBF\xBF";
   it = s.begin();

   REQUIRE(*(it++) == 0x007F);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0x007F);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);
   REQUIRE(*(it++) == 0x007F);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);
   REQUIRE(*(it++) == 0x007F);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);

   REQUIRE(*(it++) == 0x07FF);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0x07FF);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);
   REQUIRE(*(it++) == 0x07FF);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);

   REQUIRE(*(it++) == 0xFFFF);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFF);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);

   // note overlong surrogate codepoints should set Error::OverlongEncoding, not Error::SurrogateCodepoint (see torture test).
}

TEST_CASE("util::Utf8Iterator - surrogate pair codepoints", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   //   U+D800         U+0020 U+DFFF
   s = "\xED\xA0\x80" " "    "\xED\xBF\xBF";
   it = s.begin();

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xD800);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::surrogate_codepoint);
   REQUIRE(*(it++) == ' ');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xDFFF);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::surrogate_codepoint);

   // note overlong surrogate codepoints should set Error::OverlongEncoding, not Error::SurrogateCodepoint (see torture test).
}

TEST_CASE("util::Utf8Iterator - invalid codepoints", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   //   U+110000           U+0020 U+1FFFFF
   s = "\xF4\x90\x80\x80" " "    "\xF7\xBF\xBF\xBF";
   it = s.begin();

   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_codepoint);
   REQUIRE(*(it++) == ' ');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::no_error);
   REQUIRE(*(it++) == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_codepoint);
}

TEST_CASE("util::Utf8Iterator - torture test", BE_CATCH_TAGS) {
   S s;
   util::Utf8Iterator it;

   // ' ' at beginning prevents buffer underrun
   //       0x80   0xF4   .   U+DFFF             0x80   0xFF   0xF088     ABC   U+4E80         U+1FFFFF
   s = " " "\x80" "\xF4" "." "\xF0\x8D\xBF\xBF" "\x80" "\xFF" "\xF0\x88" "ABC" "\xE4\xBA\x80" "\xF7\xBF\xBF\xBF";
   s.c_str(); // ensure string is null terminated.
   it = s.begin() + 1;  // skip initial space padding character

   REQUIRE(*it == 0xFFFD); ++it;
   REQUIRE(*it == 0xFFFD); ++it;
   // test multiple errors in a row without resetError() in between.
   REQUIRE(it.error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*it == '.'); ++it;
   // overlong surrogate codepoint
   REQUIRE(*it == 0xDFFF); ++it;
   REQUIRE(it.error() == util::Utf8Iterator::error_type::overlong_encoding);

   REQUIRE(*it == 0xFFFD); ++it;
   REQUIRE(it.error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*it == 0xFFFD); ++it;
   REQUIRE(it.error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*it == 0xFFFD); ++it;
   REQUIRE(it.error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*it == 'A'); ++it;
   REQUIRE(*it == 'B'); ++it;
   REQUIRE(*it == 'C'); ++it;
   REQUIRE(*it == 0x4E80); ++it;
   // resetError() hasn't been called, so it should still say Error::MissingContinuationByte even after some valid input.
   REQUIRE(it.error() == util::Utf8Iterator::error_type::missing_continuation_byte);

   REQUIRE(*it == 0xFFFD); ++it;
   REQUIRE(it.error() == util::Utf8Iterator::error_type::invalid_codepoint);

   REQUIRE(it == s.end());

   --it; REQUIRE(*it == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_codepoint);
   REQUIRE(*it == 0xFFFD);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::invalid_codepoint); // ensure errors are set if dereferenced twice but resetError is called in between
   --it; REQUIRE(*it == 0x4E80);
   REQUIRE(*it == 0x4E80);
   REQUIRE(*it == 0x4E80);
   REQUIRE(*it == 0x4E80);
   --it; REQUIRE(*it == 'C');
   REQUIRE(*it == 'C');
   REQUIRE(*it == 'C');
   REQUIRE(*it == 'C');
   --it; REQUIRE(*it == 'B');
   --it; REQUIRE(*it == 'A');
   REQUIRE(*it == 'A');
   REQUIRE(it.error() == util::Utf8Iterator::error_type::invalid_codepoint);
   --it; REQUIRE(*it == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   REQUIRE(*it == 0xFFFD);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::missing_continuation_byte);
   --it; REQUIRE(*it == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::invalid_byte);
   REQUIRE(*it == 0xFFFD);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::invalid_byte);
   --it; REQUIRE(*it == 0xFFFD);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   REQUIRE(*it == 0xFFFD);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
   --it; REQUIRE(*it == 0xDFFF);
   REQUIRE(it.reset_error() == util::Utf8Iterator::error_type::overlong_encoding);
   REQUIRE(*it == 0xDFFF);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::overlong_encoding);
   --it; REQUIRE(*it == '.');
   --it; REQUIRE(*it == 0xFFFD);
   --it; REQUIRE(*it == 0xFFFD);

   REQUIRE(it == s.begin() + 1);
   REQUIRE(it.error() == util::Utf8Iterator::error_type::unexpected_continuation_byte);
}

#endif
