#pragma once
#ifndef BE_UTIL_SERVICE_SPLIT_MIX_64_HPP_
#define BE_UTIL_SERVICE_SPLIT_MIX_64_HPP_

#include <be/core/service.hpp>
#include "split_mix_64.hpp"

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<rnd::sm64> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<rnd::sm64> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<rnd::sm64> {
   const char* operator()() {
      return "SplitMix64";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<rnd::sm64> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<rnd::sm64> {
   std::unique_ptr<rnd::sm64> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<rnd::sm64> {
   void operator()(std::unique_ptr<rnd::sm64>& service) {
      service.reset();
   }
};

} // be

#endif
