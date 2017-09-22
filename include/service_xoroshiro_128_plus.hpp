#pragma once
#ifndef BE_UTIL_PRNG_SERVICE_XOROSHIRO_128_PLUS_HPP_
#define BE_UTIL_PRNG_SERVICE_XOROSHIRO_128_PLUS_HPP_

#include "xoroshiro_128_plus.hpp"
#include <be/core/service.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<util::xo128p> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<util::xo128p> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<util::xo128p> {
   const char* operator()() {
      return "xoroshiro128+";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<util::xo128p> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<util::xo128p> {
   std::unique_ptr<util::xo128p> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<util::xo128p> {
   void operator()(std::unique_ptr<util::xo128p>& service) {
      service.reset();
   }
};

} // be

#endif
