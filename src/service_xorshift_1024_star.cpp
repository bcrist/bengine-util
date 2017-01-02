#include "pch.hpp"
#include "service_xorshift_1024_star.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<rnd::xs1024s>::operator()() {
   service<rnd::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rnd::xs1024s> ServiceFactory<rnd::xs1024s>::operator()(Id) {
   auto ptr = std::make_unique<rnd::xs1024s>();
   ptr->seed(service<rnd::sm64>());
   return ptr;
};

} // be
