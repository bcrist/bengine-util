#pragma once
#ifndef BE_UTIL_STRING_STRING_INTERNER_HPP_
#define BE_UTIL_STRING_STRING_INTERNER_HPP_

#include <be/core/be.hpp>
#include <be/core/alg.hpp>
#include <unordered_set>
#include <functional>

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
/// \brief  Provides interned versions of string views passed in.
///
/// \details The string views returned will remain valid until the
///         StringInterner is destroyed.  Interning string views with different
///         backing memory but equal content will result in an identical output
///         string view.
class StringInterner {
   struct segment {
      std::size_t size;
      std::size_t free;
      std::unique_ptr<char[]> data;
   };
public:
   void provisioning_policy(std::function<std::size_t(std::size_t)> func);

   void reserve(std::size_t bytes);

   be::SV operator()(be::SV str);

private:
   segment& reserve_(std::size_t bytes);

   std::unordered_set<be::SV> set_;
   std::vector<segment> segments_;
   std::function<std::size_t(std::size_t)> policy_;
};

} // be::util
} // be

#endif
