#ifdef BE_TEST

#include "base64_encode.hpp"
#include "base64_decode.hpp"
#include <catch/catch.hpp>

#define BE_CATCH_TAGS "[util][util:base64]"

using namespace be;

TEST_CASE("util::base64_encode", BE_CATCH_TAGS) {
   REQUIRE(util::base64_encode("") == "");
   REQUIRE(util::base64_encode("Hello World!") == "SGVsbG8gV29ybGQh");
   REQUIRE(util::base64_encode("3.14159265358979") == "My4xNDE1OTI2NTM1ODk3OQ==");
   REQUIRE(util::base64_encode("Lorem Ipsum dolor sit amet") == "TG9yZW0gSXBzdW0gZG9sb3Igc2l0IGFtZXQ=");
   REQUIRE(util::base64_encode("\xff\xff\xfe") == "///+");
}

TEST_CASE("util::base64_encode Buf<UC> input", BE_CATCH_TAGS) {
   auto buf = make_buf<UC>(256);
   for (size_t i = 0; i < 256; ++i) {
      buf[i] = (UC)i;
   }

   REQUIRE(util::base64_encode(tmp_buf(buf)) ==
           "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
           "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
           "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
           "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
           "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
           "8PHy8/T19vf4+fr7/P3+/w==");
}

TEST_CASE("util::base64_encode alternate S62/S63/P symbols", BE_CATCH_TAGS) {
   typedef std::string(*EncodeFunc)(const std::string& data);
   EncodeFunc f = util::base64_encode<'-', '|', '>'>;

   REQUIRE(f("Vacuums Suck.") == "VmFjdXVtcyBTdWNrLg>>");
   REQUIRE(f("\xff\xff\xfe") == "|||-");
}

TEST_CASE("util::base64_encode unpadded output", BE_CATCH_TAGS) {
   typedef std::string(*EncodeFunc)(const std::string& data);
   EncodeFunc f = util::base64_encode<'+', '/', 0>;

   REQUIRE(f("han shot first") == "aGFuIHNob3QgZmlyc3Q");
   REQUIRE(f("a") == "YQ");
}

TEST_CASE("util::base64_decode basic usage", BE_CATCH_TAGS) {
   REQUIRE(util::base64_decode_string("") == "");
   REQUIRE(util::base64_decode_string("SGVsbG8gV29ybGQh") == "Hello World!");
   REQUIRE(util::base64_decode_string("My4xNDE1OTI2NTM1ODk3OQ==") == "3.14159265358979");
   REQUIRE(util::base64_decode_string("TG9yZW0gSXBzdW0gZG9sb3Igc2l0IGFtZXQ=") == "Lorem Ipsum dolor sit amet");
   REQUIRE(util::base64_decode_string("///+") == "\xff\xff\xfe");
}

TEST_CASE("util::base64_decode Buf<UC> output", BE_CATCH_TAGS) {
   std::string encoded =
      "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
      "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
      "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
      "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
      "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
      "8PHy8/T19vf4+fr7/P3+/w==";

   Buf<UC> buf = util::base64_decode_buf(encoded);

   for (int i = 0; i < 256; ++i) {
      REQUIRE(buf[i] == UC(i));
   }
}

TEST_CASE("util::base64_decode unpadded input", BE_CATCH_TAGS) {
   REQUIRE(util::base64_decode_string("aGFuIHNob3QgZmlyc3Q") == "han shot first");
   REQUIRE(util::base64_decode_string("YQ") == "a");
}

TEST_CASE("util::base64_decode single extra symbol", BE_CATCH_TAGS) {
   REQUIRE(util::base64_decode_string("0") == "");
   REQUIRE(util::base64_decode_string("////A") == "\xff\xff\xff");
}

TEST_CASE("util::base64_decode non-symbol/padding characters", BE_CATCH_TAGS) {
   REQUIRE(util::base64_decode_string("SWYgeW91I   \t\n\nGdpdmUgYSBtb29zZSBhIG11\nZmZpbiwgaGUnbG (wgd2FudCBzb21) lIGphbSB0byBnbyB3aXRoIGl0Lg  = =") ==
           "If you give a moose a muffin, he'll want some jam to go with it.");
}

TEST_CASE("util::base64_decode premature padding", BE_CATCH_TAGS) {
   REQUIRE(util::base64_decode_string("Y2l0aWVzIHRvIHZhcG9yaX=pl") == "cities to vapori");
}

TEST_CASE("util::base64_decode alternate S62/S63/P symbols", BE_CATCH_TAGS) {
   typedef std::string(*DecodeFunc)(gsl::cstring_span<>);
   DecodeFunc f = util::base64_decode_string<'.', ',', '?'>;

   REQUIRE(f(",,,.") == "\xff\xff\xfe");
   REQUIRE(f("?AAAAaaaaa") == "");
}

#endif
