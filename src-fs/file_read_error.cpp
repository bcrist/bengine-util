#include "pch.hpp"
#include "file_read_error.hpp"

/*!! include 'file_read_error' !! 51 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
bool is_valid(FileReadError constant) noexcept {
   switch (constant) {
      case FileReadError::none:
      case FileReadError::unknown_filesystem_error:
      case FileReadError::file_not_found:
      case FileReadError::file_not_readable:
      case FileReadError::out_of_memory:
         return true;
      default:
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* file_read_error_name(FileReadError constant) noexcept {
   switch (constant) {
      case FileReadError::none:                     return "none";
      case FileReadError::unknown_filesystem_error: return "unknown_filesystem_error";
      case FileReadError::file_not_found:           return "file_not_found";
      case FileReadError::file_not_readable:        return "file_not_readable";
      case FileReadError::out_of_memory:            return "out_of_memory";
      default:
         return "?";
   }
}

///////////////////////////////////////////////////////////////////////////////
std::array<const FileReadError, 5> file_read_error_values() noexcept {
   return ::be::EnumTraits<FileReadError>::values<>();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, FileReadError constant) {
   if (is_valid(constant)) {
      os << file_read_error_name(constant);
   } else {
      os << static_cast<I64>(static_cast<U8>(constant));
   }
   return os;
}

} // be::util


/* ######################### END OF GENERATED CODE ######################### */
