#pragma once
#ifndef BE_UTIL_SERVICE_XOROSHIRO_128_PLUS_HPP_
#define BE_UTIL_SERVICE_XOROSHIRO_128_PLUS_HPP_

#include <be/core/service.hpp>
#include "xoroshiro_128_plus.hpp"

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<rnd::xo128p> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<rnd::xo128p> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<rnd::xo128p> {
   const char* operator()() {
      return "xoroshiro128+";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<rnd::xo128p> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<rnd::xo128p> {
   std::unique_ptr<rnd::xo128p> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<rnd::xo128p> {
   void operator()(std::unique_ptr<rnd::xo128p>& service) {
      service.reset();
   }
};

} // be

#endif
