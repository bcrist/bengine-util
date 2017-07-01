#include "pch.hpp"
#include "zlib_result_code.hpp"

/*!! include 'zlib_result_code' !! 89 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
bool is_valid(ZlibResultCode constant) noexcept {
   switch (constant) {
      case ZlibResultCode::ok:
      case ZlibResultCode::stream_end:
      case ZlibResultCode::need_dictionary:
      case ZlibResultCode::io_error:
      case ZlibResultCode::stream_error:
      case ZlibResultCode::data_error:
      case ZlibResultCode::not_enough_memory:
      case ZlibResultCode::buffer_error:
      case ZlibResultCode::version_mismatch:
         return true;
      default:
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* zlib_result_code_name(ZlibResultCode constant) noexcept {
   switch (constant) {
      case ZlibResultCode::ok:                return "ok";
      case ZlibResultCode::stream_end:        return "stream_end";
      case ZlibResultCode::need_dictionary:   return "need_dictionary";
      case ZlibResultCode::io_error:          return "io_error";
      case ZlibResultCode::stream_error:      return "stream_error";
      case ZlibResultCode::data_error:        return "data_error";
      case ZlibResultCode::not_enough_memory: return "not_enough_memory";
      case ZlibResultCode::buffer_error:      return "buffer_error";
      case ZlibResultCode::version_mismatch:  return "version_mismatch";
      default:
         return "?";
   }
}

///////////////////////////////////////////////////////////////////////////////
std::array<const ZlibResultCode, 9> zlib_result_code_values() noexcept {
   return ::be::EnumTraits<ZlibResultCode>::values<>();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, ZlibResultCode constant) {
   if (is_valid(constant)) {
      os << zlib_result_code_name(constant);
   } else {
      os << static_cast<I64>(static_cast<I8>(constant));
   }
   return os;
}

///////////////////////////////////////////////////////////////////////////////
int generic_error_condition(ZlibResultCode constant) noexcept {
   switch (constant) {
      case ZlibResultCode::io_error:          return static_cast<int>(std::errc::io_error);
      case ZlibResultCode::stream_error:      return static_cast<int>(std::errc::invalid_argument);
      case ZlibResultCode::data_error:        return static_cast<int>(std::errc::illegal_byte_sequence);
      case ZlibResultCode::not_enough_memory: return static_cast<int>(std::errc::not_enough_memory);
      case ZlibResultCode::buffer_error:      return static_cast<int>(std::errc::no_message_available);
      default:
         return 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* result_code_msg(ZlibResultCode constant) noexcept {
   switch (constant) {
      case ZlibResultCode::ok:                return "Operation suceeded";
      case ZlibResultCode::stream_end:        return "End of input reached";
      case ZlibResultCode::need_dictionary:   return "Stream inflation requires dictionary";
      case ZlibResultCode::io_error:          return "An error occurred in a platform I/O library function";
      case ZlibResultCode::stream_error:      return "Invalid argument or stream state";
      case ZlibResultCode::data_error:        return "Input is corrupted or not compressed";
      case ZlibResultCode::not_enough_memory: return "Failed to allocate memory";
      case ZlibResultCode::buffer_error:      return "No data available in buffer";
      case ZlibResultCode::version_mismatch:  return "Linked zlib binary version doesn't match header";
      default:
         return "An unknown error occurred";
   }
}

} // be::util


/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
S ZlibResultCodeErrorCategory::message(int condition) const {
   return result_code_msg(zlib_result_code(condition));
}

///////////////////////////////////////////////////////////////////////////////
bool ZlibResultCodeErrorCategory::equivalent(int code, const std::error_condition& condition) const noexcept {
   if (condition.category() == std::generic_category() &&
       condition.value() == generic_error_condition(zlib_result_code(code))) {
      return true;
   }

   if (condition.category() == zlib_result_code_error_category() &&
       condition.value() == code) {
      return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
const std::error_category& zlib_result_code_error_category() {
   static ZlibResultCodeErrorCategory instance;
   return instance;
}

///////////////////////////////////////////////////////////////////////////////
std::error_code make_error_code(ZlibResultCode e) {
   return std::error_code(static_cast<int>(e), zlib_result_code_error_category());
}

///////////////////////////////////////////////////////////////////////////////
std::error_condition make_error_condition(ZlibResultCode e) {
   return std::error_condition(static_cast<int>(e), zlib_result_code_error_category());
}

///////////////////////////////////////////////////////////////////////////////
ZlibResultCode zlib_result_code(int result) {
   return static_cast<ZlibResultCode>(result);
}

} // be::belua
