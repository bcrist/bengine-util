#pragma once
#ifndef BE_UTIL_PRNG_SERVICE_SPLIT_MIX_64_HPP_
#define BE_UTIL_PRNG_SERVICE_SPLIT_MIX_64_HPP_

#include "split_mix_64.hpp"
#include <be/core/service.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct SuppressUndefinedService<util::sm64> : True { };

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceTraits<util::sm64> : ServiceTraits<> {
   using thread_local_manager = yes;
   using lazy_ids = yes;
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceName<util::sm64> {
   const char* operator()() {
      return "SplitMix64";
   }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceInitDependencies<util::sm64> {
   void operator()();
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceFactory<util::sm64> {
   std::unique_ptr<util::sm64> operator()(Id);
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct ServiceCleanup<util::sm64> {
   void operator()(std::unique_ptr<util::sm64>& service) {
      service.reset();
   }
};

} // be

#endif
