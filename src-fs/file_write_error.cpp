#include "pch.hpp"
#include "file_write_error.hpp"

/*!! include 'file_write_error' !! 53 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
bool is_valid(FileWriteError constant) noexcept {
   switch (constant) {
      case FileWriteError::none:
      case FileWriteError::unknown_filesystem_error:
      case FileWriteError::failed_to_create_file:
      case FileWriteError::file_not_writable:
      case FileWriteError::disk_full:
      case FileWriteError::out_of_memory:
         return true;
      default:
         return false;
   }
}

///////////////////////////////////////////////////////////////////////////////
const char* file_write_error_name(FileWriteError constant) noexcept {
   switch (constant) {
      case FileWriteError::none:                     return "none";
      case FileWriteError::unknown_filesystem_error: return "unknown_filesystem_error";
      case FileWriteError::failed_to_create_file:    return "failed_to_create_file";
      case FileWriteError::file_not_writable:        return "file_not_writable";
      case FileWriteError::disk_full:                return "disk_full";
      case FileWriteError::out_of_memory:            return "out_of_memory";
      default:
         return "?";
   }
}

///////////////////////////////////////////////////////////////////////////////
std::array<const FileWriteError, 6> file_write_error_values() noexcept {
   return ::be::EnumTraits<FileWriteError>::values<>();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, FileWriteError constant) {
   if (is_valid(constant)) {
      os << file_write_error_name(constant);
   } else {
      os << static_cast<I64>(static_cast<U8>(constant));
   }
   return os;
}

} // be::util


/* ######################### END OF GENERATED CODE ######################### */
