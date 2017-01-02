#include "pch.hpp"
#include "service_xoroshiro_128_plus.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<rnd::xo128p>::operator()() {
   service<rnd::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rnd::xo128p> ServiceFactory<rnd::xo128p>::operator()(Id) {
   auto ptr = std::make_unique<rnd::xo128p>();
   ptr->seed(service<rnd::sm64>());
   return ptr;
};

} // be
