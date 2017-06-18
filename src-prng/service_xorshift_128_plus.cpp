#include "pch.hpp"
#include "service_xorshift_128_plus.hpp"
#include "service_split_mix_64.hpp"
#include <be/core/service_helpers.hpp>

namespace be {

///////////////////////////////////////////////////////////////////////////////
void ServiceInitDependencies<util::xs128p>::operator()() {
   service<util::sm64>();
}

///////////////////////////////////////////////////////////////////////////////
std::unique_ptr<util::xs128p> ServiceFactory<util::xs128p>::operator()(Id) {
   auto ptr = std::make_unique<util::xs128p>();
   ptr->seed(service<util::sm64>());
   return ptr;
};

} // be
