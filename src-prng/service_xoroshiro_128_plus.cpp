#include "pch.hpp"
#include "service_xoroshiro_128_plus.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<util::xo128p>::operator()() {
   service<util::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<util::xo128p> ServiceFactory<util::xo128p>::operator()(Id) {
   auto ptr = std::make_unique<util::xo128p>();
   ptr->seed(service<util::sm64>());
   return ptr;
};

} // be
