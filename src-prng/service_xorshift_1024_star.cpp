#include "pch.hpp"
#include "service_xorshift_1024_star.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<util::xs1024s>::operator()() {
   service<util::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<util::xs1024s> ServiceFactory<util::xs1024s>::operator()(Id) {
   auto ptr = std::make_unique<util::xs1024s>();
   ptr->seed(service<util::sm64>());
   return ptr;
};

} // be
