#include "pch.hpp"
#include "fnv.hpp"
#include <cassert>
#include <sstream>
#include <iomanip>

namespace be {
namespace util {
namespace {

///////////////////////////////////////////////////////////////////////////////
struct U256 {
   U32 d[8];    // Big-endian - d[0] is most significant
};

///////////////////////////////////////////////////////////////////////////////
void shift_left(U256& n, const int bits) {
   assert(bits <= 32);
   const int ibits = 32 - bits;

   for (int i = 0; i < 7; ++i) {
      n.d[i] <<= bits;
      n.d[i] |= n.d[i + 1] >> ibits;
   }
   n.d[7] <<= bits;
}

///////////////////////////////////////////////////////////////////////////////
void shift_left_160(U256& n) {
   n.d[0] = n.d[5];
   n.d[1] = n.d[6];
   n.d[2] = n.d[7];
   n.d[3] = 0;
   n.d[4] = 0;
   n.d[5] = 0;
   n.d[6] = 0;
   n.d[7] = 0;
}

///////////////////////////////////////////////////////////////////////////////
U32 accumulate(U256& a, const U256& b) {
   U64 c = 0;
   for (int i = 7; i >= 0; --i)
   {
      U64 nd = c + a.d[i] + b.d[i];
      a.d[i] = (U32)nd;
      c = nd >> 32;
   }
   return (U32)c;
}

///////////////////////////////////////////////////////////////////////////////
S u256_tostring(const U256& n) {
   std::ostringstream oss;
   oss << std::hex << std::setfill('0');

   for (int i = 0; i < 8; ++i)
      oss << std::setw(8) << n.d[i];

   return oss.str();
}

} // be::util::()

///////////////////////////////////////////////////////////////////////////////
S fnv256_0(gsl::cstring_span<> input) {
   U256 hash = { { } };
   for (auto i(input.begin()), end(input.end()); i != end; ++i) {
      U256 t = hash;
      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 4);
      accumulate(hash, t);

      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 2);
      accumulate(hash, t);

      shift_left_160(t);
      accumulate(hash, t);

      hash.d[7] ^= (U8)*i;
   }

   return u256_tostring(hash);
}

///////////////////////////////////////////////////////////////////////////////
S fnv256_1(gsl::cstring_span<> input) {
   U256 hash = { {
         0xdd268dbcu, 0xaac55036u, 0x2d98c384u, 0xc4e576ccu,
         0xc8b15368u, 0x47b6bbb3u, 0x1023b4c8u, 0xcaee0535u
      } };

   for (auto i(input.begin()), end(input.end()); i != end; ++i) {
      U256 t = hash;
      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 4);
      accumulate(hash, t);

      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 2);
      accumulate(hash, t);

      shift_left_160(t);
      accumulate(hash, t);

      hash.d[7] ^= (U8)*i;
   }

   return u256_tostring(hash);
}

///////////////////////////////////////////////////////////////////////////////
S fnv256_1a(gsl::cstring_span<> input) {
   U256 hash = { {
         0xdd268dbcu, 0xaac55036u, 0x2d98c384u, 0xc4e576ccu,
         0xc8b15368u, 0x47b6bbb3u, 0x1023b4c8u, 0xcaee0535u
      } };

   for (auto i(input.begin()), end(input.end()); i != end; ++i) {
      hash.d[7] ^= (U8)*i;

      U256 t = hash;
      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 4);
      accumulate(hash, t);

      shift_left(t, 1);
      accumulate(hash, t);

      shift_left(t, 2);
      accumulate(hash, t);

      shift_left_160(t);
      accumulate(hash, t);
   }

   return u256_tostring(hash);
}

} // be::util
} // be
