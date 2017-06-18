#if !defined(BE_UTIL_STRING_INTERPOLATE_STRING_HPP_) && !defined(DOXYGEN)
#include "interpolate_string.hpp"
#elif !defined(BE_UTIL_STRING_INTERPOLATE_STRING_INL_)
#define BE_UTIL_STRING_INTERPOLATE_STRING_INL_

namespace be::util {
namespace detail {

template <char Sigil>
S string_interpolation_sigil_parsing_regex_source() {
   S base = "[X]{2}|[X]\\(([^\\)]*)\\)";

   // replace X's with Sigil
   base[1] = Sigil;
   base[8] = Sigil;

   return base;
}

template <char Sigil>
std::regex& string_interpolation_sigil_parsing_regex() {
   static std::regex re(string_interpolation_sigil_parsing_regex_source<Sigil>());
   return re;
}

} // be::detail

///////////////////////////////////////////////////////////////////////////////
template <typename F, char Sigil>
S interpolate_string(const S& source, F func) {
   S out;
   out.reserve(source.size());

   std::regex& re = detail::string_interpolation_sigil_parsing_regex<Sigil>();
   std::smatch match;
   auto i = source.begin(), end = source.end();
   while (i != end && std::regex_search(i, end, match, re)) {
      out.append(match.prefix().first, match.prefix().second);

      if (match[1].matched) {
         out.append(func(match[1].str()));
      } else {
         out.append(1, Sigil);
      }

      i = match.suffix().first;
   }

   if (i != end) {
      out.append(i, end);
   }

   return out;
}

///////////////////////////////////////////////////////////////////////////////
template <typename G, typename F, char Sigil>
void interpolate_string_ex(const S& source, G noninterp_func, F interp_func) {
   std::regex& re = detail::string_interpolation_sigil_parsing_regex<Sigil>();
   std::smatch match;
   auto i = source.begin(), end = source.end();
   while (i != end && std::regex_search(i, end, match, re)) {
      noninterp_func(S(match.prefix()));

      if (match[1].matched) {
         interp_func(match[1].str());
      } else {
         noninterp_func(S(1, Sigil));
      }

      i = match.suffix().first;
   }

   if (i != end) {
      noninterp_func(S(i, end));
   }
}

} // be::util

#endif
