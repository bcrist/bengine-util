#include "pch.hpp"
#include "service_split_mix_64.hpp"
#include "fnv.hpp"
#include <be/core/service_helpers.hpp>
#include <be/core/service_log.hpp>
#include <be/core/time.hpp>
#include <thread>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<rnd::sm64>::operator()() {
   default_log();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rnd::sm64> ServiceFactory<rnd::sm64>::operator()(Id) {
   U64 hash = util::detail::FnvBasis<U64>::value;
   {
      // thread ID
      auto ent = std::hash<std::thread::id>()(std::this_thread::get_id());
      const char* begin = reinterpret_cast<const char*>(&ent);
      const char* end = begin + sizeof(ent);
      hash = util::fnv1a(hash, begin, end);
   }
   {
      // stack address
      void* ent = &hash;
      const char* begin = reinterpret_cast<const char*>(&ent);
      const char* end = begin + sizeof(ent);
      hash = util::fnv1a(hash, begin, end);
   }
   {
      // timestamp
      auto ent = ts_now().count();
      const char* begin = reinterpret_cast<const char*>(&ent);
      const char* end = begin + sizeof(ent);
      hash = util::fnv1a(hash, begin, end);
   }
   do {
      // perf timestamp
      auto ent = perf_now();
      const char* begin = reinterpret_cast<const char*>(&ent);
      const char* end = begin + sizeof(ent);
      hash = util::fnv1a(hash, begin, end);
   } while (hash == 0);

   return std::make_unique<rnd::sm64>(hash);
};

} // be
