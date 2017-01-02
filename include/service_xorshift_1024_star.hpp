#pragma once
#ifndef BE_UTIL_SERVICE_XORSHIFT_1024_STAR_HPP_
#define BE_UTIL_SERVICE_XORSHIFT_1024_STAR_HPP_

#include <be/core/service.hpp>
#include "xorshift_1024_star.hpp"

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<rnd::xs1024s> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<rnd::xs1024s> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<rnd::xs1024s> {
   const char* operator()() {
      return "xorshift1024*";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<rnd::xs1024s> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<rnd::xs1024s> {
   std::unique_ptr<rnd::xs1024s> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<rnd::xs1024s> {
   void operator()(std::unique_ptr<rnd::xs1024s>& service) {
      service.reset();
   }
};

} // be

#endif
