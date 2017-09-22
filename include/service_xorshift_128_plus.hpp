#pragma once
#ifndef BE_UTIL_PRNG_SERVICE_XORSHIFT_128_PLUS_HPP_
#define BE_UTIL_PRNG_SERVICE_XORSHIFT_128_PLUS_HPP_

#include "xorshift_128_plus.hpp"
#include <be/core/service.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<util::xs128p> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<util::xs128p> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<util::xs128p> {
   const char* operator()() {
      return "xorshift128+";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<util::xs128p> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<util::xs128p> {
   std::unique_ptr<util::xs128p> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<util::xs128p> {
   void operator()(std::unique_ptr<util::xs128p>& service) {
      service.reset();
   }
};

} // be

#endif
