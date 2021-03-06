#pragma once
#ifndef BE_UTIL_STRING_KEYWORD_PARSER_HPP_
#define BE_UTIL_STRING_KEYWORD_PARSER_HPP_

#include "parse_string_error_condition.hpp"
#include "trim.hpp"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
struct DefaultKeywordTransform {
   S operator()(SV input) const {
      S transformed;
      input = trim(input);
      transformed.assign(input.size(), '\0');
      std::transform(input.begin(), input.end(), transformed.begin(), [](char c) {
            if (c == '-' || c == ' ') {
               return '_';
            }
            return to_lower(c);
         });
      return transformed;
   }
};

///////////////////////////////////////////////////////////////////////////////
struct DefaultKeywordEnumerator {
   std::vector<S> operator()(SV keyword) const {
      std::vector<S> keywords;
      keywords.push_back(S(keyword));
      S transformed = S(keyword);
      transformed.erase(std::remove(transformed.begin(), transformed.end(), '_'), transformed.end());
      if (transformed != keyword) {
         keywords.push_back(std::move(transformed));
      }

      return keywords;
   }
};

///////////////////////////////////////////////////////////////////////////////
struct ExactKeywordTransform {
   S operator()(SV input) const {
      return S(input);
   }
};

///////////////////////////////////////////////////////////////////////////////
struct ExactKeywordEnumerator {
   std::array<S, 1> operator()(const S& keyword) const {
      return { keyword };
   }
};

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
template <typename E = I32, typename F = detail::DefaultKeywordTransform, typename G = detail::DefaultKeywordEnumerator>
class KeywordParser {
public:
   KeywordParser(E default_value = E(), F transform = F(), G enumerate = G())
      : default_value_(std::move(default_value)),
        transform_(std::move(transform)),
        enumerate_(std::move(enumerate)) { }

   KeywordParser<E, F, G>& reset(E default_value = E(), F transform = F(), G enumerate = G()) {
      using std::swap;
      mappings_.clear();
      swap(default_value_, default_value);
      swap(transform_, transform);
      swap(enumerate_, enumerate);
      return *this;
   }

   KeywordParser<E, F, G>& set_default(E default_value = E()) {
      default_value_ = std::move(default_value);
      return *this;
   }

   template <typename T, typename... Ts>
   KeywordParser<E, F, G>& operator()(E value, const T& keyword, Ts&&... args) {
      auto results = enumerate_(transform_(keyword));
      for (auto& key : results) {
         auto result = mappings_.emplace(key, value);
         if (!result.second) {
            assert(result.first->second == value);
         }
      }

      (*this)(value, std::forward<Ts>(args)...);
      return *this;
   }

   template <typename T>
   E parse(const T& input) const {
      auto iter = mappings_.find(transform_(input));
      if (iter != mappings_.end()) {
         return iter->second;
      } else {
         return default_value_;
      }
   }

   template <typename T>
   E parse(const T& input, std::error_code& ec) const {
      auto iter = mappings_.find(transform_(input));
      if (iter != mappings_.end()) {
         return iter->second;
      } else {
         ec = make_error_code(ParseStringErrorCondition::syntax_error);
         return default_value_;
      }
   }

private:
   void operator()(E) { } // terminates variadic args

   std::unordered_map<S, E> mappings_;
   E default_value_;
   F transform_;
   G enumerate_;
};

///////////////////////////////////////////////////////////////////////////////
template <typename E = int, typename F = detail::ExactKeywordTransform>
using ExactKeywordParser = KeywordParser<E, F, detail::ExactKeywordEnumerator>;

} // be::util

#endif
