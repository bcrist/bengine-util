#pragma once
#ifndef BE_UTIL_PRNG_SERVICE_XORSHIFT_1024_STAR_HPP_
#define BE_UTIL_PRNG_SERVICE_XORSHIFT_1024_STAR_HPP_

#include "util_prng_autolink.hpp"
#include "xorshift_1024_star.hpp"
#include <be/core/service.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<util::xs1024s> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<util::xs1024s> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<util::xs1024s> {
   const char* operator()() {
      return "xorshift1024*";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<util::xs1024s> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<util::xs1024s> {
   std::unique_ptr<util::xs1024s> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<util::xs1024s> {
   void operator()(std::unique_ptr<util::xs1024s>& service) {
      service.reset();
   }
};

} // be

#endif
