#if !defined(BE_UTIL_STRING_INTERPOLATE_STRING_HPP_) && !defined(DOXYGEN)
#include "interpolate_string.hpp"
#elif !defined(BE_UTIL_STRING_INTERPOLATE_STRING_INL_)
#define BE_UTIL_STRING_INTERPOLATE_STRING_INL_

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
// Sigil Sigil -> Sigil
// Sigil '(' ([^)]*) ')' -> func($1)
// Sigil (any - (Sigil | ')')) -> $0
// !Sigil -> $0
template <typename F, char Sigil>
S interpolate_string(SV source, F func) {
   S out;

   auto begin = source.begin();
   auto end = source.end();
   auto it = begin;
   for (;;) {
      if (it == end) return S(source);
      if (*it == Sigil) break;
      ++it;
   }

   out.reserve(source.size());
   out.append(source.begin(), it);
   ++it;

   auto section_begin = it;

found_sigil:
   if (it == end) {
      out.append(1, Sigil);
      return out;
   }
   switch (*it) {
      case Sigil:
         // double sigil escape
         out.append(1, Sigil);
         ++it;
         section_begin = it;
         goto normal;

      case '(':
         // interpolant opener
         ++it;
         section_begin = it;
         goto in_interpolant;

      default:
         // unescaped literal sigil without a following '('
         out.append(1, Sigil);
         section_begin = it;
         goto normal;
   }

in_interpolant:
   if (it == end) {
      // malformed interpolant
      out.append(section_begin - 2, it);
      return out;
   } else if (*it == ')') {
      // interpolant closed; process it
      SV interpolant(source);
      interpolant.remove_prefix(section_begin - begin);
      interpolant.remove_suffix(end - it);
      out.append(func(interpolant));
      ++it;
      // since we might have inserted a lot, re-reserve enough space to hold the rest of the input
      out.reserve(end - it);
      section_begin = it;
      goto normal;
   } else {
      // still in interpolant
      ++it;
      goto in_interpolant;
   }
   
normal:
   if (it == end) {
      // normal EOF, append last normal section
      out.append(section_begin, it);
      return out;
   } else if (*it == Sigil) {
      // found a new sigil, append last normal section
      out.append(section_begin, it);
      ++it;
      goto found_sigil;
   } else {
      // still normal, keep looking for end of section
      ++it;
      goto normal;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename G, typename F, char Sigil>
void interpolate_string_ex(SV source, G noninterp_func, F interp_func) {
   auto begin = source.begin();
   auto end = source.end();
   auto it = begin;
   for (;;) {
      if (it == end) {
         noninterp_func(source);
         return;
      }
      if (*it == Sigil) {
         SV prefix(source);
         prefix.remove_suffix(end - it);
         noninterp_func(prefix);
         break;
      }
      ++it;
   }

   ++it;

   const char s = Sigil;
   SV sigil(&s, 1);

   auto section_begin = it;

found_sigil:
   if (it == end) {
      noninterp_func(sigil);
      return;
   }
   switch (*it) {
      case Sigil:
         // double sigil escape
         noninterp_func(sigil);
         ++it;
         section_begin = it;
         goto normal;

      case '(':
         // interpolant opener
         ++it;
         section_begin = it;
         goto in_interpolant;

      default:
         // unescaped literal sigil without a following '('
         noninterp_func(sigil);
         section_begin = it;
         goto normal;
   }

in_interpolant:
   if (it == end) {
      // malformed interpolant
      section_begin -= 2;
      SV section(source);
      section.remove_prefix(section_begin - begin);
      noninterp_func(section);
      return;
   } else if (*it == ')') {
      // interpolant closed; process it
      SV interpolant(source);
      interpolant.remove_prefix(section_begin - begin);
      interpolant.remove_suffix(end - it);
      interp_func(interpolant);
      ++it;
      section_begin = it;
      goto normal;
   } else {
      // still in interpolant
      ++it;
      goto in_interpolant;
   }
   
normal:
   if (it == end) {
      // normal EOF, append last normal section
      SV section(source);
      section.remove_prefix(section_begin - begin);
      noninterp_func(section);
      return;
   } else if (*it == Sigil) {
      // found a new sigil, append last normal section
      SV section(source);
      section.remove_prefix(section_begin - begin);
      section.remove_suffix(end - it);
      noninterp_func(section);
      ++it;
      goto found_sigil;
   } else {
      // still normal, keep looking for end of section
      ++it;
      goto normal;
   }
}

} // be::util

#endif
