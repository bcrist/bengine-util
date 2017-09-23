#pragma once
#ifndef BE_UTIL_VERSION_HPP_
#define BE_UTIL_VERSION_HPP_

#include <be/core/macros.hpp>

#define BE_UTIL_VERSION_MAJOR 0
#define BE_UTIL_VERSION_MINOR 1
#define BE_UTIL_VERSION_REV 16

/*!! include('common/version', 'be::util') !! 6 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */
#define BE_UTIL_VERSION (BE_UTIL_VERSION_MAJOR * 100000 + BE_UTIL_VERSION_MINOR * 1000 + BE_UTIL_VERSION_REV)
#define BE_UTIL_VERSION_STRING "be::util " BE_STRINGIFY(BE_UTIL_VERSION_MAJOR) "." BE_STRINGIFY(BE_UTIL_VERSION_MINOR) "." BE_STRINGIFY(BE_UTIL_VERSION_REV)

/* ######################### END OF GENERATED CODE ######################### */

#endif
