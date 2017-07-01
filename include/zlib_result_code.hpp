#pragma once
#ifndef BE_UTIL_COMPRESSION_ZLIB_RESULT_CODE_HPP_
#define BE_UTIL_COMPRESSION_ZLIB_RESULT_CODE_HPP_

#include "util_compression_autolink.hpp"
#include <be/core/enum_traits.hpp>
#include <zlib/zlib.h>

/*!! include 'zlib_result_code' !! 64 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
enum class ZlibResultCode : I8 {
   ok = Z_OK,
   stream_end = Z_STREAM_END,
   need_dictionary = Z_NEED_DICT,
   io_error = Z_ERRNO,
   stream_error = Z_STREAM_ERROR,
   data_error = Z_DATA_ERROR,
   not_enough_memory = Z_MEM_ERROR,
   buffer_error = Z_BUF_ERROR,
   version_mismatch = Z_VERSION_ERROR
};

bool is_valid(ZlibResultCode constant) noexcept;
const char* zlib_result_code_name(ZlibResultCode constant) noexcept;
std::array<const ZlibResultCode, 9> zlib_result_code_values() noexcept;
std::ostream& operator<<(std::ostream& os, ZlibResultCode constant);
int generic_error_condition(ZlibResultCode constant) noexcept;
const char* result_code_msg(ZlibResultCode constant) noexcept;

} // be::util

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct EnumTraits<::be::util::ZlibResultCode> {
   using type = ::be::util::ZlibResultCode;
   using underlying_type = typename std::underlying_type<type>::type;

   static constexpr std::size_t count = 9;

   static bool is_valid(type value) {
      return ::be::util::is_valid(value);
   }

   static const char* name(type value) {
      return ::be::util::zlib_result_code_name(value);
   }

   template <typename C = std::array<const type, count>>
   static C values() {
      return {
         ::be::util::ZlibResultCode::ok,
         ::be::util::ZlibResultCode::stream_end,
         ::be::util::ZlibResultCode::need_dictionary,
         ::be::util::ZlibResultCode::io_error,
         ::be::util::ZlibResultCode::stream_error,
         ::be::util::ZlibResultCode::data_error,
         ::be::util::ZlibResultCode::not_enough_memory,
         ::be::util::ZlibResultCode::buffer_error,
         ::be::util::ZlibResultCode::version_mismatch,
      };
   }
};

} // be

/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
class ZlibResultCodeErrorCategory : public std::error_category {
public:
   virtual const char* name() const noexcept { return "zlib Result Code"; }
   virtual S message(int condition) const;
   virtual bool equivalent(int code, const std::error_condition& condition) const noexcept;
};

const std::error_category& zlib_result_code_error_category();
std::error_code make_error_code(ZlibResultCode e);
std::error_condition make_error_condition(ZlibResultCode e);
ZlibResultCode zlib_result_code(int result);

} // be::util

namespace std {

///////////////////////////////////////////////////////////////////////////////
template <>
struct is_error_code_enum<be::util::ZlibResultCode> : std::true_type { };

} // std

#endif
