#pragma once
#ifndef BE_UTIL_STRING_KEYWORD_PARSER_HPP_
#define BE_UTIL_STRING_KEYWORD_PARSER_HPP_

#include "string_span.hpp"
#include "parse_string_error.hpp"
#include <be/core/logging.hpp>
#include <be/core/stack_trace.hpp>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
struct DefaultKeywordTransform {
   S operator()(gsl::cstring_span<> input) const {
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

   S operator()(const char* input) const {
      return (*this)(gsl::ensure_z(input));
   }

   S operator()(const S& input) const {
      return (*this)(gsl::cstring_span<>(input));
   }
};

///////////////////////////////////////////////////////////////////////////////
struct DefaultKeywordEnumerator {
   std::vector<S> operator()(const S& keyword) const {
      std::vector<S> keywords;
      keywords.push_back(keyword);
      S transformed = keyword;
      transformed.erase(std::remove(transformed.begin(), transformed.end(), '_'), transformed.end());
      if (transformed != keyword) {
         keywords.push_back(std::move(transformed));
      }

      return keywords;
   }
};

///////////////////////////////////////////////////////////////////////////////
struct ExactKeywordTransform {
   S operator()(gsl::cstring_span<> input) const {
      return to_string(input);
   }

   S operator()(const char* input) const {
      return S(input);
   }

   S operator()(S input) const {
      return input;
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
            if (result.first->second != value) {
               be_warn() << "Keyword mapping collision!"
                  & attr(ids::log_attr_keyword) << S(key)
                  & attr(ids::log_attr_existing_value) << E(result.first->second)
                  & attr(ids::log_attr_new_value) << E(value)
                  & trace()
                  | default_log();
            }
         }
      }

      (*this)(value, std::forward<Ts>(args)...);
      return *this;
   }

   template <typename T>
   std::pair<E, ParseStringError> parse(const T& input) const {
      auto iter = mappings_.find(transform_(input));
      if (iter != mappings_.end()) {
         return std::make_pair(iter->second, ParseStringError::none);
      } else {
         return std::make_pair(default_value_, ParseStringError::syntax_error);
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
