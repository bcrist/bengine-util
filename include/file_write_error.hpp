#pragma once
#ifndef BE_UTIL_FS_FILE_WRITE_ERROR_HPP_
#define BE_UTIL_FS_FILE_WRITE_ERROR_HPP_

#include "util_fs_autolink.hpp"
#include <be/core/enum_traits.hpp>
#include <be/core/exceptions.hpp>

/*!! include 'file_write_error' !! 56 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
enum class FileWriteError : U8 {
   none = 0,
   unknown_filesystem_error,
   failed_to_create_file,
   file_not_writable,
   disk_full,
   out_of_memory
};

bool is_valid(FileWriteError constant) noexcept;
const char* file_write_error_name(FileWriteError constant) noexcept;
std::array<const FileWriteError, 6> file_write_error_values() noexcept;
std::ostream& operator<<(std::ostream& os, FileWriteError constant);

} // be::util

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct EnumTraits<::be::util::FileWriteError> {
   using type = ::be::util::FileWriteError;
   using underlying_type = typename std::underlying_type<type>::type;

   static constexpr std::size_t count = 6;

   static bool is_valid(type value) {
      return ::be::util::is_valid(value);
   }

   static const char* name(type value) {
      return ::be::util::file_write_error_name(value);
   }

   template <typename C = std::array<const type, count>>
   static C values() {
      return {
         ::be::util::FileWriteError::none,
         ::be::util::FileWriteError::unknown_filesystem_error,
         ::be::util::FileWriteError::failed_to_create_file,
         ::be::util::FileWriteError::file_not_writable,
         ::be::util::FileWriteError::disk_full,
         ::be::util::FileWriteError::out_of_memory,
      };
   }
};

} // be

/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T ignore_error(std::pair<T, FileWriteError> result) {
   return std::move(result.first);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T default_on_error(std::pair<T, FileWriteError> result, T default_value) {
   if (result.second == FileWriteError::none) {
      return std::move(result.first);
   } else {
      return std::move(default_value);
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T throw_on_error(std::pair<T, FileWriteError> result) {
   switch (result.second) {
      case FileWriteError::none:
         return std::move(result.first);
      default:
         throw Recoverable<>(file_write_error_name(result.second));
   }
}

} // be::util

#endif
