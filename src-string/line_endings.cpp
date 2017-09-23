#include "pch.hpp"
#include "line_endings.hpp"

namespace be::util {

//////////////////////////////////////////////////////////////////////////////
void normalize_newlines(S& string) {
   auto it = string.begin();
   auto end = string.end();
   for (;;) {
      if (it == end) return;
      if (*it == '\r') break;
      ++it;
   }
   *it = '\n';
   bool skip_lf = true;
   ++it;
   auto d = it;
   while (it != end) {
      switch (*it) {
         case '\n':
            if (skip_lf) {
               skip_lf = false;
            } else {
               *d = '\n';
               ++d;
            }
            break;
         case '\r':
            *d = '\n';
            ++d;
            skip_lf = true;
            break;
         default:
            *d = *it;
            ++d;
            skip_lf = false;
            break;
      }
      ++it;
   }
   if (d != end) {
      string.resize(d - string.begin());
   }
}

//////////////////////////////////////////////////////////////////////////////
S normalize_newlines_copy(SV string) {
   S result;
   result.reserve(string.size());

   auto it = string.begin();
   auto end = string.end();
   for (;;) {
      if (it == end) return S(string);
      if (*it == '\r') break;
      ++it;
   }
   result.append(string.begin(), it);
   result.append(1, '\n');
   ++it;
   bool skip_lf = true;
   auto start = it;
   while (it != end) {
      switch (*it) {
         case '\n':
            if (skip_lf) {
               skip_lf = false;
               ++start;
            }
            break;
         case '\r':
            result.append(start, it);
            result.append(1, '\n');
            start = it + 1;
            skip_lf = true;
            break;
         default:
            skip_lf = false;
            break;
      }
      ++it;
   }
   result.append(start, end);
   return result;
}

} // be::util
