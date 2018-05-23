#include "pch.hpp"
#include "string_interner.hpp"

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
void StringInterner::provisioning_policy(std::function<std::size_t(std::size_t)> func) {
   policy_ = std::move(func);
}

///////////////////////////////////////////////////////////////////////////////
void StringInterner::reserve(std::size_t bytes) {
   reserve_(bytes);
}

///////////////////////////////////////////////////////////////////////////////
be::SV StringInterner::operator()(be::SV str) {
   auto it = set_.find(str);
   if (it == set_.end()) {
      std::size_t reserve_amount = str.length();
      if (policy_) {
         reserve_amount = be::max(reserve_amount, policy_(reserve_amount));
      }
      segment& seg = reserve_(reserve_amount);
      char* start = seg.data.get() + seg.size - seg.free;
      std::memcpy(start, str.data(), str.length());
      seg.free -= str.length();
      be::SV value = be::SV(start, str.length());
      set_.insert(value);
      return value;
   }
   return *it;
}

///////////////////////////////////////////////////////////////////////////////
StringInterner::segment& StringInterner::reserve_(std::size_t bytes) {
   for (auto& seg : segments_) {
      if (seg.free >= bytes) {
         return seg;
      }
   }
   segments_.push_back(segment { bytes, bytes, std::make_unique<char[]>(bytes) });
   return segments_.back();
}

} // be::util
} // be
