#ifndef BE_UTIL_IIRF_HPP
#define BE_UTIL_IIRF_HPP

#include <be/core/be.hpp>

namespace be::util {

template <typename T, std::size_t DivisorBits = 4>
struct FastDiscreteInfiniteImpulseResponseFilter {
   T operator()(T input, T last_output) {
      return (input >> DivisorBits) + (last_output - (last_output >> Divisorbits));
   }
};

} // be::util

#endif
