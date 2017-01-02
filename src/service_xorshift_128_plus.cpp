#include "pch.hpp"
#include "service_xorshift_128_plus.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<rnd::xs128p>::operator()() {
   service<rnd::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rnd::xs128p> ServiceFactory<rnd::xs128p>::operator()(Id) {
   auto ptr = std::make_unique<rnd::xs128p>();
   ptr->seed(service<rnd::sm64>());
   return ptr;
};

} // be
