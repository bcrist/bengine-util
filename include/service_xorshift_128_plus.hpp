#pragma once
#ifndef BE_UTIL_SERVICE_XORSHIFT_128_PLUS_HPP_
#define BE_UTIL_SERVICE_XORSHIFT_128_PLUS_HPP_

#include <be/core/service.hpp>
#include "xorshift_128_plus.hpp"

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<rnd::xs128p> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<rnd::xs128p> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<rnd::xs128p> {
   const char* operator()() {
      return "xorshift128+";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<rnd::xs128p> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<rnd::xs128p> {
   std::unique_ptr<rnd::xs128p> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<rnd::xs128p> {
   void operator()(std::unique_ptr<rnd::xs128p>& service) {
      service.reset();
   }
};

} // be

#endif
